#include <kernel/memory/heap.h>
#include <kernel/core/kernel.h>

heap_entry_t* heap_alloc_entry(heap_t* heap, uint32_t size);
heap_entry_t* heap_create_entry(heap_t* heap, thread_t* thread, uint32_t addr, uint32_t size, HEAPTYPE type);
bool          heap_delete_entry(heap_t* heap, heap_entry_t* entry);
uint32_t      heap_free_index(heap_t* heap);

heap_t HEAP_KERNEL;
heap_t HEAP_USER;

bool     heap_initialized = false;
uint32_t heap_last;

void heap_init(heap_t* heap, uint32_t virt, uint32_t size, uint32_t max_entries)
{
    if (!heap_initialized) 
    { 
        heap_last        = PHYS_PFA + mem_align(pfa_get_size() + 0x1000, 0x1000); 
        heap_initialized = true;
    }

    // clamp values to within valid regions
    size = clamp(size, 4 * MB, 1024 * MB);
    max_entries = clamp(max_entries, 1024, HEAP_COUNT_MAX);

    heap->p_address = heap_last;
    heap->v_address = virt;
    heap->count     = 0;
    heap->count_max = clamp(max_entries, 1024, 8192);
    heap->size      = size;
    heap->entries   = (heap_entry_t*)heap->v_address;
    heap->messages  = true;
    heap_last       += mem_align(heap->size + (4 * MB), MB);

    // map physical
    (void)pmm_create_ext(heap->p_address, heap->size + HEAP_TABLE_SIZE, PMMTYPE_HEAP);

    // map virtual
    heap_map(heap, vmm_get_kdir());

    // clear memory region
    memset(heap->v_address, 0, heap->size + HEAP_TABLE_SIZE);

    // create mass entry
    uint32_t mass = heap->v_address + (heap->count_max * sizeof(heap_entry_t));
    heap->entries[HEAP_MASS] = (heap_entry_t){ .thread = NULL, .address = mass, .size = heap->size, .type = HEAPTYPE_FREE };

    // finished
    char temp[32];
    debug_ok("Initialized heap - PHYS: 0x%8x, VIRT: 0x%8x, SIZE: %s", heap->p_address, heap->v_address, mem_convert_size_str(temp, heap->size));
}

void heap_map(heap_t* heap, pagedir_t* dir)
{
    for (uint32_t i = 0; i <= heap->size + HEAP_TABLE_SIZE; i += 4 * MB)
    {
        pagedir_map(dir, heap->v_address + i, heap->p_address + i, false);
    }
    debug_info("Finished mapping %d pages for heap 0x%8x", (heap->size / (4 * MB)) + 1, heap->v_address);
}

void heap_print(heap_t* heap)
{
    if (heap == NULL) { return; }
    char temp[32];
    memset(temp, 0, 32);

    printf("------ HEAP ENTRIES -------------------------------------\n");
    for (uint32_t i = 0; i < heap->count_max; i++)
    {
        if (heap->entries[i].address > 0 && heap->entries[i].size > 0)
        {
            printf("HEAP ENTRY 0x%8x - THREAD: 0x%8x, ADDR: 0x%8x, TYPE: 0x%2x, SIZE: %s", 
                    heap->v_address, heap->entries[i].thread, heap->entries[i].address, heap->entries[i].type, mem_convert_size_str(temp, heap->entries[i].size));
        }
    }
    printf("\n");
}

void* heap_alloc(heap_t* heap, uint32_t size, bool clear, HEAPTYPE type)
{
    if (size == 0) { return NULL; }
    uint32_t aligned = mem_align(size, 0x1000);
    heap_entry_t* entry = heap_alloc_entry(heap, aligned);
    if (clear) { memset(entry->address, 0, entry->size); }
    if (type != HEAPTYPE_FREE) { entry->type = type; }

    char temp[32];
    if (heap->messages) 
    {
        debug_header("MALLOC", COL4_GREEN);
        debug_printf("THREAD: 0x%8x, ADDR: 0x%8x, TYPE: 0x%2x, SIZE: %s\n", entry->thread, entry->address, entry->type, mem_convert_size_str(temp, entry->size)); 
    }
    return entry->address;
}

void heap_free(heap_t* heap, void* ptr)
{
    if (heap == NULL || ptr == NULL) { return; }

    for (uint32_t i = 0; i < heap->count_max; i++)
    {
        if (heap->entries[i].type == HEAPTYPE_FREE) { continue; }
        if (heap->entries[i].address == 0 || heap->entries[i].size == 0) { continue; }
        if (heap->entries[i].address == (uint32_t)ptr)
        {
            heap->entries[i].type = HEAPTYPE_FREE;
            char temp[32];
            if (heap->messages) 
            { 
                debug_header(" FREE ", COL4_MAGENTA);
                debug_printf("THREAD: 0x%8x, ADDR: 0x%8x, SIZE: %s\n", heap->entries[i].thread, heap->entries[i].address, mem_convert_size_str(temp, heap->entries[i].size)); 
            }
            return;
        }
    }

    debug_error("Unable to free pointer 0x%8x", ptr);
}

heap_entry_t* heap_alloc_entry(heap_t* heap, uint32_t size)
{
    for (uint32_t i = 0; i < heap->count_max; i++)
    {
        if (heap->entries[i].type != HEAPTYPE_FREE) { continue; }
        if (heap->entries[i].address > 0 && heap->entries[i].size == size)
        {
            heap->entries[i].type = HEAPTYPE_USED;
            return &heap->entries[i];
        }
    }

    heap_entry_t* new_entry = heap_create_entry(heap, NULL, heap->entries[HEAP_MASS].address, size, HEAPTYPE_USED);
    heap->entries[HEAP_MASS].address += size;
    heap->entries[HEAP_MASS].size    -= size;
    return new_entry;
}

heap_entry_t* heap_create_entry(heap_t* heap, thread_t* thread, uint32_t addr, uint32_t size, HEAPTYPE type)
{
    if (heap == NULL) { return NULL; }

    int i = heap_free_index(heap);
    if (i < 0 || i >= heap->count_max) { debug_error("Maximum amount of entries reached in heap 0x%8x", heap->v_address); return NULL; }
    heap->entries[i] = (heap_entry_t){ .thread = thread, .address = addr, .size = size, .type = type };
    heap->count++;
    return &heap->entries[i];
}

bool heap_delete_entry(heap_t* heap, heap_entry_t* entry)
{
    if (heap == NULL) { return false; }

    for (uint32_t i = 0; i < heap->count_max; i++)
    {
        if (&heap->entries[i] == entry)
        {
            memset(&heap->entries[i], 0, sizeof(heap_entry_t));
            heap->count--;
            return true;
        }
    }
}

heap_entry_t* heap_locate(heap_t* heap, void* ptr)
{
    for (int i = 0; i < heap->count_max; i++)
    {
        if (heap->entries[i].address == 0 || heap->entries[i].size == 0) { continue; }
        if (heap->entries[i].type == HEAPTYPE_FREE) { continue; }
        if (heap->entries[i].address == (uint32_t)ptr) { return &heap->entries[i]; }
    }
    return NULL;
}

uint32_t heap_free_index(heap_t* heap)
{
    for (uint32_t i = 0; i < heap->count_max; i++)
    {
        if (heap->entries[i].address == 0 && heap->entries[i].size == 0 && heap->entries[i].thread == NULL && heap->entries[i].type == HEAPTYPE_FREE) { return (int)i; }
    }
    return -1;
}