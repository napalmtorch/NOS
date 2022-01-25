#pragma once
#include <lib/stdint.h>

/// @brief Page frame allocation entry structure
typedef struct
{
    uint32_t address;
    bool     used;
} PACKED pfa_entry_t;

/// @internal Amount of page frame allocation entries
#define PFA_COUNT 4096

void pfa_init_kernel();

/// @brief Initialize page frame allocator @param size Size of page frame allocator in bytes
void pfa_init(uint32_t size);

void pfa_map(void* pagedir);;

/// @brief Allocate page frame @return Address of page frame
uint32_t pfa_allocate();

/// @brief Allocate kernel page frame @return Address of page frame
uint32_t pfa_kallocate();

/// @brief Free existing page frame @param phys Address of page frame
void pfa_free(uint32_t phys);

bool pfa_is_ready();

uint32_t pfa_get_size();