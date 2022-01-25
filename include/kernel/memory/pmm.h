#pragma once
#include <lib/stdint.h>

/// @brief Physical memory entry types
typedef enum
{
    PMMTYPE_FREE,
    PMMTYPE_RESERVED,
    PMMTYPE_KERNEL,
    PMMTYPE_PCI,
    PMMTYPE_ACPI,
    PMMTYPE_PFA,
    PMMTYPE_HEAP,
    PMMTYPE_UNKNOWN,
} PMMTYPE;

/// @brief Physical memory entry structure
typedef struct
{
    uint32_t address;
    uint32_t size;
    PMMTYPE  type;
} PACKED pmm_entry_t;

/// @internal Data sizes
#define KB 1024
#define MB 1048576
#define GB 1073741824

/// @internal Region sizes
#define SIZE_KERNEL (16   * MB)
#define SIZE_HEAP   (1024 * MB)

/// @internal Reserved physical addresses
#define PHYS_KERNEL 0x00000000
#define PHYS_PFA    PHYS_KERNEL + SIZE_KERNEL

/// @internal Reserved virtual addresses
#define VIRT_KERNEL 0xC0000000
#define VIRT_PFA    VIRT_KERNEL + SIZE_KERNEL
#define VIRT_KHEAP  0xD0000000
#define VIRT_UHEAP  0x20000000

/// @internal Maximum amount of physical memory entries
#define PMM_COUNT 4096

/// @brief Initialize physical memory manager
void pmm_init();

/// @brief Create physical memory entry @param entry PMM entry @return Pointer to entry
pmm_entry_t* pmm_create(pmm_entry_t entry);

/// @brief Create physical memory entry @param addr Address of entry @param size Size of entry @param type Type of entry @return Pointer to entry
pmm_entry_t* pmm_create_ext(uint32_t addr, uint32_t size, PMMTYPE type);

/// @brief Delete existing physical memory entry @param entry PMM entry @return Result of deletion
bool pmm_delete(pmm_entry_t entry);