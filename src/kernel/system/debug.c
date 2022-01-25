#include <kernel/system/debug.h>
#include <kernel/core/kernel.h>

bool debug_enabled;

void debug_toggle(bool state) { debug_enabled = state; }

void debug_halt() { _cli(); _hlt(); }

void debug_breakpoint() { asm volatile("xchg %bx, %bx"); }

void debug_newline()
{
    if (!debug_enabled) { return; }
    serial_newline();
}

void debug_printc(char c)
{
    if (!debug_enabled) { return; }
    serial_printc(c);
}

void debug_print(const char* str)
{
    if (!debug_enabled) { return; }
    serial_print(str);
}

void debug_print_col(const char* str, uint8_t color)
{
    if (!debug_enabled) { return; }
    serial_print_col(str, color);
}

void debug_println(const char* str)
{
    if (!debug_enabled) { return; }
    serial_println(str);
}

void debug_println_col(const char* str, uint8_t color)
{
    if (!debug_enabled) { return; }
    serial_println_col(str, color);
}

int debug_vprintf(const char* str, va_list args)
{
    if (!debug_enabled) { return false; }
    return serial_vprintf(str, args);
}

int debug_printf(const char* str, ...)
{
    if (!debug_enabled) { return false; }
    va_list args;
    va_start(args, str);
    debug_vprintf(str, args);
    va_end(args);
    return true;
}

void debug_header(const char* str, uint8_t color)
{
    debug_printc('[');
    debug_print_col(str, color);
    debug_printc(']');
    debug_printc(' ');
}

void debug_ok(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  OK  ", COL4_GREEN);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_info(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  >>  ", COL4_CYAN);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_warn(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  ??  ", COL4_YELLOW);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_error(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  !!  ", COL4_RED);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_dumpmem(uint8_t* ptr, uint32_t size)
{

}

void debug_dumpregs(void* regs, bool irq)
{

}

void debug_dumpregs_raw()
{

}

void panic(EXCEPTION code, void* regs, bool irq)
{
    if (code >= EXC_COUNT) { code = EXC_UNKNOWN; }
    debug_print("---- ");
    debug_print_col("KERNEL PANIC", COL4_RED);
    debug_println(" ----");
    debug_error("Error code: '%s'", EXCEPTION_MSGS[code]);
    debug_dumpregs(regs, irq);
    if (PROCMGR.current->id == 0) { debug_halt(); }
}

void panicf(EXCEPTION code, void* regs, bool irq, const char* str, ...)
{
    va_list args;
    va_start(args, str);
    if (code >= EXC_COUNT) { code = EXC_UNKNOWN; }
    debug_print("---- ");
    debug_print_col("KERNEL PANIC", COL4_RED);
    debug_println(" ----");
    debug_error("Error code: '%s'", EXCEPTION_MSGS[code]);
    debug_vprintf(str, args);
    debug_dumpregs(regs, irq);
    va_end(args);
    if (PROCMGR.current->id == 0) { debug_halt(); }
}