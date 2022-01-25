#pragma once
#include <lib/stdint.h>
#include <kernel/exec/elf.h>

typedef struct
{
    uint8_t*          data;
    uint32_t          size;
    elf_header_t*     elf;
} PACKED executable_t;