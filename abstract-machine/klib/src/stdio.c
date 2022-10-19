#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char *loadInt(char *out, int val, int base);
char *loadStr(char *out, char *str);

int printf(const char *fmt, ...) {
  char out[256];
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
          out = loadInt(out, va_arg(ap, int), 10);
          break;
        case 'x':
          out = loadInt(out, va_arg(ap, int), 16);
          break;
        case '0':
          while (*fmt >= '0' && *fmt <= '9')
            fmt++;
          if (*fmt == 'd')
            out = loadInt(out, va_arg(ap, int), 10);
          else if (*fmt == 'x')
            out = loadInt(out, va_arg(ap, int), 16);
          // else
          //   exit(1);
          break;
        case 's':
          out = loadStr(out, va_arg(ap, char *));
          break;
        case '%':
          *out++ = '%';
          break;
      }
    }
    else {
      *out++ = *fmt;
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

char *loadInt(char *out, int val, int base) {
  static char representation[16] = "0123456789ABCDEF";
  if (val < 0) {
    *out++ = '-';
    out = loadInt(out, -val, base);
  }
  else if (val < base) {
    *out++ = representation[val];
  }
  else {
    int msd = val;
    int digit = 1;
    while (msd >= base) {
      msd /= base;
      digit++;
    }
    for (int i = digit - 1; i >= 0; i--) {
      out[i] = representation[val % base];
      val /= base;
    }
    out += digit;
  }
  return out;
}

char *loadStr(char *out, char *str) {
  while (*str != '\0') {
    *out++ = *str++;
  }
  return out;
}
#endif
