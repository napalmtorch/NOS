#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <kernel/hal/interrupts/idt.h>
#include <kernel/exec/thread.h>

typedef enum
{
    EXC_CPU_DIVBYZERO,
    EXC_CPU_DEBUG,
    EXC_CPU_NMI,
    EXC_CPU_BREAKPOINT,
    EXC_CPU_OVERFLOW,
    EXC_CPU_OUTOFBOUNDS,
    EXC_CPU_INVALIDOP,
    EXC_CPU_NO_COPROC,
    EXC_CPU_DOUBLE_FAULT,
    EXC_CPU_COPROC_SEG_OVERRUN,
    EXC_CPU_BADTSS,
    EXC_CPU_SEG_NOT_PRESENT,
    EXC_CPU_STACK_FAULT,
    EXC_CPU_GPF_FAULT,
    EXC_CPU_PAGE_FAULT,
    EXC_CPU_UNKNOWN_INT,
    EXC_CPU_COPROC_FAULT,
    EXC_CPU_ALIGNMENT_CHECK,
    EXC_CPU_MACHINE_CHECK,
    EXC_UNKNOWN_ARG,
    EXC_NULL_ARG,
    EXC_NULLPTR,
    EXC_OUTOFBOUNDS,
    EXC_INDEX_OUTOFBOUNDS,
    EXC_INVALID_FORMAT,
    EXC_NOT_SUPPORTED,
    EXC_OUT_OF_MEMORY,
    EXC_TIMEOUT,
    EXC_INVALID_FILE,
    EXC_DISKRD_ERR,
    EXC_DISKWR_ERR,
    EXC_HEAP_CORRUPT,
    EXC_UNKNOWN,
    EXC_COUNT,
} EXCEPTION;

static const char* EXCEPTION_MSGS[EXC_COUNT] = 
{
    "CPU_DIVBYZERO",
    "CPU_DEBUG",
    "CPU_NMI",
    "CPU_BREAKPOINT",
    "CPU_OVERFLOW",
    "CPU_OUTOFBOUNDS",
    "CPU_INVALIDOP",
    "CPU_NO_COPROC",
    "CPU_DOUBLE_FAULT",
    "CPU_COPROC_SEG_OVERRUN",
    "CPU_BADTSS",
    "CPU_SEG_NOT_PRESENT",
    "CPU_STACK_FAULT",
    "CPU_GPF_FAULT",
    "CPU_PAGE_FAULT",
    "CPU_UNKNOWN_INT",
    "CPU_COPROC_FAULT",
    "CPU_ALIGNMENT_CHECK",
    "CPU_MACHINE_CHECK",
    "UNKNOWN_ARG",
    "NULL_ARG",
    "NULL_POINTER",
    "OUTOFBOUNDS",
    "INDEX_OUTOFBOUNDS",
    "INVALID_FORMAT",
    "NOT_SUPPORTED",
    "OUT_OF_MEMORY",
    "TIMEOUT",
    "INVALID_FILE",
    "DISKRD_ERR",
    "DISKWR_ERR",
    "HEAP_CORRUPT",
    "UNKNOWN,"
};

void      debug_toggle(bool state);
void      debug_halt();
void      debug_breakpoint();

void debug_newline();
void debug_printc(char c);
void debug_print(const char* str);
void debug_print_col(const char* str, uint8_t color);
void debug_println(const char* str);
void debug_println_col(const char* str, uint8_t color);
int  debug_vprintf(const char* str, va_list args);
int  debug_printf(const char* str, ...);

void debug_header(const char* str, uint8_t color);
void debug_ok(const char* str, ...);
void debug_info(const char* str, ...);
void debug_warn(const char* str, ...);
void debug_error(const char* str, ...);

void debug_dumpmem(uint8_t* ptr, uint32_t size);
void debug_dumpregs(void* regs, bool irq);
void debug_dumpregs_raw();

void panic(EXCEPTION code, void* regs, bool irq);
void panicf(EXCEPTION code, void* regs, bool irq, const char* str, ...);