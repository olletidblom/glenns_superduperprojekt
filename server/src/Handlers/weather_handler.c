#include "../HTTPServerConnection.h"
#include "handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* Handle_Weather(HTTPServerConnection* connection)
{
    printf("Weather handler called\n");
    return strdup("{\"temperature\":25,\"condition\":\"sunny\",\"location\":\"Stockholm\"}");
}

char* Handle_UsersGET(HTTPServerConnection* connection)
{
    printf("Users GET handler called\n");
    return strdup("{\"users\":[{\"id\":1,\"name\":\"Alice\"},{\"id\":2,\"name\":\"Bob\"}]}");
}

char* Handle_UsersPOST(HTTPServerConnection* connection)
{
    printf("Users POST handler called - received: %s\n",
        connection->request_body ? connection->request_body : "no body");
    // Here you would normally process conn->request_body
    return strdup("{\"status\":\"User created successfully\"}");
}