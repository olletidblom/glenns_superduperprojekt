#include "../src/HTTPServerConnection.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Execution depth counters (global so we can track across runs)
static unsigned long stage_parse_called = 0;
static unsigned long stage_parse_success = 0;
static unsigned long stage_buildurl = 0;
static unsigned long stage_handlerequest = 0;

// AFL entry point - tests full HTTP request flow
int main(int argc, char **argv) {
    uint8_t buffer[8192];
    
    // Read input from stdin (AFL will provide this)
    ssize_t size = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
    
    if (size < 1) {
        return 0;
    }
    
    buffer[size] = '\0';

    // Create a connection structure
    HTTPServerConnection conn;
    memset(&conn, 0, sizeof(HTTPServerConnection));
    
    conn.socket = 999;  // Mock socket
    conn.state = HTTPServerConnection_State_Read_Make_URL;
    conn.recv_buffer_length = 0;
    conn.content_length = 0;
    conn.method_url = NULL;
    conn.host = NULL;
    conn.url_path = NULL;
    conn.url = NULL;
    conn.request_body = NULL;
    conn.task = NULL;
    conn.context = NULL;

    // Copy input into recv_buffer
    size_t copy_size = size < sizeof(conn.recv_buffer) - 1 ? size : sizeof(conn.recv_buffer) - 1;
    memcpy(conn.recv_buffer, buffer, copy_size);
    conn.recv_buffer[copy_size] = '\0';
    conn.recv_buffer_length = copy_size;

    // Test full flow: ParseHeader -> BuildURL -> HandleRequest
    stage_parse_called++;
    int parse_result = HTTPServerConnection_ParseHeader(&conn);
    
    if (parse_result == 0) {
        stage_parse_success++;
        // Only proceed if parsing succeeded
        stage_buildurl++;
        HTTPServerConnection_BuildURL(&conn);
        
        stage_handlerequest++;
        HTTPServerConnection_HandleRequest(&conn);
    }

    // Print stats every 10000 executions
    if (stage_parse_called % 10000 == 0) {
        fprintf(stderr, "\n[STATS] Total: %lu | ParseOK: %lu (%.1f%%) | BuildURL: %lu | HandleReq: %lu\n",
                stage_parse_called,
                stage_parse_success,
                (stage_parse_success * 100.0) / stage_parse_called,
                stage_buildurl,
                stage_handlerequest);
    }

    // Cleanup
    free(conn.method_url);
    free(conn.host);
    free(conn.url_path);
    free(conn.url);
    free(conn.request_body);

    return 0;
}
