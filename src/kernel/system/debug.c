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
    int bytes_per_line = 16;
    char temp[16];
    char chars[bytes_per_line];
    memset(temp, 0, 16);
    memset(chars, 0, bytes_per_line);

    debug_print("Dumping memory at: ");
    strhex((uint32_t)ptr, temp, false, 4);
    debug_print_col(temp, 0x0B);
    debug_newline();

    int xx = 0;
    uint32_t pos = 0;
    for (size_t i = 0; i < (size / bytes_per_line); i++)
    {              
        // address range
        pos = i * bytes_per_line;
        memset(temp, 0, 16);
        memset(chars, 0, bytes_per_line);
        strhex((uint32_t)(ptr + pos), temp, true, 4);
        debug_print_col(temp, 0x0B); debug_print(":");
        memset(temp, 0, 16);
        strhex((uint32_t)(ptr + pos + (bytes_per_line - 1)), temp, true, 4);
        debug_print_col(temp, 0x0B);
        debug_print("    ");

        // bytes
        for (size_t j = 0; j < bytes_per_line; j++)
        {
            memset(temp, 0, 16);
            strhex(ptr[pos + j], temp, false, 1);
            if (ptr[pos + j] > 0) { debug_print(temp); }
            else { debug_print_col(temp, 0x0C); }
            debug_print(" ");

            if (ptr[pos + j] >= 32 && ptr[pos + j] <= 126) { stradd(chars, ptr[pos + j]); }
            else { stradd(chars, '.'); }
        }

        debug_print("    ");
        debug_print_col(chars, 0x0E);
        debug_newline();
    }
}

void debug_dumpregs(void* regs, bool irq)
{
    if (regs == NULL) { return; }
    if (irq)
    {
        idt_regs_t* r = regs;
        debug_printf("EAX: 0x%8x EBX: 0x%8x ECX: 0x%8x EDX: 0x%8x\n", r->eax, r->ebx, r->ecx, r->edx);
        debug_printf("ESI: 0x%8x EDI: 0x%8x ESP: 0x%8x EBP: 0x%8x\n", r->esi, r->edi, r->esp, r->ebp);
        debug_printf("EIP: 0x%8x EFL: 0x%8x INT: 0x%8x ERR: 0x%8x\n", r->eip, r->eflags, r->irq, r->err);
        debug_printf("CR0: 0x%8x CR2: 0x%8x CR3: 0x%8x CR4: 0x%8x\n", regread_cr0(), regread_cr2(), regread_cr3(), regread_cr4());
    }
    else
    {
        thread_regs_t* r = regs;
        debug_printf("EAX: 0x%8x EBX: 0x%8x ECX: 0x%8x EDX: 0x%8x\n", r->eax, r->ebx, r->ecx, r->edx);
        debug_printf("ESI: 0x%8x EDI: 0x%8x ESP: 0x%8x EBP: 0x%8x\n", r->esi, r->edi, r->esp, r->ebp);
        debug_printf("EIP: 0x%8x EFL: 0x%8x\n", r->eip, r->eflags);
        debug_printf("CR0: 0x%8x CR2: 0x%8x CR3: 0x%8x CR4: 0x%8x\n", regread_cr0(), regread_cr2(), r->cr3, regread_cr4());
    }
}

void debug_dumpregs_raw()
{

}

void panic(EXCEPTION code, void* regs, bool irq)
{
    asm volatile("cli");
    if (code >= EXC_COUNT) { code = EXC_UNKNOWN; }
    debug_print("---- ");
    debug_print_col("KERNEL PANIC", COL4_RED);
    debug_println(" ----------------------------------");
    debug_error("Error code: '%s'", EXCEPTION_MSGS[code]);
    debug_printf("PROC: '%s'('%s')\n", PROCMGR.current->name, _thread_current->name);
    debug_dumpregs(regs, irq);
    if (PROCMGR.current->id == 0) { debug_halt(); }
    else { procmgr_kill(PROCMGR.current); asm volatile("sti"); }
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