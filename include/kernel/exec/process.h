#pragma once
#include <lib/stdint.h>
#include <lib/stddef.h>
#include <kernel/exec/exec.h>
#include <kernel/exec/thread.h>

typedef enum
{
    PROCPRIV_SYSTEM,
    PROCPRIV_DRIVER,
    PROCPRIV_USER,
} PROCPRIV;

typedef struct
{
    char     msg[120];
    uint32_t size;
    void*    data;
} PACKED process_msg_t;

typedef struct
{
    thread_t**     threads;
    thread_t*      thread_current;
    uint32_t       count;
    uint32_t       index;
    uint32_t       id;
    uint32_t       switches;
} PACKED process_ctrl_t;

typedef struct
{
    executable_t   executable;
    process_msg_t* messages;
    process_ctrl_t ctrl;
    PROCPRIV       privilege;
    uint32_t       message_count;
    bool           running;
    uint32_t       id;
    char           name[32];
} PACKED process_t;

typedef struct
{
    process_t** processes;
    process_t*  current;
    process_t*  next;
    uint32_t    index;
    uint32_t    count;
    bool        ready;
    bool        pit_ready;
} PACKED process_mgr_t;

#define THREAD_COUNT  1024
#define PROCMGR_COUNT 8192
#define PROCMSG_COUNT 4096

extern process_mgr_t PROCMGR;

int idle_main(char** argv, int argc);

process_t* proc_create_kernel();
process_t* proc_create(char* name, thread_t* entry_thread);
process_t* proc_create_elf(char* name, array_t* elf_data);

void procmgr_init();
void procmgr_ready();
void procmgr_unready();
bool procmgr_load(process_t* proc);
bool procmgr_unload(process_t* proc);
bool procmgr_kill(process_t* proc);
bool procmgr_kill_byname(char* name);
bool procmgr_kill_byid(uint32_t id);
bool procmgr_kill_byindex(int index);
void procmgr_schedule(bool interrupts);

process_t* procmgr_next_process();
thread_t*  procmgr_next_thread(process_t* proc);

process_t* procmgr_get_byname(process_t* proc);
process_t* procmgr_get_byid(uint32_t id);
process_t* procmgr_get_byindex(int index);