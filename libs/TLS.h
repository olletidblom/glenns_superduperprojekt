#ifndef TLS_H
#define TLS_H

#include "mbedtls/ssl.h"
#include "mbedtls/x509.h"   // certificates
#include "mbedtls/pk.h"     // private keys
#include "mbedtls/net_sockets.h"  // for mbedtls_net_send/recv
#include "mbedtls/private/entropy.h"
#include "mbedtls/private/ctr_drbg.h"
#include "psa/crypto.h"     // PSA Crypto API (required in mbedtls 4.0)
#include <sys/types.h>      // ssize_t

// ========== SERVER STRUCTS ==========

typedef struct {        
                        // TLS Server Context - holds server certificates, keys, and configurations
    mbedtls_entropy_context entropy;    //Gather randomness from OS
    mbedtls_ctr_drbg_context ctr_drbg;  //Generate random numbers
    mbedtls_ssl_config conf;            //SSL configuration TLS configuration
    mbedtls_x509_crt cert;              // server certificate
    mbedtls_pk_context pkey;            // server private key

} TLSServerContext;

//TLS connection - per-client SSL session
typedef struct {

    mbedtls_ssl_context ssl;
    int socket_fd;
    //mbedtls_net_context server_fd;

} TLSConnection;

// ========== CLIENT STRUCTS ==========
typedef struct {

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;        // CA certs to verify server

} TLSClientContext;

typedef struct {
    mbedtls_ssl_context ssl;
    mbedtls_net_context server_fd;  // Client socket

} TLSClientConnection;

// Server functions
int tls_server_init(TLSServerContext *ctx, const char* cert_file, const char* key_file);
int tls_connection_init(TLSConnection *conn, TLSServerContext *ctx, int client_socket);
int tls_handshake(TLSConnection *conn);
ssize_t tls_read(TLSConnection *conn, void* buffer, size_t len);
ssize_t tls_write(TLSConnection *conn, const void* data, size_t len);
void tls_connection_free(TLSConnection *conn);
void tls_server_free(TLSServerContext *ctx);

// Client functions
int tls_client_init(TLSClientContext *ctx, const char* ca_cert_file);
int tls_client_connect(TLSClientConnection *conn, TLSClientContext *ctx, const char* host, const char* port);
ssize_t tls_client_read(TLSClientConnection *conn, void* buffer, size_t len);
ssize_t tls_client_write(TLSClientConnection *conn, const void* data, size_t len);
void tls_client_disconnect(TLSClientConnection *conn);
void tls_client_free(TLSClientContext *ctx);

#endif // TLS_H