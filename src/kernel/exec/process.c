#include <kernel/exec/process.h>
#include <kernel/core/kernel.h>

int proc_free_index(process_t* proc);

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
    proc->ctrl.count          = 0;
    proc->ctrl.id             = 1;
    proc->ctrl.index          = 0;
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

process_t* proc_create_elf(char* name, array_t* elf_data, char** argv, int argc)
{
    if (elf_data == NULL)        { return NULL; }
    if (elf_data->items == NULL) { return NULL; }

    elf_header_t* elf_hdr = elf_data->items;
    process_t*    proc    = kmalloc(sizeof(process_t), HEAPTYPE_PROC);

    char* tname[32];
    memset(tname, 0, 32);
    strcpy(tname, name);
    strcat(tname, "_entry");
    thread_t*  thread = thread_create(tname, 0x10000, elf_hdr->entry, argv, argc);

    proc->ctrl.threads        = kmalloc(sizeof(thread_t*) * THREAD_COUNT, HEAPTYPE_PTRARRAY);
    proc->ctrl.threads[0]     = thread;
    proc->ctrl.count          = 0;
    proc->ctrl.id             = 1;
    proc->ctrl.index          = 0;
    proc->ctrl.thread_current = thread;
    proc->ctrl.switches       = 0;

    strcpy(proc->name, name);
    proc->id            = proc_id++;
    proc->messages      = kmalloc(sizeof(process_msg_t) * PROCMSG_COUNT, HEAPTYPE_ARRAY);
    proc->message_count = 0;
    proc->privilege     = PROCPRIV_SYSTEM;
    proc->running       = false;

    if (!elf_load(elf_data->items, elf_data->count, proc)) 
    {
        debug_error("Unable to create process %d", proc->id); 
        kfree(thread->ctrl.stack);
        kfree(thread);
        kfree(proc->ctrl.threads);
        kfree(proc->messages);
        kfree(proc);
        return false;
    }

    debug_info("Created process - ID: %d, ENTRY: 0x%8x, ENTRY_THREAD: 0x%8x, PRIV: 0x%2x, NAME: '%s'",
            proc->id, thread->ctrl.entry, thread, proc->privilege, proc->name);
    return proc;
}

bool proc_load_thread(process_t* proc, thread_t* thread)
{
    if (proc == NULL || thread == NULL) { return false; }
    int i = proc_free_index(proc);
    if (i < 0 || i >= THREAD_COUNT) { debug_error("Maximum amount of processes reached"); return false; }
    thread->ctrl.registers.cr3 = vmm_get_kdir()->address;
    proc->ctrl.threads[i] = thread;
    proc->ctrl.count++;
    thread->state = THREADSTATE_RUNNING;
    debug_info("Loaded thread %d('%s') into process %d('%s')", thread->id, thread->name, proc->id, proc->name);
    return true;
}

bool proc_unload_thread(process_t* proc, thread_t* thread)
{
    if (proc == NULL || thread == NULL) { return false; }
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (proc->ctrl.threads[i] == thread)
        {
            debug_info("Unloading thread %d('%s')", thread->id, thread->name);
            kfree(thread->ctrl.stack);
            kfree(thread);
            proc->ctrl.threads[i] = NULL;
            proc->ctrl.count--;
            debug_ok("Finished unloading thread");
            return true;
        }
    }
    return false;
}

bool proc_msg_is_ready(process_t* proc)
{
    if (proc == NULL) { return false; }
    return proc->message_count > 0;
}

bool proc_msg_push(process_t* proc, process_msg_t msg)
{
    if (proc == NULL) { return false; }
    if (proc->message_count >= PROCMSG_COUNT) { debug_error("Process '%s' IPC overflow"); }
}

process_msg_t proc_msg_pop(process_t* proc)
{
    
}

int proc_free_index(process_t* proc)
{
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (proc->ctrl.threads[i] == NULL) { return i; }
    }
    return -1;
}
