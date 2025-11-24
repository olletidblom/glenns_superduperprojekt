#include "../src/HTTPServerConnection.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// AFL entry point - reads from stdin
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
    conn.task = NULL;
    conn.context = NULL;

    // Copy input into recv_buffer
    size_t copy_size = size < sizeof(conn.recv_buffer) - 1 ? size : sizeof(conn.recv_buffer) - 1;
    memcpy(conn.recv_buffer, buffer, copy_size);
    conn.recv_buffer[copy_size] = '\0';
    conn.recv_buffer_length = copy_size;

    // Test the parsing function
    HTTPServerConnection_ParseHeader(&conn);

    // Cleanup
    free(conn.method_url);
    free(conn.host);
    free(conn.url_path);
    free(conn.url);

    return 0;
}
