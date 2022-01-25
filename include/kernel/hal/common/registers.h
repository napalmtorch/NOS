#pragma once
#include <lib/stdint.h>

extern uint32_t regread_cr0();
extern uint32_t regread_cr2();
extern uint32_t regread_cr3();
extern uint32_t regread_cr4();
extern uint32_t regread_ds();
extern uint32_t regread_ss();

extern void regwrite_cr0(uint32_t value);
extern void regwrite_cr3(uint32_t value);
extern void regwrite_ds(uint16_t value);