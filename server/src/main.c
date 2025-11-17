#include "../../libs/utils.h"
#include "../../libs/HTTPServer.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  smw_init();

  HTTPServer *httpServer = NULL;
  if (!HTTPServer_Initialize(http_POST, &httpServer)) {
    printf("Failed to initialize HTTP server\n");
    return -1;
  }

  uint64_t monTime = 0;
  while (smw_getTaskCount() > 0) {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }

  smw_dispose();
  HTTPServer_Dispose(&httpServer);

  return 0;
}
