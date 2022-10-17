#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t l = 0;
  while (s[l] != '\0')
    l++;
  return l;
}

char *strcpy(char *dst, const char *src) {
  size_t l = 0;
  while (src[l] != '\0') {
    dst[l] = src[l];
    l++;
  }
  dst[l] = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t l = strlen(src);
  for (int i = 0; i < n; i++) {
    if (i > l - 1)
      dst[i] = ' ';
    else
      dst[i] = src[i];
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  char *begin = dst;
  while (*dst != '\0')
    dst++;
  strcpy(dst, src);
  return begin;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s2 != '\0') {
    if (*s1 != *s2)
      return *s1 - *s2;
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  for (size_t i = 0; i < n; i++)
    *(uint8_t *)(s + i) = *(uint8_t *)&c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  for (size_t i = 0; i < n; i++) {
    uint8_t val1 = *(uint8_t *)(s1 + i);
    uint8_t val2 = *(uint8_t *)(s2 + i);
    if (val1 != val2)
      return val1 - val2;
  }
  return 0;
}

#endif
