#include <lib/string.h>
#include <lib/nos.h>

void* memchr(const void* str, int c, size_t n) { return __memchr(str, c, n); }

int memcmp(const void* str1, const char* str2, size_t n) { return __memcmp(str1, str2, n); }

void* memcpy(void* dest, const void* src, size_t n) { return __memcpy(dest, src, n); }

void* memmove(void* dest, const void* src, size_t n) { return __memmove(dest, src, n); }

void* memset(void* str, int c, size_t n) { return __memset(str, c, n); }

char* strcat(char* dest, const char* src) { return __strcat(dest, src); }

char* strncat(char* dest, const char* src, size_t n) { return __strncat(dest, src, n); }

char* strchr(const char* str, int c) { return __strchr(str, c); }

int strcmp(const char* str1, const char* str2) { return __strcmp(str1, str2); }

int strncmp(const char* str1, const char* str2, size_t n) { return __strncmp(str1, str2, n); }

char* strcpy(char* dest, const char* src) { return __strcpy(dest, src); }

char* strncpy(char* dest, const char* src, size_t n) { return __strncpy(dest, src, n); }

size_t strcspn(const char* str1, const char* str2) { return __strcspn(str1, str2); }

size_t strlen(const char* str) { return __strlen(str); }

char* strpbrk(const char* str1, const char* str2) { return __strpbrk(str1, str2); }

char* strrchr(const char* str, int c) { return __strrchr(str, c); }

size_t strspn(const char* str1, const char* str2) { return __strspn(str1, str2); }

char* strstr(const char* haystack, const char* needle) { return __strstr(haystack, needle); }

char* strtok(char* str, const char* delim) { return __strtok(str, delim); }

size_t strxfrm(char* dest, const char* str, size_t n) { return __strxfrm(dest, str, n); }

char* stradd(char* str, char c) { return __stradd(str, c); }

char** strsplit(char* str, char delim, int* count) { return __strsplit(str, delim, count); }