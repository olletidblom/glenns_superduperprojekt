#include "../../libs/HTTP.h"
#include "../../libs/TCPClient.h"
#include "../../libs/utils.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    smw_init();
    HTTPClient* client = NULL;
    HTTP_Initialize(http_GET, &client);

  while (smw_getTaskCount() > 0) {
    smw_work(SystemMonotonicMS());
  }

  smw_dispose();

  return 0;
}
