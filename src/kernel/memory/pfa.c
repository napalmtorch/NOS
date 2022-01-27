#include <kernel/memory/pfa.h>
#include <kernel/core/kernel.h>

extern uint32_t _boot_page_dir0;
extern uint32_t _boot_page_dir0_end;

pfa_entry_t* pfa_allocated_entry();
int pfa_free_index();

pfa_entry_t  pfa_entries[PFA_COUNT];
uint32_t     pfa_count;
uint32_t     pfa_size;
uint32_t     pfa_last;
uint32_t     pfa_kstart;
uint32_t     pfa_kaddr;
uint32_t     pfa_ksize;
bool         pfa_ready = false;

void pfa_init_kernel()
{
    pfa_kstart   = mem_align((uint32_t)&_boot_page_dir0 + 0x2000, 0x1000) - VIRT_KERNEL;
    pfa_kaddr    = pfa_kstart;
    pfa_ksize    = ((uint32_t)&_boot_page_dir0_end - VIRT_KERNEL) - pfa_kstart;
    pfa_ready    = false;
    (void)pmm_create_ext(pfa_kaddr, pfa_ksize, PMMTYPE_PFA);
}

void pfa_init(uint32_t size)
{
    if (size < 4 * MB) { size = 4 * MB; }
    pfa_count    = 0;
    pfa_size     = size;
    pfa_last     = PHYS_PFA;
    memset(&pfa_entries, 0, sizeof(pfa_entry_t*) * PFA_COUNT);

    // map pages
    pfa_map(vmm_get_kdir());

    // clear memory region
    memset(PHYS_PFA, 0, pfa_size);

    // finished
    (void)pmm_create_ext(PHYS_PFA, pfa_size, PMMTYPE_PFA);
    pfa_ready = true;
    debug_ok("Initialized page-frame allocator - TABLE: 0x%8x, START: 0x%8x", ((uint32_t)&pfa_entries) - VIRT_KERNEL, PHYS_PFA);
}

void pfa_map(void* pagedir)
{
    debug_info("Mapping page-frame allocator onto directory at 0x%8x...", ((pagedir_t*)pagedir)->address);
    for (uint32_t i = 0; i <= pfa_size; i += 4 * MB) 
    { 
        if (pfa_ready) { pagedir_map(pagedir, PHYS_PFA + i, PHYS_PFA + i, false); }
        else { vmm_kmap(PHYS_PFA + i, PHYS_PFA + i, false); }
    }
    debug_info("Finished identity mapping page-frame allocator");
}

uint32_t pfa_allocate()
{
    pfa_entry_t* entry = pfa_allocated_entry();
    if (entry == NULL) { return 0; }
    return entry->address;
}

uint32_t pfa_kallocate()
{
    if (pfa_kaddr >= pfa_kstart + pfa_ksize) { debug_error("Kernel PFA out of frames"); return 0; }
    uint32_t addr = pfa_kaddr;
    pfa_kaddr += 0x1000;
    return addr;
}

pfa_entry_t* pfa_allocated_entry()
{
    int i = 0;
    for (i = 0; i < PFA_COUNT; i++)
    {
        if (!pfa_entries[i].address > 0 && pfa_entries[i].used) { return &pfa_entries[i]; }
    }

    i = pfa_free_index();
    if (i < 0 || i >= PFA_COUNT) { debug_error("PFA out of frames"); return NULL; }
    uint32_t addr = pfa_last;
    pfa_last += 0x1000;
    pfa_entries[i].address = addr;
    pfa_entries[i].used    = true;
    return &pfa_entries[i];
}

void pfa_free(uint32_t phys)
{

}

int pfa_free_index()
{
    for (uint32_t i = 0; i < PFA_COUNT; i++)
    {
        if (pfa_entries[i].address == 0 && !pfa_entries[i].used) { return false; }
    }
}

bool pfa_is_ready() { return pfa_ready; }

uint32_t pfa_get_size() { return pfa_size; }