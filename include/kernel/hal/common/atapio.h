#pragma once
#include <lib/stdint.h>

void atapio_init();
bool atapio_identify();
void atapio_read(uint64_t sector, uint32_t count, uint8_t* buffer);
void atapio_write(uint64_t sector, uint32_t count, uint8_t* buffer);