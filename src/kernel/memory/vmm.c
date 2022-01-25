#include <kernel/memory/vmm.h>
#include <kernel/core/kernel.h>

pagetable_t* pagedir_get_free(pagedir_t* dir);

#define PAGE_COUNT 1024

uint32_t  pagedir_id;
pagedir_t pagedir_kernel;

void vmm_init()
{
    uint32_t phys = pfa_kallocate();
    pagedir_init(&pagedir_kernel, phys);

    // map kernel
    for (uint32_t i = PHYS_KERNEL; i <= PHYS_KERNEL + SIZE_KERNEL; i += 4 * MB)
    {
        vmm_kmap(i, i, false);
        vmm_kmap(VIRT_KERNEL + i, i, true);
    }

    regwrite_cr3(phys);

    debug_ok("Initialized VMM");
}

void vmm_kmap(uint32_t v_addr, uint32_t p_addr, bool msg)
{
    uint32_t     p     = p_addr;
    uint32_t     id    = v_addr >> 22;
    pagetable_t* table = pagedir_get_free(&pagedir_kernel);
    uint32_t     index = pagedir_get_index(&pagedir_kernel, table);
    if (table == NULL) { debug_error("Unable to map page at virtual address 0x%8x", v_addr); return; }
    
    if (pfa_is_ready()) { table->entries = pfa_allocate(); } else { table->entries = pfa_kallocate(); }
    table->p_address = p_addr;
    table->v_address = v_addr;
    table->used      = true;
    
    for (int i = 0; i < PAGE_COUNT; i++) 
    { 
        table->entries[i] = (pagetable_entry_t){ .block_addr = p >> 22, .present = true, .rw = true, .privilege = PAGEPRIV_SUPERVISOR };
        table->entries[i].block_addr = p >> 12;
        table->entries[i].present = true;
        table->entries[i].rw = true;
        table->entries[i].privilege = PAGEPRIV_SUPERVISOR;
        p += 0x1000;
    }

    pagedir_entry_t entry = (pagedir_entry_t){ .table_addr = (uint32_t)table->entries >> 12, .present = 1, .rw = true, .privilege = PAGEPRIV_SUPERVISOR, .page_size = PAGESIZE_4KB };
    pagedir_write(&pagedir_kernel, id, entry);
    if (msg) 
    { 
        debug_header(" PMAP ", COL4_GREEN);
        debug_printf("Mapped physical 0x%8x to virtual 0x%8x(%d)\n", p_addr, v_addr, id); 
    }
    return true;
}

void vmm_kunmap(uint32_t v_addr, bool msg)
{
    uint32_t id = v_addr >> 22;

    for (int i = 0; i < PAGE_COUNT; i++)
    {
        if (pagedir_kernel.tables[i].used && pagedir_kernel.tables[i].v_address == v_addr)
        {
            pagetable_entry_t tentry = (pagetable_entry_t){ .block_addr = 0, .present = 0, .rw = false, .privilege = PAGEPRIV_SUPERVISOR };
            for (int j = 0; j < PAGE_COUNT; j++) { pagedir_kernel.tables[i].entries[j] = tentry; }
            pagedir_kernel.tables[i].v_address = 0;
            pagedir_kernel.tables[i].p_address = 0;
            pagedir_kernel.tables[i].entries   = NULL;
            pagedir_kernel.tables[i].used      = false;
            pagedir_entry_t entry = (pagedir_entry_t){ .table_addr = 0, .present = 0, .rw = false, .privilege = PAGEPRIV_SUPERVISOR, .page_size = PAGESIZE_4KB };
            pagedir_write(&pagedir_kernel, id, entry);
            if (msg) 
            { 
                debug_header("PUNMAP", COL4_MAGENTA);
                debug_printf("Un-mapped virtual address(%d) 0x%8x\n", i, v_addr); 
            }
            return true;
        }
    }

    debug_error("Unable to un-map virtual address 0x%8x", v_addr);
    return false;
}

pagedir_t* vmm_get_kdir() { return &pagedir_kernel; }

void pagedir_init(pagedir_t* dir, uint32_t p_addr)
{
    dir->mapped_count = 0;
    dir->address = p_addr;
    dir->entries = (uint32_t*)p_addr;
    pagedir_entry_t entry = (pagedir_entry_t){ .table_addr = 0, .present = 0, .rw = false, .privilege = PAGEPRIV_SUPERVISOR, .page_size = PAGESIZE_4KB };
    for (int i = 0; i < PAGE_COUNT; i++) { pagedir_write(dir, i, entry); }
    dir->id = pagedir_id++;
    debug_ok("Initialized page directory - ADDR: 0x%8x, ID: %d", p_addr, dir->id);
    return true;
}

bool pagedir_map(pagedir_t* dir, uint32_t v_addr, uint32_t p_addr, bool msg)
{
    uint32_t     p     = p_addr;
    uint32_t     id    = v_addr >> 22;
    pagetable_t* table = pagedir_get_free(dir);
    uint32_t     index = pagedir_get_index(dir, table);
    if (table == NULL) { debug_error("Unable to map page at virtual address 0x%8x", v_addr); return false; }
    
    table->entries   = pfa_allocate();
    table->p_address = p_addr;
    table->v_address = v_addr;
    table->used      = true;
    
    for (int i = 0; i < PAGE_COUNT; i++) 
    { 
        table->entries[i] = (pagetable_entry_t){ .block_addr = p >> 22, .present = true, .rw = true, .privilege = PAGEPRIV_SUPERVISOR };
        table->entries[i].block_addr = p >> 12;
        table->entries[i].present = true;
        table->entries[i].rw = true;
        table->entries[i].privilege = PAGEPRIV_SUPERVISOR;
        p += 0x1000;
    }

    pagedir_entry_t entry = (pagedir_entry_t){ .table_addr = (uint32_t)table->entries >> 12, .present = 1, .rw = true, .privilege = PAGEPRIV_SUPERVISOR, .page_size = PAGESIZE_4KB };
    pagedir_write(dir, id, entry);
    if (msg) 
    { 
        debug_header(" PMAP ", COL4_GREEN);
        debug_printf("Mapped physical 0x%8x to virtual 0x%8x(%d)\n", p_addr, v_addr, id); 
    }
    return true;
}

bool pagedir_unmap(pagedir_t* dir, uint32_t v_addr, bool msg)
{
    return false;
}

void pagedir_write(pagedir_t* dir, int index, pagedir_entry_t table)
{
    dir->entries[index] = table;
}

pagedir_entry_t pagedir_read(pagedir_t* dir, int index)
{
    if (index < 0 || index >= 1024) { return (pagedir_entry_t){}; }   
    return dir->entries[index];
}

int pagedir_get_index(pagedir_t* dir, pagetable_t* table)
{
    for (int i = 0; i < PAGE_COUNT; i++)
    {
        if (&dir->tables[i] == table) { return i; }
    }
    return -1;
}

pagetable_t* pagedir_get_free(pagedir_t* dir)
{
    for (int i = 0; i < PAGE_COUNT; i++)
    {
        if (!dir->tables[i].used) { return &dir->tables[i]; }
    }
    return NULL;
}