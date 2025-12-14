#ifndef WRAPPER_H
#define WRAPPER_H

#include <stdint.h>
#include <stddef.h>


int HTTPConnection_Read(int socket, uint8_t *buffer, size_t length);


int HTTPConnection_Write(int socket, char *buffer, size_t length);



#endif