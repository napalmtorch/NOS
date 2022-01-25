#pragma once
#include <lib/stdint.h>

uint8_t  port_inb(uint16_t port);
uint16_t port_inw(uint16_t port);
uint32_t port_inl(uint16_t port);
void     port_instr(uint16_t port, uint8_t* data, uint32_t size);

void port_outb(uint16_t port, uint8_t data);
void port_outw(uint16_t port, uint16_t data);
void port_outl(uint16_t port, uint32_t data);
void port_outstr(uint16_t port, uint8_t* data, uint32_t size);