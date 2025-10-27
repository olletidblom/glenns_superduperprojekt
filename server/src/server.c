// server.c - un micro-serveur qui accepte une connexion client, attend un message, et y répond
// server.c - micro-server allowing client connection, expect a message and answers it
#include "server.h"
#include "../../libs/HTTP.h"
#include "../../libs/TCPServer.h"
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>


#define PORT 8080  // Server port
#define BACKLOG 10 // maximum connection at a time


static TCPServer g_TCPServer;


bool server_init(server_s** srv)
{
    if(srv == NULL)
    return false;

    server_s* _Srv = (server_s*)malloc(sizeof(server_s));

    if(_Srv == NULL)
    return false;

    _Srv->task = smw_create_task(_Srv, server);


    *srv = _Srv;
    return true;
}

void server_listen_accept()
{
    printf("---- SERVER ----\n\n");

    tcpserver_listen(&g_TCPServer, PORT, BACKLOG);
    
    tcpserver_accept(&g_TCPServer);
}

void server(void* _Context)
{
    server_s* _Srv = (server_s*)_Context;

    tcpserver_work(&g_TCPServer);

    
}

server_s* server_run(void (*_Callback)())
{
    server_s* srv = NULL;

    if(!server_init(&srv))
    return -1;

    srv->callback = _Callback;

    
    return srv;

}

void server_dispose(server_s** srv)
{
    if(srv == NULL || *srv == NULL)
    return;

    server_s* _Srv = *srv;

    if(_Srv->task != NULL)
    smw_destroy_task(_Srv->task);

    tcpserver_dispose(&g_TCPServer);

    free(_Srv);
    *srv = NULL;
}