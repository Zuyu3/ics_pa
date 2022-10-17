#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  panic("Not implemented");
  size_t res = 0;
  while(*s != '\0'){
    res++;
    s++;
  }
  return res;
}

char *strcpy(char *dst, const char *src) {
  char *end_of_dst = dst;
  while(*src != '\0') {
    *end_of_dst = *src;
    end_of_dst++;
    src++;
  }
  end_of_dst = '\0';
  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  char *end_of_dst = dst;
  while(*end_of_dst != '\0') {
    end_of_dst++;
  }
  
  while(*src != '\0') {
    *end_of_dst = *src;
    src++;
    end_of_dst++;
  }
  end_of_dst = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while(*s1 != '\0' && *s2 != '\0') {
    if(*s1 != *s2) 
      return *s1 - *s2;
    else {
      s1++;
      s2++;
    }
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  void *res = s;
  while(n--) {
    *(char*)s = c;
    s++;
  }
  return res;
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  for(int i = 0; i < n; i++) {
    if(*(char*)s1 != *(char*)s2)
      return s1 - s2;
    s1++;
    s2++;
  }
  return 0;
}

#endif
