#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct KeyValuePair {
  char *key;
  char *value;
} KeyValuePair;

static inline int create_folder(const char *_Path) {
#if defined _WIN32
  bool success = CreateDirectory(_Path, NULL);
  if (success == false) {
    DWORD err = GetLastError();
    if (err == ERROR_ALREADY_EXISTS)
      return 1;
    else
      return -1;
  }
#else
  int success = mkdir(_Path, 0777);
  if (success != 0) {
    if (errno == EEXIST)
      return 1;
    else
      return -1;
  }
#endif

  return 0;
}

static inline int compare_strings(const void *a, const void *b) {
  const char *s1 = *(const char **)a;
  const char *s2 = *(const char **)b;
  return strcmp(s1, s2);
}

static inline void super_cool_sort(char **unsorted_list, const size_t count) {
  qsort(unsorted_list, count, sizeof(char *), compare_strings);
}

static inline char *url_encode(const char *str) {
  if (str == NULL)
    return NULL;

  size_t len = strlen(str);
  size_t max_size = len * 3 + 1;

  char *encoded = (char *)malloc(max_size);
  if (encoded == NULL)
    return NULL;

  const char *p = str;
  char *out = encoded;

  while (*p) {
    unsigned char c = (unsigned char)*p;

    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
        (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' ||
        c == '~') {
      *out++ = c;
    } else {
      sprintf(out, "%%%02X", c);
      out += 3;
    }
    p++;
  }

  *out = '\0';
  return encoded;
}
