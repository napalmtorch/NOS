#pragma once
#include <lib/stdint.h>

typedef enum { PAGEACCESS_R, PAGEACCESS_RW } PAGEACCESS;
typedef enum { PAGEPRIV_SUPERVISOR, PAGEPRIV_USER } PAGEPRIV;
typedef enum { PAGESIZE_4KB, PAGESIZE_4MB } PAGESIZE;

typedef struct
{
    bool         present    : 1;    // Indicates whether the page is present in memory
    PAGEACCESS   rw         : 1;    // Indicates whether the page is read-only or read-write
    PAGEPRIV     privilege  : 1;    // Indicates whether the page is accessible to user or supervisor
    uint8_t      reserved_1 : 2;    // Reserved bits
    bool         accessed   : 1;    // Indicates whether the page has been accessed
    bool         dirty      : 1;    // Indicates whether the page has been written to
    uint32_t     reserved_2 : 2;    // Reserved for internal use, cannot be modified
    uint32_t     unused     : 3;    // Unused bits, available for kernel use
    uintptr_t    block_addr : 20;   // The address of the page
} PACKED pagetable_entry_t;

typedef struct
{
    bool        present    : 1;     // Indicates whether the table is present in memory
    PAGEACCESS  rw         : 1;     // Indicates whether the table is read-only or read-write
    PAGEPRIV    privilege  : 1;     // Indicates whether the table is accessible to user or supervisor
    uint8_t     reserved_1 : 2;     // Reserved bits
    bool        accessed   : 1;     // Indicates whether the table has been accessed
    bool        dirty      : 1;     // Indicates whether the table has been written to
    PAGESIZE    page_size  : 1;     // Indicates whether the table is 4KB or 4MB
    uint8_t     unused     : 4;     // Unused bits, available for kernel use
    uintptr_t   table_addr: 20;     // The address of the table

} PACKED pagedir_entry_t;

typedef struct
{
    uint32_t           p_address;
    uint32_t           v_address;
    pagetable_entry_t* entries;
    bool               used;
} PACKED pagetable_t;

typedef struct
{
    uint32_t         id;
    uint32_t         address;
    uint32_t         mapped_count;
    pagedir_entry_t* entries;
    pagetable_t      tables[1024];
} PACKED pagedir_t;

void vmm_init();
void vmm_kmap(uint32_t v_addr, uint32_t p_addr, bool msg);
void vmm_kunmap(uint32_t v_addr, bool msg);
pagedir_t* vmm_get_kdir();

void     pagedir_init(pagedir_t* dir, uint32_t p_addr);
bool     pagedir_map(pagedir_t* dir, uint32_t v_addr, uint32_t p_addr, bool msg);
bool     pagedir_unmap(pagedir_t* dir, uint32_t v_addr, bool msg);
void     pagedir_write(pagedir_t* dir, int index, pagedir_entry_t table);
pagedir_entry_t pagedir_read(pagedir_t* dir, int index);
int      pagedir_get_index(pagedir_t* dir, pagetable_t* table);