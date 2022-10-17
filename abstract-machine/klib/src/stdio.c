#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char *loadInt(char *out, int val);
char *loadStr(char *out, char *str);

int printf(const char *fmt, ...) {
  char out[1024];
  va_list ap;

  va_start(ap, fmt);
  int count = vsprintf(out, fmt, ap);
  va_end(ap);
  
  putstr(out);
  return count;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *begin = out;
  while (*fmt != '\0') {
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
        case 'd':
          out = loadInt(out, va_arg(ap, int));
          break;
        case 's':
          out = loadStr(out, va_arg(ap, char *));
          break;
        case '%':
          *out = '%';
          out++;
          break;
      }
    }
    else {
      *out = *fmt;
      out++;
    }
    fmt++;
  }
  *out = '\0';
  return out - begin;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  int count = vsprintf(out, fmt, ap);
  va_end(ap);

  return count;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

char *loadInt(char *out, int val) {
  if (val < 0) {
    *out = '-';
    out++;
    out = loadInt(out, -val);
  }
  else if (val < 10) {
    *out = val + '0';
    out++;
  }
  else {
    int msd = val;
    int digit = 1;
    while (msd >= 10) {
      msd /= 10;
      digit *= 10;
    }
    *out = msd + '0';
    out++;
    out = loadInt(out, val % (msd * digit));
  }
  return out;
}

char *loadStr(char *out, char *str) {
  while (*str != '\0') {
    *out = *str;
    out++;
    str++;
  }
  return out;
}
#endif
