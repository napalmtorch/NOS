#include <kernel/hal/interrupts/syscall.h>
#include <kernel/core/kernel.h>

const syscall_args_t NULL_SYSCALL_ARGS = { 0, 0, 0, 0 };

void syscall_callback(idt_regs_t* regs);
int syscall_free_index();

syscall_t**     syscalls_list;
syscall_args_t* syscalls_queue;
uint32_t        syscalls_count;
uint32_t        syscalls_count_max;
uint32_t        syscalls_queue_index;

void syscall_init()
{
    syscalls_count_max   = 4096;
    syscalls_count       = 0;
    syscalls_queue_index = 0;
    syscalls_list        = kmalloc(sizeof(syscall_t*) * syscalls_count_max, HEAPTYPE_PTRARRAY);
    syscalls_queue       = kmalloc(sizeof(syscall_args_t) * syscalls_count_max, HEAPTYPE_ARRAY);

    syscall_register(&SYSCALL_VPRINTF);
    syscall_register(&SYSCALL_METHODS);
    syscall_register(&SYSCALL_VBEMODE);

    irq_register(IRQ_SYSCALL, syscall_callback);
    debug_ok("Initialized system calls");
}

void syscall_callback(idt_regs_t* regs)
{
    syscall_args_t args = { regs->ebx, regs->edx, regs->ecx, regs->eax };
	syscall_handle_args(args);
}

bool syscall_monitor()
{
    while (syscalls_queue_index > 0)
    {
        syscall_args_t args = syscall_pop();
        syscall_execute(args);
        if (syscalls_queue_index < 0) { syscalls_queue_index = -1; break; }
    }
}

bool syscall_register(syscall_t* syscall)
{
    if (syscall == NULL) { return false; }
    int i = syscall_free_index();
    if (i < 0 || i >= syscalls_count_max) { debug_error("Maximum amount of registered system calls reached"); return false; }
    syscalls_list[i] = syscall;
    syscalls_count++;
    debug_info("Registered system call - CODE: 0x%8x, HANDLER: 0x%8x, NAME: '%s'", syscall->code, syscall->handler, syscall->name);
    return true;
}

bool syscall_push(syscall_args_t args)
{
    if (syscalls_queue_index >= syscalls_count_max) { debug_error("System call queue overflow"); return false; }
    syscalls_queue[syscalls_queue_index] = args;
    syscalls_queue_index++;
    return true;
}

syscall_args_t syscall_pop()
{
    syscalls_queue_index--;
    if (syscalls_queue_index < 0) { syscalls_queue_index = -1; return NULL_SYSCALL_ARGS; }
    syscall_args_t args = syscalls_queue[syscalls_queue_index];
    return args;
}

void syscall_handle_args(syscall_args_t args)
{
    syscall_t* syscall = NULL;
    for (int i = 0; i < syscalls_count_max; i++)
    {
        if (syscalls_list[i]->code == args.code)
        {
            if (syscalls_list[i]->direct)
            {
                if (syscalls_list[i]->handler != NULL) 
                { 
                    syscalls_list[i]->handler(args); 
                    //debug_info("Executed system call '%s' - ARGS: 0x%8x, 0x%8x, 0x%8x, 0x%8x", syscalls_list[i]->name, args.arg, args.src, args.code, args.dest);
                    return; 
                }
                else 
                { 
                    debug_error("Invalid handler for system call - CODE: 0x%8x, NAME: '%s'", syscalls_list[i]->code, syscalls_list[i]->name);
                    return; 
                }
            }
            else { syscall_push(args); return; }
        }
    }
}

bool syscall_execute(syscall_args_t args)
{
    for (int i = 0; i < syscalls_count_max; i++)
    {
        if (syscalls_list[i] == NULL) { continue; }
        if (syscalls_list[i]->code == args.code)
        {
            if (syscalls_list[i]->handler != NULL) { syscalls_list[i]->handler(args); return true; }
            else 
            { 
                debug_error("Invalid handler for system call - CODE: 0x%8x, NAME: '%s'", syscalls_list[i]->code, syscalls_list[i]->name);
                return false; 
            }
        }
    }

    debug_error("Unknown system call 0x%8x", args.code);
    return false;
}

int syscall_free_index()
{
    for (int i = 0; i < syscalls_count_max; i++)
    {
        if (syscalls_list[i] == NULL) { return i; }
    }
    return -1;
}