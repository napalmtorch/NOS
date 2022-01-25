#pragma once
#include <lib/stdint.h>

/// @brief ELF identifier information structure
typedef struct
{
    char id[4];
    uint8_t platform;
    uint8_t endianness;
    uint8_t version;
    uint8_t abi;
    uint8_t abi_version;
    uint8_t reserved[7];
} PACKED elf_identifier_t;

/// @brief ELF file structure
typedef struct
{
    elf_identifier_t identifier;
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t ph_table;
    uint32_t sh_table;
    uint32_t flags;
    uint16_t header_size;
    uint16_t ph_entry_size;
    uint16_t ph_entry_count;
    uint16_t sh_entry_size;
    uint16_t sh_entry_count;
    uint16_t sh_string_index;
} PACKED elf_header_t;

/// @brief ELF program header structure
typedef struct
{
    uint32_t type;
    uint32_t offset;
    uint32_t v_address;
    uint32_t p_address;
    uint32_t file_size;
    uint32_t segment_size;
    uint32_t flags;
    uint32_t align;
} PACKED elf_progheader_t;

/// @brief Validate identifier of ELF header @param header ELF file header @return Successfully validated header
bool elf_validate(elf_header_t* header);