#pragma once
#include <lib/stdint.h>

#define NULL 0
typedef long unsigned int size_t;
typedef uint32_t ptrdiff_t;    

typedef struct
{
    void*    items;
    uint32_t item_size;
    uint32_t count;
} array_t;

typedef struct
{
    void**   items;
    uint32_t count;
} ptrarray_t;

array_t  array_create(uint32_t count, uint32_t item_size);
array_t  array_create_ext(uint32_t count, uint32_t item_size, void* items);
bool     array_dispose(array_t* arr);
void     array_clear(array_t* arr);
void*    array_get(array_t* arr, int index);

ptrarray_t ptrarray_create(uint32_t count);
ptrarray_t ptrarray_create_ext(uint32_t count, void** items);
bool       ptrarray_dispose(ptrarray_t* arr);
void       ptrarray_clear(ptrarray_t* arr, bool dispose);
void*      ptrarray_get(ptrarray_t* arr, int index);