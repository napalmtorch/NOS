#pragma once
#include <lib/stdint.h>
#include <kernel/fs/vfs.h>

typedef struct
{
    uint32_t address;
    char     section[32];
    char     name[128 - sizeof(uint32_t)];
} PACKED objdump_symbol_t;

typedef struct
{
    char*             filename;
    vfs_file_t        file;
    objdump_symbol_t* methods;
    uint32_t          method_count;
} PACKED objdump_t;

extern objdump_t KERNEL_OBJDUMP;
extern objdump_t USERLIB_OBJDUMP;

void  objdump_load(objdump_t* dump, char* filename);
uint32_t objdump_get_count(objdump_t* dump);
objdump_symbol_t* objdump_parse(objdump_t* dump);