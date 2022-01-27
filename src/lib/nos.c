#include <lib/nos.h>

void*    (*__malloc)(size_t) = NULL;
void*    (*__malloc_c)(size_t) = NULL;
void*    (*__malloc_a)(size_t, uint8_t) = NULL;
void     (*__free)(void*) = NULL;
void     (*__freearray)(void**, uint32_t) = NULL;
int      (*__atoi)(const char*) = NULL;
uint32_t (*__atol)(const char*) = NULL;
uint32_t (*__atox)(const char*) = NULL;
float    (*__atof)(const char*) = NULL;
int      (*__rand)(void) = NULL;
void     (*__yield)(void) = NULL;
void     (*__exit)(int) = NULL;
char*    (*__itoa)(int, char*, int) = NULL;
char*    (*__ltoa)(uint32_t, char*, int) = NULL;
char*    (*__strhex)(uint32_t, char*, bool, uint8_t) = NULL;
int      (*__system)(char*) = NULL;

int (*__vprintf)(const char*, va_list);

// string
void*  (*__memchr)(const void*, int, size_t);
int    (*__memcmp)(const void*, const void*, size_t);
void*  (*__memcpy)(void*, const void*, size_t);
void*  (*__memmove)(void*, const void*, size_t);
void*  (*__memset)(void*, int, size_t);
char*  (*__strcat)(char*, const char*);
char*  (*__strncat)(char*, const char*, size_t);
char*  (*__strchr)(const char*, int);
int    (*__strcmp)(const char*, const char*);
int    (*__strncmp)(const char*, const char*, size_t);
char*  (*__strcpy)(char*, const char*);
char*  (*__strncpy)(char*, const char*, size_t);
size_t (*__strcspn)(const char*, const char*);
size_t (*__strlen)(const char*);
char*  (*__strpbrk)(const char*, const char*);
char*  (*__strrchr)(const char*, int c);
size_t (*__strspn)(const char*, const char*);
char*  (*__strstr)(const char*, const char*);
char*  (*__strtok)(char*, const char*);
size_t (*__strxfrm)(char*, const char*, size_t);
char*  (*__stradd)(char* str, char c);
char** (*__strsplit)(char*, char, int*);

// ctype
int (*__isalnum)(int);
int (*__isalpha)(int);
int (*__iscntrl)(int);
int (*__isdigit)(int);
int (*__isgraph)(int);
int (*__islower)(int);
int (*__isprint)(int);
int (*__ispunct)(int);
int (*__isspace)(int);
int (*__isupper)(int);
int (*__isxdigit)(int);
int (*__tolower)(int);
int (*__toupper)(int);

// time
char*  (*__timestr)(time_t, bool, bool);
time_t (*__timenow)(void);

// font
const uint8_t* __FONTDATA_DEFAULT = NULL;

// null method
const objdump_symbol_t NULL_METHOD = (objdump_symbol_t){ .address = 0, .name = "" };

// method table
uint32_t          _MTABLE_COUNT;
objdump_symbol_t* _MTABLE = NULL;

// section identifiers
#define SEC_TEXT   0
#define SEC_DATA   1
#define SEC_RODATA 2

// section names
const char SECSTR_TEXT[]   = ".text";
const char SECSTR_DATA[]   = ".data";
const char SECSTR_RODATA[] = ".rodata";

void lib_init()
{
    // fetch method table
    _printf("Initializing library methods...\n");
    asm volatile("int $0x80": :"a"(&_MTABLE_COUNT), "b"(0), "c"(SYSCALL_METHODS), "d"(&_MTABLE));

    // stdlib
    __malloc = lib_fetch("malloc", SEC_TEXT).address;
    __malloc_c = lib_fetch("malloc_c", SEC_TEXT).address;
    __malloc_a = lib_fetch("malloc_a", SEC_TEXT).address;
    __free = lib_fetch("free", SEC_TEXT).address;
    __freearray = lib_fetch("freearray", SEC_TEXT).address;
    __atoi = lib_fetch("atoi", SEC_TEXT).address;
    __atol = lib_fetch("atol", SEC_TEXT).address;
    __atox = lib_fetch("atox", SEC_TEXT).address;
    __atof = lib_fetch("atof", SEC_TEXT).address;
    __rand = lib_fetch("rand", SEC_TEXT).address;
    __yield = lib_fetch("yield", SEC_TEXT).address;
    __exit = lib_fetch("exit", SEC_TEXT).address;
    __itoa = lib_fetch("itoa", SEC_TEXT).address;
    __ltoa = lib_fetch("ltoa", SEC_TEXT).address;
    __strhex = lib_fetch("strhex", SEC_TEXT).address;
    __system = lib_fetch("system", SEC_TEXT).address;

    // stdio
    __vprintf = lib_fetch("vprintf", SEC_TEXT).address;

    // string
    __memchr = lib_fetch("memchr", SEC_TEXT).address;
    __memcmp = lib_fetch("memcmp", SEC_TEXT).address;
    __memcpy = lib_fetch("memcpy", SEC_TEXT).address;
    __memmove = lib_fetch("memmove", SEC_TEXT).address;
    __memset = lib_fetch("memset", SEC_TEXT).address;
    __strcat = lib_fetch("strcat", SEC_TEXT).address;
    __strncat = lib_fetch("strncat", SEC_TEXT).address;
    __strchr = lib_fetch("strchr", SEC_TEXT).address;
    __strcmp = lib_fetch("strcmp", SEC_TEXT).address;
    __strncmp = lib_fetch("strncmp", SEC_TEXT).address;
    __strcpy = lib_fetch("strcpy", SEC_TEXT).address;
    __strncpy = lib_fetch("strncpy", SEC_TEXT).address;
    __strcspn = lib_fetch("strcspn", SEC_TEXT).address;
    __strlen = lib_fetch("strlen", SEC_TEXT).address;
    __strpbrk = lib_fetch("strpbrk", SEC_TEXT).address;
    __strrchr = lib_fetch("strrchr", SEC_TEXT).address;
    __strspn = lib_fetch("strspn", SEC_TEXT).address;
    __strstr = lib_fetch("strstr", SEC_TEXT).address;
    __strtok = lib_fetch("strtok", SEC_TEXT).address;
    __strxfrm = lib_fetch("strxfrm", SEC_TEXT).address;
    __stradd = lib_fetch("stradd", SEC_TEXT).address;
    __strsplit = lib_fetch("strsplit", SEC_TEXT).address;

    // ctype
    __isalnum = lib_fetch("isalnum", SEC_TEXT).address;
    __isalpha = lib_fetch("isalpha", SEC_TEXT).address;
    __iscntrl = lib_fetch("iscntrl", SEC_TEXT).address;
    __isdigit = lib_fetch("isdigit", SEC_TEXT).address;
    __isgraph = lib_fetch("isgraph", SEC_TEXT).address;
    __islower = lib_fetch("islower", SEC_TEXT).address;
    __isprint = lib_fetch("isprint", SEC_TEXT).address;
    __ispunct = lib_fetch("ispunct", SEC_TEXT).address;
    __isspace = lib_fetch("isspace", SEC_TEXT).address;
    __isupper = lib_fetch("isupper", SEC_TEXT).address;
    __isxdigit = lib_fetch("isxdigit", SEC_TEXT).address;
    __tolower = lib_fetch("tolower", SEC_TEXT).address;
    __toupper = lib_fetch("toupper", SEC_TEXT).address;

    // time
    __timestr = lib_fetch("timestr", SEC_TEXT).address;
    __timenow = lib_fetch("timenow", SEC_TEXT).address;

    // font
    __FONTDATA_DEFAULT = lib_fetch("FONTDATA_DEFAULT", SEC_RODATA).address;
    FONT_DEFAULT = (font_t){ .width = 8, .height = 16, .spacing_x = -1, .spacing_y = 0, .data = __FONTDATA_DEFAULT };

    // finished
    _printf("Initialized elf libraries...\n");
}

objdump_symbol_t lib_fetch(char* name, uint8_t section)
{
    char* sec_name = SECSTR_TEXT;
    if (section == 1) { sec_name = SECSTR_DATA; }
    else if (section == 2) { sec_name = SECSTR_RODATA; }

    for (uint32_t i = 0; i < _MTABLE_COUNT; i++)
    {
        if (!_stringcomp(_MTABLE[i].name, name) && !_stringcomp(_MTABLE[i].section, sec_name)) 
        {
            _printf("Located symbol - ADDR: 0x%8x, SECTION: %s, NAME: '%s'\n", _MTABLE[i].address, _MTABLE[i].section, _MTABLE[i].name);
            return _MTABLE[i]; 
        }
    }

    _printf("Unable to fetch library symbol '%s'\n", name);
    return NULL_METHOD;
}

int _vprintf(const char* str, va_list args)
{
    asm volatile("int $0x80": :"a"(&args), "b"(str), "c"(SYSCALL_VPRINTF), "d"(0));
    return true;
}

int _printf(const char* str, ...)
{   
    va_list args;
    va_start(args, str);
    _vprintf(str, args);
    va_end(args);
    return true;
}

bool _stringcomp(char* str1, char* str2)
{
    while (*str1)
    {
        if (*str1 != *str2) { break; }
        str1++; str2++;
    }
    return *(char*)str1 - *(char*)str2;
}