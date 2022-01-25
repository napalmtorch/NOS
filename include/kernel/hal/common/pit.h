#pragma once
#include <lib/stdint.h>
#include <kernel/hal/interrupts/idt.h>

void pit_init(uint32_t freq, irq_handler_t handler);
void pit_disable();
void pit_callback(idt_regs_t* regs);
void pit_calculate();

uint64_t pit_get_ticks();
uint32_t pit_get_millis();
uint64_t pit_get_millis_total();
uint32_t pit_get_seconds();