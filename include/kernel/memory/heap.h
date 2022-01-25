#pragma once
#include <lib/stdint.h>
#include <kernel/exec/thread.h>
#include <kernel/memory/vmm.h>

typedef enum
{
    HEAPTYPE_FREE,
    HEAPTYPE_USED,
    HEAPTYPE_ARRAY,
    HEAPTYPE_PTRARRAY,
    HEAPTYPE_STRING,
    HEAPTYPE_OBJECT,
    HEAPTYPE_PROC,
    HEAPTYPE_THREAD,
    HEAPTYPE_THREADSTACK,
    HEAPTYPE_UNKNOWN,
} HEAPTYPE;

typedef struct
{
    uint32_t  address;
    thread_t* thread;
    uint32_t  size;
    HEAPTYPE  type;
} PACKED heap_entry_t;

typedef struct
{
    heap_entry_t* entries;
    uint32_t      count;
    uint32_t      count_max;
    uint32_t      size;
    uint32_t      p_address;
    uint32_t      v_address;
    bool          messages;
} PACKED heap_t;

#define HEAP_COUNT_MAX  322000
#define HEAP_MASS       0
#define HEAP_TABLE_SIZE 4 * MB

extern heap_t HEAP_KERNEL;
extern heap_t HEAP_USER;

void heap_init(heap_t* heap, uint32_t virt, uint32_t size, uint32_t max_entries);

void heap_map(heap_t* heap, pagedir_t* dir);

void heap_print(heap_t* heap);

void* heap_alloc(heap_t* heap, uint32_t size, bool clear, HEAPTYPE type);

void heap_free(heap_t* heap, void* ptr);

heap_entry_t* heap_locate(heap_t* heap, void* ptr);