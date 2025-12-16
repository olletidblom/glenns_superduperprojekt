#include "../../libs/utils.h"
#include "WeatherServer.h"
#include "HTTPServer.h"
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

  WeatherServer *weatherServer = NULL;
  if (WeatherServer_Initialize(&weatherServer) != 0) {
    printf("Failed to initialize Weather server\n");
    return -1;
  }

  uint64_t monTime = 0;
  while (smw_getTaskCount() > 0 && shutdown == 0) {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }

  WeatherServer_Dispose(&weatherServer);
  smw_dispose();  

  return 0;
}
