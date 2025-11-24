#include "../../libs/utils.h"
#include "HTTPServer.h"
#include "Handlers/weather_handler.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  smw_init();

  HTTPServer *httpServer = NULL;
  if (HTTPServer_Initialize(http_POST, &httpServer) != 0) {
    printf("Failed to initialize HTTP server\n");
    return -1;
  }

  // Register routes

  

  uint64_t monTime = 0;
  while (smw_getTaskCount() > 0) {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }

  HTTPServer_Dispose(&httpServer);
  smw_dispose();
  

  return 0;
}
