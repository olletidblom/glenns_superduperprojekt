#ifndef __HANDLER_H__
#define __HANDLER_H__

#include "../HTTPServerConnection.h"

typedef struct HTTPServerConnection HTTPServerConnection;

// Now we can use the full type
typedef char* (*RequestHandler)(HTTPServerConnection* connection);

typedef struct {
    char* path;
    char* method;
    RequestHandler handler;
} Route;





#endif // __HANDLER_H__