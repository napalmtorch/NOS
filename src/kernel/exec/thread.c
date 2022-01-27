#include <kernel/exec/thread.h>
#include <kernel/core/kernel.h>

const char* KTHREAD_NAME = "kernel";

void thread_exit();

thread_t* _thread_current;
thread_t* _thread_next;
uint32_t  thread_id = 0;

thread_t* thread_create_kernel()
{
    debug_info("Creating kernel thread...");
    thread_t* thread = kmalloc(sizeof(thread_t), HEAPTYPE_THREAD);
    _thread_current = thread;
    
    strcpy(thread->name, KTHREAD_NAME);
    thread_id     = 0;
    thread->id    = thread_id++;
    thread->state = THREADSTATE_RUNNING;
    
    thread->ctrl.entry         = _kernel_entry;
    thread->ctrl.stack         = stack_get_bottom();
    thread->ctrl.stack_size    = stack_get_size();
    thread->ctrl.registers.cr3 = vmm_get_kdir()->address;
    
    debug_info("Created thread - ID: %d, ENTRY: 0x%8x, STACK: 0x%8x-0x%8x, NAME: '%s'", 
            thread->id, thread->ctrl.entry, thread->ctrl.stack, thread->ctrl.stack + thread->ctrl.stack_size, thread->name);
    return thread;
}

thread_t* thread_create_idle()
{
    debug_info("Creating idle thread...");
    thread_t* thread = kmalloc(sizeof(thread_t), HEAPTYPE_THREAD);

    strcpy(thread->name, "idle");
    thread->id = thread_id++;
    thread->state = THREADSTATE_RUNNING;

        // set thread control block
    thread->ctrl.entry      = idle_main;
    thread->ctrl.stack_size = 16384;
    thread->ctrl.exit_code  = 0;
    thread->ctrl.stack      = kmalloc(thread->ctrl.stack_size, HEAPTYPE_THREADSTACK);

    // push starting stack values
    uint32_t* stk = (uint32_t*)((uint32_t)thread->ctrl.stack + (thread->ctrl.stack_size - (sizeof(thread_regs_t) + 20)));
    *--stk = (uint32_t)thread;
    *--stk = (uint32_t)0;
    *--stk = (uint32_t)NULL;
    *--stk = (uint32_t)thread_exit;
    *--stk = (uint32_t)idle_main;

    // set thread registers
    thread->ctrl.registers = (thread_regs_t){ .eflags = 0x202, .esp = (uint32_t)stk, .esi = (uint32_t)idle_main, .cr3 = vmm_get_kdir()->address };

    // finished
    debug_info("Created thread - ID: %d, ENTRY: 0x%8x, STACK: 0x%8x-0x%8x, NAME: '%s'", 
            thread->id, thread->ctrl.entry, thread->ctrl.stack, thread->ctrl.stack + thread->ctrl.stack_size, thread->name);
    return thread;
}

thread_t* thread_create(char* name, uint32_t stack_size, thread_entry_t entry, char** argc, int argv)
{
    thread_t* thread = heap_alloc(&HEAP_KERNEL, sizeof(thread_t), true, HEAPTYPE_THREAD);

    // set thread id
    strcpy(thread->name, name);
    thread->id     = thread_id++;
    thread->state  = THREADSTATE_RUNNING;

    // set thread control block
    thread->ctrl.entry      = entry;
    thread->ctrl.stack_size = clamp(stack_size, THREAD_STACK_MIN, THREAD_STACK_MAX);
    thread->ctrl.exit_code  = 0;
    thread->ctrl.stack      = heap_alloc(&HEAP_KERNEL, thread->ctrl.stack_size, true, HEAPTYPE_THREADSTACK);

    // push starting stack values
    uint32_t* stk = (uint32_t*)((uint32_t)thread->ctrl.stack + (thread->ctrl.stack_size - (sizeof(thread_regs_t) + 20)));
    *--stk = (uint32_t)thread;
    *--stk = (uint32_t)argc;
    *--stk = (uint32_t)argv;
    *--stk = (uint32_t)thread_exit;
    *--stk = (uint32_t)entry;

    // set thread registers
    thread->ctrl.registers = (thread_regs_t)
    { 
        .eflags = 0x202, 
        .esp = (uint32_t)stk, 
        .esi = (uint32_t)entry, 
        .cr3 = vmm_get_kdir()->address,
    };

    // finished
    debug_info("Created thread - ID: %d, ENTRY: 0x%8x, STACK: 0x%8x-0x%8x, NAME: '%s'", 
            thread->id, thread->ctrl.entry, thread->ctrl.stack, thread->ctrl.stack + thread->ctrl.stack_size, thread->name);
    return thread;
}

void thread_exit()
{
    asm volatile("cli");
    register int eax asm("eax");
    uint32_t exit_code = eax;

    asm volatile("cli");
    debug_info("Thread '%s' exited with code %d", _thread_current->name, exit_code);
    _thread_current->state = THREADSTATE_TERMINATED;
    asm volatile("sti");

    while (true);
}