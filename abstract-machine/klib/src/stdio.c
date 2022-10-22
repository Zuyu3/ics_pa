#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int format_helper_int2str(char *out, int d) {
  int res = 0;
  if(d < 0) {
      *out = '-';
      out++;
      if(d == 0x80000000) {
          strcpy(out, "2147483648");
          return 11;
      }
      return 1 + format_helper_int2str(out, -d);
  }
  char temp;
  do {
      *(out + res) = d % 10  + '0';
      res++;
      d /= 10;
  } while(d);

  *(out + res) = '\0';

  for(int i = 0; i < res / 2; i++) {
      temp = *(out + i);
      *(out + i) = *(out + res - i - 1);
      *(out + res - i - 1) = temp;
  }
  return res;
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  //panic("Not implemented");
  int res = 0;
  va_list ap;
  va_start(ap, fmt);

  while(*fmt != '\0') {
      if(*fmt == '%') {
          switch (*(++fmt))
          {
          case 'd':
              res += format_helper_int2str(out + res, va_arg(ap, int));
              break;
          case 's':
              strcpy(out + res, va_arg(ap, char*));
              res += strlen(out + res);
              if(strlen(out) != 8)
              panic("wrong index");
              break;
          case 'c':
              *(out + res) = va_arg(ap, int);
              res++;
              break;
          default:
              break;
          }
      }
      else {
          *(out + res) = *fmt;
          res++;
      }
      fmt++;
  }
  va_end(ap);
  *(out + res) = '\0';
  return res;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
