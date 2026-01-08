#ifndef URLDECODER_H
#define URLDECODER_H



#include <ctype.h>


static int hex(char c);

void url_decode_inplace(char *s);


#endif