#pragma once
#include <lib/stdint.h>

/// @brief Convert size using multiple
uint32_t mem_convert_size(uint32_t value);

/// @brief Convert size using mulitple and output string to buffer
char* mem_convert_size_str(char* buff, uint32_t value);

/// @brief Align value
uint32_t mem_align(uint32_t value, uint32_t align);