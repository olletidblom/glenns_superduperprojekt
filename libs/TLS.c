#include "TLS.h"
#include <stdio.h>
#include <string.h>

/*
mbedtls_net_context server_fd;        ----tls_connection_init----
mbedtls_entropy_context entropy;        ----tls_client_init----
mbedtls_ctr_drbg_context ctr_drbg;      ----tls_client_init----
mbedtls_ssl_context ssl;              ----tls_connection_init----
mbedtls_ssl_config conf;                ----tls_client_init----

mbedtls_ssl_init( &ssl );             ----tls_connection_init----
mbedtls_net_init( &server_fd );       ----tls_connection_init----
mbedtls_ssl_config_init( &conf );       ----tls_client_init----
mbedtls_x509_crt_init( &cacert );       ----tls_client_init----
mbedtls_ctr_drbg_init( &ctr_drbg );     ----tls_client_init----
*/

int tls_client_init(TLSClientContext *ctx, const char* ca_cert_file)
{
    int ret;
    const char *pers = "tls_client";

    // Initialize structures
    mbedtls_ssl_config_init(&ctx->conf);
    mbedtls_x509_crt_init(&ctx->cacert);
    mbedtls_ctr_drbg_init(&ctx->ctr_drbg);
    mbedtls_entropy_init(&ctx->entropy);

    // Initialize PSA crypto (required in mbedtls 4.0)
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("Failed in psa_crypto_init: %d\n", (int)status);
        return -1;
    }
    
    // Seed the RNG
    if ((ret = mbedtls_ctr_drbg_seed(&ctx->ctr_drbg, mbedtls_entropy_func, &ctx->entropy,
                                    (const unsigned char *)pers, strlen(pers))) != 0)
    {
        printf("Failed in mbedtls_ctr_drbg_seed returned: %d\n", ret);
        return -1;
    }

    // Load CA certificate if provided
    if (ca_cert_file != NULL) {
        ret = mbedtls_x509_crt_parse_file(&ctx->cacert, ca_cert_file);
        if (ret != 0)
        {
            printf("Failed in mbedtls_x509_crt_parse_file returned: %d\n", ret);
            return -1;
        }
    }

    // Set up SSL config defaults
    ret = mbedtls_ssl_config_defaults(&ctx->conf,
                                   MBEDTLS_SSL_IS_CLIENT,
                                   MBEDTLS_SSL_TRANSPORT_STREAM,
                                   MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0)
    {
        printf("Failed in mbedtls_ssl_config_defaults returned: %d\n", ret);
        return -1;
    }

    // Note: mbedtls_ssl_conf_rng() not needed in mbedtls 4.0 - handled by config_defaults

    // Set CA certificate for verification
    if (ca_cert_file != NULL) {
        mbedtls_ssl_conf_ca_chain(&ctx->conf, &ctx->cacert, NULL);
        mbedtls_ssl_conf_authmode(&ctx->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    } else {
        mbedtls_ssl_conf_authmode(&ctx->conf, MBEDTLS_SSL_VERIFY_NONE);         // No verification without CA
    }

    printf("TLS client initialized successfully\n");

    return 0;
}

int tls_client_connect(TLSClientConnection *conn, TLSClientContext *ctx, const char* host, const char* port)
{
    int ret;
    mbedtls_ssl_init(&conn->ssl);
    mbedtls_net_init(&conn->server_fd);

    printf("connecting to %s:%s...\n", host, port);

    if ((ret = mbedtls_net_connect(&conn->server_fd, host, port, MBEDTLS_NET_PROTO_TCP)) != 0)
    {
        printf("Failed in mbedtls_net_connect returned: %d\n", ret);
        return -1;
    }

    if ((ret = mbedtls_ssl_setup(&conn->ssl, &ctx->conf)) != 0)
    {
        printf("Failed in mbedtls_ssl_setup returned: %d\n", ret);
        return -1;
    }

    if ((ret = mbedtls_ssl_set_hostname(&conn->ssl, host)) != 0)
    {
        printf("Failed in mbedtls_ssl_set_hostname returned: %d\n", ret);
        return -1;
    }
    mbedtls_ssl_set_bio(&conn->ssl, &conn->server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);


    printf("Performing TLS handshake...\n");
    while ((ret = mbedtls_ssl_handshake(&conn->ssl)) != 0)
    {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf("Failed in mbedtls_ssl_handshake returned: %d\n", ret);
            return -1;
            }
    }

    uint32_t flags = mbedtls_ssl_get_verify_result(&conn->ssl);
    if (flags != 0)
    {
        printf("Certificate verification failed, flags: %08x\n", flags);
        return -1;
    }

    printf("TLS connection established successfully\n");
    return 0;
}

ssize_t tls_client_read(TLSClientConnection *conn, void* buffer, size_t len)
{
    int ret = mbedtls_ssl_read(&conn->ssl, buffer, len);
    
    // Handle graceful close
    if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
        return 0;  // Connection closed by peer
    }
    
    // Handle temporary conditions (non-blocking)
    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        return 0;  // Try again
    }
    
    // Handle real errors
    if (ret < 0) {
        printf("mbedtls_ssl_read failed: -0x%04x\n", -ret);
        return -1;
    }
    
    return ret;
}

ssize_t tls_client_write(TLSClientConnection *conn, const void* data, size_t len)
{

/*
For reading from the network layer:

ret = read( server_fd, buf, len );

becomes

 ret = mbedtls_ssl_read( &ssl, buf, len );
*/

    int ret = mbedtls_ssl_write(&conn->ssl, data, len);
    if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
        printf("mbedtls_ssl_write failed: -0x%04x\n", -ret);
        return -1;
    }
    return ret;
}


void tls_client_disconnect(TLSClientConnection *conn)
{
    mbedtls_ssl_close_notify(&conn->ssl);
    mbedtls_ssl_free(&conn->ssl);
    mbedtls_net_free(&conn->server_fd);
}

void tls_client_free(TLSClientContext *ctx)
{
    mbedtls_x509_crt_free(&ctx->cacert);
    mbedtls_ssl_config_free(&ctx->conf);
    mbedtls_ctr_drbg_free(&ctx->ctr_drbg);
    mbedtls_entropy_free(&ctx->entropy);
    mbedtls_psa_crypto_free();
}


            // Server functions

int tls_server_init(TLSServerContext *ctx, const char* cert_file, const char* key_file);
//Initialize random number generator
//Load server certificate (.crt file)
//Load server private key (.key file)
//Configure TLS settings (TLS version, ciphers, etc.)
//Think of it as: Loading your server's identity card

int tls_connection_init(TLSConnection *conn, TLSServerContext *ctx, int client_socket);
//Create a new SSL context for this specific client
//Link it to the server config
//Tell mbedtls which socket to use
//Think of it as: Starting a conversation with one client

int tls_handshake(TLSConnection *conn);
//Negotiate encryption (client and server agree on how to encrypt)
//Exchange keys
//Think of it as: The secret handshake before talking



void tls_connection_free(TLSConnection *conn);
//Free SSL context for this client
void tls_server_free(TLSServerContext *ctx);
//Free certificate, keys, config
//Called when server shuts down