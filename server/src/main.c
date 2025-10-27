#include <stdio.h>
#include <stdlib.h>
#include "server.h"



int main() {
  smw_init();

  server_s* srv = server_run(server);
  server_listen_accept();

  uint64_t monTime = 0;
  while(smw_getTaskCount() > 0)
  {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }
  
  smw_dispose();
  server_dispose(&srv);

  return 0;
}
