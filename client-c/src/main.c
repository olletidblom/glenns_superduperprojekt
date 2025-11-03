#include "../../libs/HTTP.h"
#include "../../libs/TCPClient.h"
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main()
{
    smw_init();
    HTTP_run(http_GET, HTTP_work);

    while(smw_getTaskCount() > 0)
    {
        smw_work(SystemMonotonicMS());
    }

    smw_dispose();

    return 0;
}