#include <lib/stddef.h>
#include <kernel/core/kernel.h>

array_t array_create(uint32_t count, uint32_t item_size)
{
    array_t arr = (array_t){ .items = kmalloc(item_size * count, HEAPTYPE_ARRAY), .item_size = item_size, .count = count };
    return arr;
}

array_t array_create_ext(uint32_t count, uint32_t item_size, void* items)
{
    array_t arr = (array_t){ .items = items, .item_size = item_size, .count = count };
    return arr;
}

bool array_dispose(array_t* arr)
{
    if (arr == NULL) { return false; }
    free(arr->items);
    arr->items = NULL;
    arr->count = 0;
    return true;
}

void array_clear(array_t* arr)
{
    if (arr == NULL) { return; }
    if (arr->items == NULL) { return; }
    memset(arr->items, 0, arr->count * arr->item_size);
}

void* array_get(array_t* arr, int index)
{
    if (arr == NULL) { return NULL; }
    if (arr->items == NULL) { return NULL; }
    if (index < 0 || index >= arr->count) { return NULL; }
    return &arr->items[index];
}

ptrarray_t ptrarray_create(uint32_t count)
{
    ptrarray_t arr = (ptrarray_t){ .items = kmalloc(count * sizeof(void*), HEAPTYPE_PTRARRAY), .count = count };
    return arr;
}

ptrarray_t ptrarray_create_ext(uint32_t count, void** items)
{
    ptrarray_t arr = (ptrarray_t){ .items = items, .count = count };
    return arr;
}

bool ptrarray_dispose(ptrarray_t* arr)
{
    if (arr == NULL) { return false; }
    freearray(arr->items, arr->count);
    arr->items = NULL;
    arr->count = 0;
    return true;
}

void ptrarray_clear(ptrarray_t* arr, bool dispose)
{
    if (arr == NULL) { return; }
    if (arr->items == NULL) { return; }
    if (dispose) { for (uint32_t i = 0; i < arr->count; i++) { free(arr->items[i]); arr->items[i] = NULL; } }
    memset(arr->items, 0, arr->count * sizeof(void*));
}

void* ptrarray_get(ptrarray_t* arr, int index)
{
    if (arr == NULL) { return NULL; }
    if (arr->items == NULL) { return NULL; }
    if (index < 0 || index >= arr->count) { return NULL; }
    return arr->items[index];
}