#ifndef UTILS_H
#define UTILS_H

#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <time.h>

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

static inline char* my_strdup(const char* str)
{
	char* copy = (char*)malloc(strlen(str) + 1);
	if(copy == NULL)
		return NULL;

	strcpy(copy, str);
	return copy;
}

static inline int create_folder(const char* _Path)
{
	#if defined _WIN32
		bool success = CreateDirectory(_Path, NULL);
		if(success == false)
		{
			DWORD err = GetLastError();
			if(err == ERROR_ALREADY_EXISTS)
				return 1;
			else
				return -1;

		}
	#else
		int success = mkdir(_Path, 0777);
		if(success != 0)
		{
			if(errno == EEXIST)
				return 1;
			else
				return -1;

		}
	#endif

	return 0;
}



static inline uint64_t SystemMonotonicMS() {
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

#endif
