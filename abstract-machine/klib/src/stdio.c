#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int format_helper_value2str(char *out, uint64_t d, unsigned base_value) {
  int res = 0;
  char temp;
  do {
      *(out + res) = d % base_value > 9 ? d % base_value  + 'a' - 10 : d % base_value + '0';
      res++;
      d /= base_value;
  } while(d);

  *(out + res) = '\0';

  for(int i = 0; i < res / 2; i++) {
      temp = *(out + i);
      *(out + i) = *(out + res - i - 1);
      *(out + res - i - 1) = temp;
  }
  return res;
}

int format_helper_int2str(char *out, int d) {
  int res = 0;
  if(d < 0) {
      *out = '-';
      out++;
      if(d == 0x80000000) {
          strcpy(out, "2147483648");
          return 11;
      }
      res++;
      d = -d;
  }

  res += format_helper_value2str(out, d, 10);
  return res;
}

int format_helper_uint2str(char *out, unsigned int d) {
    return format_helper_value2str(out, d, 10);
}

int format_helper_uint20xstr(char *out, unsigned d) {
    strcpy(out, "0x");
    return format_helper_value2str(out + 2, d, 16) + 2;
} 

int printf(const char *fmt, ...) {
  //panic("Not implemented");
  va_list ap;
  va_start(ap, fmt);
  int res = 0;
  char temp[100], *chp;


  while(*fmt != '\0') {
      if(*fmt == '%') {
          switch (*(++fmt))
          {
          case 'd':
              res += format_helper_int2str(temp, va_arg(ap, int));
              putstr(temp);
              break;
          case 's':
              chp = va_arg(ap, char*);
              res += strlen(chp);
              putstr(chp);
              break;
          case 'c':
              putch(va_arg(ap, int));
              res++;
              break;
          case 'u':
              res += format_helper_uint2str(temp, va_arg(ap, unsigned));
              putstr(temp);
              break;
          case 'x':
              res += format_helper_uint20xstr(temp, va_arg(ap, int));
              putstr(temp);
          default:
              break;
          }
      }
      else {
          putch(*fmt);
          res++;
      }
      fmt++;
  }
  va_end(ap);
  return res;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
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
              break;
          case 'c':
              *(out + res) = va_arg(ap, int);
              res++;
              break;
          case 'u':
              res += format_helper_uint2str(out + res, va_arg(ap, unsigned));
              break;
          case 'x':
              res += format_helper_uint20xstr(out + res, va_arg(ap, int));
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
