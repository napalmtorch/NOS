#pragma once
#include <lib/stdint.h>
#include <lib/stddef.h>
#include <lib/stdarg.h>
#include <lib/stdlib.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/ctype.h>
#include <lib/math.h>
#include <lib/time.h>
#include <lib/graphics/font.h>
#include <lib/graphics/color.h>

typedef enum
{
    SYSCALL_VPRINTF = 1,
    SYSCALL_METHODS,
    SYSCALL_VBEMODE,
} SYSCALL;

typedef struct
{
    uint32_t address;
    char     section[32];
    char     name[128 - sizeof(uint32_t)];
} objdump_symbol_t;

// stdlib
extern void*    (*__malloc)(size_t);
extern void*    (*__malloc_c)(size_t);
extern void*    (*__malloc_a)(size_t, uint8_t);
extern void     (*__free)(void*);
extern void     (*__freearray)(void**, uint32_t);
extern int      (*__atoi)(const char*);
extern uint32_t (*__atol)(const char*);
extern uint32_t (*__atox)(const char*);
extern float    (*__atof)(const char*);
extern int      (*__rand)(void);
extern void     (*__yield)(void);
extern void     (*__exit)(int);
extern char*    (*__itoa)(int, char*, int);
extern char*    (*__ltoa)(uint32_t, char*, int);
extern char*    (*__strhex)(uint32_t, char*, bool, uint8_t);
extern int      (*__system)(char*);

// stdio
extern int (*__vprintf)(const char*, va_list);

// string
extern void*  (*__memchr)(const void*, int, size_t);
extern int    (*__memcmp)(const void*, const void*, size_t);
extern void*  (*__memcpy)(void*, const void*, size_t);
extern void*  (*__memmove)(void*, const void*, size_t);
extern void*  (*__memset)(void*, int, size_t);
extern char*  (*__strcat)(char*, const char*);
extern char*  (*__strncat)(char*, const char*, size_t);
extern char*  (*__strchr)(const char*, int);
extern int    (*__strcmp)(const char*, const char*);
extern int    (*__strncmp)(const char*, const char*, size_t);
extern char*  (*__strcpy)(char*, const char*);
extern char*  (*__strncpy)(char*, const char*, size_t);
extern size_t (*__strcspn)(const char*, const char*);
extern size_t (*__strlen)(const char*);
extern char*  (*__strpbrk)(const char*, const char*);
extern char*  (*__strrchr)(const char*, int c);
extern size_t (*__strspn)(const char*, const char*);
extern char*  (*__strstr)(const char*, const char*);
extern char*  (*__strtok)(char*, const char*);
extern size_t (*__strxfrm)(char*, const char*, size_t);
extern char*  (*__stradd)(char* str, char c);
extern char** (*__strsplit)(char*, char, int*);

// ctype
extern int (*__isalnum)(int);
extern int (*__isalpha)(int);
extern int (*__iscntrl)(int);
extern int (*__isdigit)(int);
extern int (*__isgraph)(int);
extern int (*__islower)(int);
extern int (*__isprint)(int);
extern int (*__ispunct)(int);
extern int (*__isspace)(int);
extern int (*__isupper)(int);
extern int (*__isxdigit)(int);
extern int (*__tolower)(int);
extern int (*__toupper)(int);

// time
extern char*  (*__timestr)(time_t, bool, bool);
extern time_t (*__timenow)(void);

// font
extern const uint8_t* __FONTDATA_DEFAULT;

void lib_init();
objdump_symbol_t lib_fetch(char* name, uint8_t section);

int _vprintf(const char* str, va_list args);
int _printf(const char* str, ...);
bool _stringcomp(char* str1, char* str2);