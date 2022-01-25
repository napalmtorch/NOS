#include <kernel/exec/process.h>
#include <kernel/core/kernel.h>

uint32_t      proc_id = 0;

process_t* proc_create_kernel()
{
    thread_t*  kthread = thread_create_kernel();
    thread_t*  ithread = thread_create_idle();
    process_t* proc    = proc_create("kernel.elf", kthread);

    kthread->state = THREADSTATE_RUNNING;
    ithread->state = THREADSTATE_RUNNING;
    ithread->ctrl.registers.cr3 = vmm_get_kdir()->address;
    ithread->id = 1;

    proc->running = true;
    proc->ctrl.threads[1] = ithread;
    proc->ctrl.count++;
    return proc;
}

process_t* proc_create(char* name, thread_t* entry_thread)
{
    if (entry_thread == NULL) { return NULL; }
    process_t* proc = kmalloc(sizeof(process_t), HEAPTYPE_PROC);

    proc->ctrl.threads        = kmalloc(sizeof(thread_t*) * THREAD_COUNT, HEAPTYPE_PTRARRAY);
    proc->ctrl.threads[0]     = entry_thread;
    proc->ctrl.count   = 0;
    proc->ctrl.id      = 1;
    proc->ctrl.index   = 0;
    proc->ctrl.thread_current = entry_thread;
    proc->ctrl.switches       = 0;

    strcpy(proc->name, name);
    proc->id            = proc_id++;
    proc->messages      = kmalloc(sizeof(process_msg_t) * PROCMSG_COUNT, HEAPTYPE_ARRAY);
    proc->message_count = 0;
    proc->privilege     = PROCPRIV_SYSTEM;
    proc->running       = false;

    debug_info("Created process - ID: %d, ENTRY: 0x%8x, ENTRY_THREAD: 0x%8x, PRIV: 0x%2x, NAME: '%s'",
            proc->id, entry_thread->ctrl.entry, entry_thread, proc->privilege, proc->name);
    return proc;
}

process_t* proc_create_elf(char* name, array_t* elf_data)
{

}