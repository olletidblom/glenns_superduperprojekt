#include <stdio.h>
#include <stdlib.h>
#include "server.h"
#include "../../libs/utils.h"

int main() {
  smw_init();

  server_run(server);
  uint64_t monTime = 0;
  while(smw_getTaskCount() > 0)
  {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }
  
  smw_dispose();

  return 0;
}
