/*#include "API.h"
#include "HTTP.h"
#include "TCPClient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to URL encode strings (basic implementation)
char* API_url_encode(const char* input)
{
    if (input == NULL) return NULL;
    
    size_t input_len = strlen(input);
    char* encoded = malloc(input_len * 3 + 1); // Worst case: every char becomes %XX
    if (encoded == NULL) return NULL;
    
    int j = 0;
    for (size_t i = 0; i < input_len; i++) {
        if (input[i] == ' ') {
            encoded[j++] = '%';
            encoded[j++] = '2';
            encoded[j++] = '0';
        } else if (input[i] == '&') {
            encoded[j++] = '%';
            encoded[j++] = '2';
            encoded[j++] = '6';
        } else {
            encoded[j++] = input[i];
        }
    }
    encoded[j] = '\0';
    
    return encoded;
}

char* API_get_geocoding_json(const char* city_name, double latitude, double longitude)
{
    if (city_name == NULL) {
        printf("Error: city_name is NULL\n");
        return NULL;
    }
    
    printf("Getting geocoding JSON for: %s\n", city_name);
    
    // Create TCP client for geocoding API
    TCPClient* tcp_client = malloc(sizeof(TCPClient));
    if (tcp_client == NULL) {
        printf("Error: Failed to allocate TCP client\n");
        return NULL;
    }
    
    tcp_init(tcp_client, "geocoding-api.open-meteo.com", 80);
    
    // Create HTTP client
    HTTPClient* http_client = malloc(sizeof(HTTPClient));
    if (http_client == NULL) {
        printf("Error: Failed to allocate HTTP client\n");
        free(tcp_client);
        return NULL;
    }
    
    // Initialize HTTP client
    http_client->task = NULL;
    http_client->status = http_client_initialized;
    http_client->method = http_GET;
    http_client->tcp_client = tcp_client;
    http_client->HTTP_response = NULL;
    http_client->url = NULL;
    http_client->response_code = 0;
    
    // URL encode the city name
    char* encoded_name = API_url_encode(city_name);
    if (encoded_name == NULL) {
        printf("Error: Failed to encode city name\n");
        free(http_client);
        free(tcp_client);
        return NULL;
    }
    
    // Build HTTP request - requesting only 1 result (closest match) to minimize response size
    char request[1024];
    snprintf(request, sizeof(request), 
        "GET /v1/search?name=%s&latitude=%f&longitude=%f&count=1&language=en&format=json HTTP/1.1\r\n" 
        "Host: %s\r\n" 
        "Connection: close\r\n" 
        "\r\n",
        encoded_name, latitude, longitude, tcp_client->hostname);
    
    free(encoded_name);
    
    printf("Connecting to geocoding API...\n");
    
    // Connect and send request
    if (HTTP_Connect(http_client) < 0) {
        printf("Error: Failed to connect to geocoding API\n");
        free(http_client);
        free(tcp_client);
        return NULL;
    }
    
    printf("Sending request...\n");
    
    if (HTTP_Write(http_client, request, strlen(request)) < 0) {
        printf("Error: Failed to send geocoding request\n");
        free(http_client);
        free(tcp_client);
        return NULL;
    }
    
    printf("Reading response...\n");
    
    if (HTTP_Read(http_client) < 0) {
        printf("Error: Failed to read geocoding response\n");
        free(http_client);
        free(tcp_client);
        return NULL;
    }
    
    // Find JSON body in HTTP response
    char* json_start = strstr(http_client->HTTP_response, "\r\n\r\n");
    if (json_start == NULL) {
        json_start = strstr(http_client->HTTP_response, "\n\n");
        if (json_start == NULL) {
            printf("Error: Could not find JSON body in response\n");
            if (http_client->HTTP_response != NULL) {
                free(http_client->HTTP_response);
            }
            free(http_client);
            free(tcp_client);
            return NULL;
        }
        json_start += 2;
    } else {
        json_start += 4;
    }
    
    // Copy JSON to new string
    char* json_response = NULL;
    if (json_start != NULL && strlen(json_start) > 0) {
        size_t json_len = strlen(json_start);
        json_response = malloc(json_len + 1);
        if (json_response != NULL) {
            strcpy(json_response, json_start);
            printf("Successfully extracted JSON (%zu bytes)\n", json_len);
        }
    }
    
    // Clean up HTTP client
    if (http_client->HTTP_response != NULL) {
        free(http_client->HTTP_response);
    }
    free(http_client);
    free(tcp_client);
    
    return json_response;
}

// Free JSON data
void API_free_json(char* json_data)
{
    if (json_data != NULL) {
        free(json_data);
    }
}*/
