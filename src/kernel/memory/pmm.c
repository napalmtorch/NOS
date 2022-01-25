#include <kernel/memory/pmm.h>
#include <kernel/core/kernel.h>

int pmm_free_index();

pmm_entry_t pmm_entries[PMM_COUNT];
uint32_t    pmm_count;

pmm_entry_t* pmme_kernel;

void pmm_init()
{
    // clear list of entries
    memset(&pmm_entries, 0, sizeof(pmm_entry_t) * PMM_COUNT);

    // add kernel entry
    pmme_kernel = pmm_create_ext(PHYS_KERNEL, SIZE_KERNEL, PMMTYPE_KERNEL);

    // finished
    debug_ok("Initialized PMM");
}

pmm_entry_t* pmm_create(pmm_entry_t entry) { return pmm_create_ext(entry.address, entry.size, entry.type); }

pmm_entry_t* pmm_create_ext(uint32_t addr, uint32_t size, PMMTYPE type)
{
    int i = pmm_free_index();
    if (i < 0 || i >= PMM_COUNT) { debug_error("Maximum amount of PMM entries reached"); return false; }
    char size_str[32];
    pmm_entries[i].address = addr;
    pmm_entries[i].size    = size;
    pmm_entries[i].type    = type;
    pmm_count++;
    debug_header(" PMMC ", COL4_GREEN);
    debug_printf("ADDR: 0x%8x, TYPE: 0x%2x, SIZE: %s\n", addr, type, mem_convert_size_str(size_str, size));
    return &pmm_entries[i];
}

bool pmm_delete(pmm_entry_t entry)
{
    char size_str[32];
    for (uint32_t i = 0; i < PMM_COUNT; i++)
    {
        if (!memcmp(&entry, &pmm_entries[i], sizeof(pmm_entry_t)))
        {
            memset(&pmm_entries[i], 0, sizeof(pmm_entry_t));
            pmm_count--;   
            debug_header(" PMMD ", COL4_MAGENTA);        
            debug_printf("ADDR: 0x%8x, TYPE: 0x%2x, SIZE: %s\n", entry.address, entry.type, mem_convert_size_str(size_str, entry.size));
            return true;
        }
    }

    debug_error("Unable to delete PMM entry - ADDR: 0x%8x, TYPE: 0x%2x, SIZE: %s", entry.address, entry.type, mem_convert_size_str(size_str, entry.size));
    return false;
}

int pmm_free_index()
{
    for (uint32_t i = 0; i < PMM_COUNT; i++)
    {
        if (pmm_entries[i].address == 0 && pmm_entries[i].size == 0 && pmm_entries[i].type == 0) { return (int)i; }
    }
    return -1;
}