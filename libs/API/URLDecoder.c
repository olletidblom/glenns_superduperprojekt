#include "URLDecoder.h"


static int hex(char c)
{
    if ('0' <= c && c <= '9') return c - '0';
    if ('a' <= c && c <= 'f') return c - 'a' + 10;
    if ('A' <= c && c <= 'F') return c - 'A' + 10;
    return -1;
}

void url_decode_inplace(char *s)
{
    char *out = s;

    while (*s) {
        if (*s == '%' &&
            isxdigit((unsigned char)s[1]) &&
            isxdigit((unsigned char)s[2])) {

            *out++ = (char)((hex(s[1]) << 4) | hex(s[2]));
            s += 3;
        } else if (*s == '+') {
            *out++ = ' ';
            s++;
        } else {
            *out++ = *s++;
        }
    }
    *out = '\0';
}