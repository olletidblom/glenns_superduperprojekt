#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <time.h>

#include <ctype.h>

static uint64_t SystemMonotonicMS() {
  long ms;
  time_t s;

  struct timespec spec;
  clock_gettime(CLOCK_MONOTONIC, &spec);
  s = spec.tv_sec;
  ms = (spec.tv_nsec / 1000000);

  uint64_t result = s;
  result *= 1000;
  result += ms;

  return result;
}

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
#endif
