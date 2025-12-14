#include "../../libs/utils.h"
#include "HTTPServer.h"
#include "Handlers/WeatherHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int shutdown = 0;

void handle_sigint()
{
  shutdown = 1;
}


int main() {
  signal(SIGINT, handle_sigint);
  smw_init();

  HTTPServer *httpServer = NULL;
  if (HTTPServer_Initialize(http_POST, &httpServer) != 0) {
    printf("Failed to initialize HTTP server\n");
    return -1;
  }


  uint64_t monTime = 0;
  while (smw_getTaskCount() > 0 && shutdown == 0) {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }

  HTTPServer_Dispose(&httpServer);
  smw_dispose();
  

  return 0;
}
