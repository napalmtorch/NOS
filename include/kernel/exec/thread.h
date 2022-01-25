#pragma once
#include <lib/stdint.h>

/// @internal Thread entry point function type definition
typedef int (*thread_entry_t)(char** argv, int argc);

/// @brief Thread execution states
typedef enum
{
    THREADSTATE_HALTED,
    THREADSTATE_RUNNING,
    THREADSTATE_TERMINATED,
} THREADSTATE;

/// @brief Thread-specific registers structure
typedef struct
{
    uint32_t esp, ebp, esi, edi;
    uint32_t eax, ebx, ecx, edx;
    uint32_t cr3, eflags, eip, pad;
} PACKED thread_regs_t;

/// @brief Thread control block structure
typedef struct
{
    thread_regs_t  registers;
    thread_entry_t entry;
    void*          process;
    void*          stack;
    uint32_t       stack_size;
    uint32_t       exit_code;
} PACKED thread_ctrl_t;

/// @brief Thread properties structure
typedef struct
{
    thread_ctrl_t ctrl;
    THREADSTATE   state;
    bool          locked;
    uint32_t      id;
    char          name[32];
} PACKED thread_t;

#define THREAD_STACK_MIN 4 * KB
#define THREAD_STACK_MAX 8 * MB

extern thread_t* _thread_current;
extern thread_t* _thread_next;

thread_t* thread_create_kernel();
thread_t* thread_create_idle();
thread_t* thread_create(char* name, uint32_t stack_size, thread_entry_t entry, char** argc, int argv);