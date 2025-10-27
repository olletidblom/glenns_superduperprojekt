#ifndef HTTP_H
#define HTTP_H

#include <stddef.h>

int HTTP_Get(const char* hostname, const char* path, char* buffer, size_t buffer_size);

#endif