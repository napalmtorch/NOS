#include <lib/string.h>
#include <kernel/core/kernel.h>

void* memchr(const void* str, int c, size_t n)
{
    unsigned char *p = (unsigned char*)str;
    unsigned char *find = NULL;
    while((str != NULL) && (n--))
    {
        if( *p != (unsigned char)c) { p++; }
        else { find = p; break; }
    }
    return find;
}

int memcmpl(const void* str1, const void* str2, size_t size)
{
	size_t l = size/4;
	asm volatile("cld; rep cmpsl" : "+S"(str2), "+D"(str1), "+c"(l) : : "memory");
	uint8_t result;
	asm volatile("setz %0" : "=r"(result));
	return result;
}
int memcmpw(const void* str1, const void* str2, size_t size)
{
	size_t l = size/2;
	asm volatile("cld; rep cmpsw" : "+S"(str2), "+D"(str1), "+c"(l) : : "memory");
	uint8_t result;
	asm volatile("setz %0" : "=r"(result));
	return result;
}
int memcmpb(const void* str1, const void* str2, size_t size)
{
	asm volatile("cld; rep cmpsb" : "+S"(str2), "+D"(str1), "+c"(size) : : "memory");
	uint8_t result;
	asm volatile("setz %0" : "=r"(result));
	return result;
}

int memcmp(const void* str1, const char* str2, size_t size)
{
	if (size % 4 == 0) { return memcmpl(str1, str2, size); }
	else if (size % 2 == 0) { return memcmpw(str1, str2, size); }
	else { return memcmpb(str1, str2, size); }
}

void* memcpy(void* dest, const void* src, size_t size)
{
    if (size % 4 == 0)
    {
        size_t l = size/4;
        asm volatile("cld;rep movsl" : "+D"(dest), "+S"(src), "+c"(l) : : "memory");
        return dest;
    }
	else if (size % 2 == 0)
    {
        size_t l = size/2;
        asm volatile("cld;rep movsw" : "+D"(dest), "+S"(src), "+c"(l) : : "memory");
        return dest;
    }
	else
    {
        asm volatile("cld;rep movsb" : "+D"(dest), "+S"(src), "+c"(size) : : "memory");
	    return dest;
    }
}

void* memset(void* dest, int data, size_t size)
{
    if (size % 4 == 0)
    {     
        size_t l = size / 4;
        asm volatile("cld;rep stosl" : "+D"(dest), "+c"(l) : "a"(data) : "memory");
        return dest;
    }
	else if (size % 2 == 0)
    {
        size_t l = size/2;
        asm volatile("cld;rep stosw" : "+D"(dest), "+c"(l) : "a"(data) : "memory");
        return dest;
    }
	else
    {
        asm volatile("cld;rep stosb" : "+D"(dest), "+c"(size) : "a"(data) : "memory");
	    return dest;
    }
}

void* memmove(void* dest, const void* src, size_t n)
{
	return NULL;
}

char* strcat(char* dest, const char* src)
{
    if (dest == NULL) { return NULL; }
    if (src == NULL) { return dest; }
    uint64_t src_len = strlen(src);
    uint64_t dest_len = strlen(dest);
    for (uint64_t i = 0; i < src_len; i++) { dest[dest_len + i] = src[i]; }
    dest[strlen(dest)] = 0;
    return dest;
}

char* strncat(char* dest, const char* src, size_t n)
{
    if (dest == NULL) { return NULL; }
    if (src == NULL) { return dest; }
    uint64_t dest_len = strlen(dest);
    for (uint64_t i = 0; i < n; i++) { dest[dest_len + i] = src[i]; }
    dest[strlen(dest)] = 0;
    return dest;
}

char* strchr(const char* str, int c)
{
    do 
    {
        if (*str == c) { return (char*)str; }
    } while (*str++);
    return NULL;
}

int strcmp(const char* str1, const char* str2)
{
    while (*str1)
    {
        if (*str1 != *str2) { break; }
        str1++; str2++;
    }
    return *(char*)str1 - *(char*)str2;
}

int strncmp(const char* str1, const char* str2, size_t n)
{
    while (n && *str1 && (*str1 == *str2))
    {
        ++str1;
        ++str2;
        --n;
    }
    if (n == 0) { return 0; }
    else { return (*(char*)str1 = *(char*)str2); }
    return 0;
}

char* strcpy(char* dest, const char* src)
{
    if (dest == NULL) { return NULL; }
    if (src == NULL) { return NULL; }

    int i = 0;
    while (src[i] != 0) { dest[i] = src[i]; i++; }
}

char* strncpy(char* dest, const char* src, size_t n)
{
    for (uint32_t i = 0; i < n; i++) { dest[i] = src[i]; }
    return dest;
}

size_t strcspn(const char* str1, const char* str2)
{
    return 0;
}

size_t strlen(const char* str)
{
    uint32_t len = 0;
    while (str[len] != 0) { len++; }
    return len;
}

char* strpbrk(const char* str1, const char* str2)
{
    return NULL;
}

char* strrchr(const char* str, int c)
{
    return NULL;
}

size_t strspn(const char* str1, const char* str2)
{
    return 0;
}

char* strstr(const char* haystack, const char* needle)
{
    return NULL;
}

char* strtok(char* str, const char* delim)
{
    return NULL;
}

size_t strxfrm(char* dest, const char* str, size_t n)
{
    return 0;
}

char* stradd(char* str, char c)
{
    if (str == NULL) { return NULL; }
    uint64_t len = strlen(str);
    str[len] = (char)c;
    str[len + 1] = 0;
    return str;
}

char** strsplit(char* str, char delim, int* count)
{
    if (str == NULL) { return NULL; }
    if (strlen(str) == 0) { return NULL; }

    int len = strlen(str);
    uint32_t num_delimeters = 0;

    for(int i = 0; i < len - 1; i++)
    {
        if(str[i] == delim) { num_delimeters++; }
    }

    uint32_t arr_size = sizeof(char*) * (num_delimeters + 1);
    char** str_array = kmalloc(arr_size, HEAPTYPE_STRING);
    int str_offset = 0;

    int start = 0;
    int end = 0;
    while(end < len)
    {
        while(str[end] != delim && end < len) { end++; }

        char* substr = kmalloc(end - start + 1, HEAPTYPE_STRING);
        memcpy(substr, str + start, end - start);
        start = end + 1;
        end++;
        str_array[str_offset] = substr;
        str_offset++;
    }

    //return necessary data now
    *count = str_offset;
    return str_array;
}