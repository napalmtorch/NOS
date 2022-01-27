#include <kernel/exec/process.h>
#include <kernel/core/kernel.h>

extern void _thread_switch();

int procmgr_free_index();

process_mgr_t PROCMGR;
bool          proc_switch, proc_unloaded;
uint32_t      proc_index_old;

void procmgr_init()
{
    proc_switch = false;
    PROC_KERNEL = proc_create_kernel();
    procmgr_unready();

    PROCMGR.pit_ready    = false;
    PROCMGR.count        = 1;
    PROCMGR.index        = 0;
    PROCMGR.current      = PROC_KERNEL;
    PROCMGR.next         = PROC_KERNEL;
    PROCMGR.processes    = kmalloc(sizeof(process_t*) * PROCMGR_COUNT, HEAPTYPE_PTRARRAY);
    PROCMGR.processes[0] = PROC_KERNEL;

    debug_ok("Initialized process manager");
}

void procmgr_ready()
{
    PROCMGR.ready     = true;
    PROCMGR.pit_ready = true;
}

void procmgr_unready()
{
    PROCMGR.ready     = false;
    PROCMGR.pit_ready = false;
}

bool procmgr_load(process_t* proc)
{
    if (proc == NULL) { debug_error("Tried to load null process"); return false; }
    int i = procmgr_free_index();
    if (i < 0 || i >= PROCMGR_COUNT) { debug_error("Maximum amount of processes reached"); return false; }
    PROCMGR.processes[i] = proc;
    PROCMGR.processes[i]->running = true;
    PROCMGR.count++;
    debug_info("Loaded process %d('%s')", proc->id, proc->name);
    return true;
}

bool procmgr_unload(process_t* proc)
{
    if (proc == NULL) { debug_error("Tried to unload null process"); return false; }
    for (int i = 0; i < PROCMGR_COUNT; i++)
    {
        if (PROCMGR.processes[i] == proc)
        {
            for (int j = 0; j < THREAD_COUNT; j++)
            {
                if (PROCMGR.processes[i]->ctrl.threads[j] == NULL) { continue; }
                PROCMGR.processes[i]->ctrl.threads[j]->state = THREADSTATE_TERMINATED;
                proc_unload_thread(PROCMGR.processes[i], PROCMGR.processes[i]->ctrl.threads[j]);
            }

            debug_info("Unloading process %d('%s')", proc->id, proc->name);
            kfree(PROCMGR.processes[i]->executable.elf_data);
            kfree(PROCMGR.processes[i]->ctrl.threads);
            //kfree(PROCMGR.processes[i]->page_dir);
            kfree(PROCMGR.processes[i]);
            PROCMGR.processes[i] = NULL;
            PROCMGR.count--;
            debug_ok("Finished unloading process");
            return true;
        }
    }

    return false;
}

bool procmgr_kill(process_t* proc)
{
    if (proc == NULL) { return false; }
    for (int i = 1; i < PROCMGR_COUNT; i++)
    {
        if (PROCMGR.processes[i] == proc) { PROCMGR.processes[i]->running = false; return true; }
    }
    return false;
}

bool procmgr_kill_byname(char* name)
{

}

bool procmgr_kill_byid(uint32_t id)
{

}

bool procmgr_kill_byindex(int index)
{

}

thread_t* curthread;
void procmgr_schedule(bool interrupts)
{
    PROCMGR.pit_ready = false;
    if (!interrupts) { _cli(); }
    if (!PROCMGR.ready) { _sti(); return; }
    proc_switch = false;

    PROCMGR.current = PROCMGR.processes[PROCMGR.index];
    _thread_current = PROCMGR.current->ctrl.threads[PROCMGR.current->ctrl.index];
    if (_thread_current == NULL) { proc_switch = true; }
    if (_thread_current->locked) { PROCMGR.pit_ready = true; _sti(); return; }

    if (!proc_switch)
    {
        proc_index_old = PROCMGR.current->ctrl.index;
        _thread_next = procmgr_next_thread(PROCMGR.current);
        if (_thread_next != _thread_current) { PROCMGR.current->ctrl.switches++; }
        if (PROCMGR.current->ctrl.index == proc_index_old) { proc_switch = true; }
    }

    if (PROCMGR.current->ctrl.switches >= PROCMGR.current->ctrl.count) { PROCMGR.current->ctrl.switches = 0; proc_switch = true; }

    if (_thread_next != NULL)
    {
        if (_thread_next->state == THREADSTATE_TERMINATED) { proc_unload_thread(PROCMGR.current, _thread_next); proc_unloaded = true; }
        proc_switch = true;
    }

    if (proc_switch)
    {
        PROCMGR.next = procmgr_next_process();
        if (proc_unloaded) { PROCMGR.next = PROCMGR.processes[0]; _thread_next = PROCMGR.next->ctrl.threads[0]; PROCMGR.index = 0; PROCMGR.next->ctrl.index = 0; }
        if (PROCMGR.next != PROCMGR.current) { _thread_next = PROCMGR.next->ctrl.threads[0]; PROCMGR.next->ctrl.index = 0; }
        //debug_info("CUR - P: '%s', T: '%s'   |   NEXT - P: '%s', T: '%s'\n", PROCMGR.current->name, _thread_current->name, PROCMGR.next->name, _thread_next->name);
        PROCMGR.current = PROCMGR.next;
        proc_unloaded = false;
    }
    else { PROCMGR.next = PROCMGR.current; }

    PROCMGR.pit_ready = true;
    _thread_switch();
}

process_t* procmgr_next_process()
{
    PROCMGR.index++;
    while (true)
    {
        if (PROCMGR.index >= PROCMGR_COUNT) { PROCMGR.index = 0; break; }
        if (PROCMGR.processes[PROCMGR.index] == NULL) { PROCMGR.index++; continue; }
        if (!PROCMGR.processes[PROCMGR.index]->running)
        {
            procmgr_unload(PROCMGR.processes[PROCMGR.index]);
            proc_unloaded = true;
            PROCMGR.index = 0;
            return PROCMGR.processes[0];
        }
        if (PROCMGR.processes[PROCMGR.index]->ctrl.count == 0) { break; }
        else { PROCMGR.processes[PROCMGR.index]->running = false; PROCMGR.index++; continue; }
        PROCMGR.index++;
    }
    return PROCMGR.processes[PROCMGR.index];
}

thread_t* procmgr_next_thread(process_t* proc)
{
    proc->ctrl.index++;
    while (true)
    {
        if (proc->ctrl.index >= THREAD_COUNT) { proc->ctrl.index = 0; return proc->ctrl.threads[0]; }
        if (proc->ctrl.threads[proc->ctrl.index] == NULL) { proc->ctrl.index++; continue; }
        break;
    }
    return proc->ctrl.threads[proc->ctrl.index];
}

process_t* procmgr_get_byname(process_t* proc)
{

}

process_t* procmgr_get_byid(uint32_t id)
{

}

process_t* procmgr_get_byindex(int index)
{

}

int procmgr_free_index()
{
    for (uint32_t i = 0; i < PROCMGR_COUNT; i++)
    {
        if (PROCMGR.processes[i] == NULL) { return (int)i; }
    }
    return -1;
}