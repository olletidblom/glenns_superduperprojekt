#ifndef SERVER_H
#define SERVER_H

#include "../../libs/smw.h"

typedef struct
{
    smw_task* task;
    void (*callback)();
}server_s;

void server_listen_accept();

void server();

int server_run(void (*_Callback)());

void server_dispose(server_s** srv);



#endif