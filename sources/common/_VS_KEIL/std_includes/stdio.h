#pragma once

int sprintf(char *dest, char *src, ...);
int snprintf(char *, int, const char *, ...);
int sscanf(void *, void *, ...);
int vsprintf(char *__restrict, const char *__restrict, struct va_list);
int vsnprintf(char* buffer, size_t buf_size, const char* format, va_list vlist);
