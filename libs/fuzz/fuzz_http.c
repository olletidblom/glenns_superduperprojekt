
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sanitizer/lsan_interface.h>
// Deklarera funktionen från din server

typedef struct HTTPServerConnection HTTPServerConnection;

HTTPServerConnection* HTTPServerConnection_ParseRequest(const uint8_t *data, size_t len);

void HTTPServerConnection_Dispose(HTTPServerConnection **connection);


int main(void) {

    uint8_t buf[65536];
    ssize_t n = read(0, buf, sizeof(buf));
    if (n <= 0) return 0;

    // KÖR FUNKTIONEN
    HTTPServerConnection *srv = HTTPServerConnection_ParseRequest(buf, n);

    // TYPISK LEAK-PROV

    // Tvinga LeakSanitizer att rapportera innan exit
    HTTPServerConnection_Dispose(&srv);

    // normal cleanup (om du har disposal)
    //HTTPServerConnection_Dispose(&srv);

    return 0;
}