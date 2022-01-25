#pragma once
#include <lib/stdint.h>
#include <lib/stddef.h>

/// @brief Locate first occurrence in value @param str Pointer to value @param c Value to locate @param n Size of value @return Pointer to first occurrence
void* memchr(const void* str, int c, size_t n);

/// @brief Compare values @param str1 Pointer to first value @param str2 Pointer to second value @param n Size of value @return Result of comparison; Returns 0 if complete match
int memcmp(const void* str1, const char* str2, size_t n);

/// @brief Copy specified region of memory @param dest Pointer to destination @param src Pointer to source @param n Size to copy @return Pointer to destination
void* memcpy(void* dest, const void* src, size_t n);

/// @brief Move specified region of memory @param dest Pointer to destination @param src Pointer to source @param n Size to copy @return Pointer to destination
void* memmove(void* dest, const void* src, size_t n);

/// @brief Fill specified region of memory with value @param str Pointer to destination @param c Value to fill with @param n Size to fill @return Pointer to destination
void* memset(void* str, int c, size_t n);

/// @brief Contcatenate source to destination string @param dest Pointer to destination @param src Pointer to source @return Pointer to destination
char* strcat(char* dest, const char* src);

/// @brief Contcatenate source to destination string @param dest Pointer to destination @param src Pointer to source @param n Size @return Pointer to destination
char* strncat(char* dest, const char* src, size_t n);

/// @brief Locate first occurrence in string @param str Pointer to string @param c Value to locate @return Pointer to first occurrence
char* strchr(const char* str, int c);

/// @brief Compare string values @param str1 Pointer to first string @param str2 Pointer to second string @return Reslut of comparison; Returns 0 if complete match
int strcmp(const char* str1, const char* str2);

/// @brief Compare string values @param str1 Pointer to first string @param str2 Pointer to second string @param n Size of string @return Result of comparison; Returns 0 if complete match
int strncmp(const char* str1, const char* str2, size_t n);

/// @brief Copy string value @param dest Pointer to destination @param src Pointer to source @return Pointer to destination
char* strcpy(char* dest, const char* src);

/// @brief Copy string value @param dest Pointer to destination @param src Pointer to source @param n Size of string @return Pointer to destination
char* strncpy(char* dest, const char* src, size_t n);

/// @brief Locate values in string @param str1 Pointer to string @param str2 Pointer to string containing values @return Amount of characters read before first occurrence
size_t strcspn(const char* str1, const char* str2);

/// @brief Get length of string @param str Pointer to string @return Length of string
size_t strlen(const char* str);

/// @brief Locate first occurrence in string @param str1 Pointer to string @param str2 Pointer to string containing values @return Pointer to first occurrence; Returns null if unable to locate
char* strpbrk(const char* str1, const char* str2);

/// @brief Locate last occurrence in string @param str Pointer to string @param c Value to locate @return Pointer to last occurrence
char* strrchr(const char* str, int c);

/// @brief Get amount of characters located in string @param str Pointer to string @param Pointer to string containing values @return Amount of characters located
size_t strspn(const char* str1, const char* str2);

/// @brief Locate string withing string @param haystack Pointer to string @param needle Pointer to string to locate @return Pointer to first occurrence in string; Returns null if unable to locate
char* strstr(const char* haystack, const char* needle);

char* strtok(char* str, const char* delim);

size_t strxfrm(char* dest, const char* str, size_t n);

/// @brief Append character to string @param str Pointer to string @param c Character value @return Pointer to string
char* stradd(char* str, char c);

/// @brief Split string @param str Pointer to string @param delim Delimiter character @param count Pointer to array count @return Array of allocated and split strings
char** strsplit(char* str, char delim, int* count);