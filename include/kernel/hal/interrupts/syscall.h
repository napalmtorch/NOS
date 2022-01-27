#pragma once
#include <lib/stdint.h>

typedef struct
{
    void*    src;
    void*    dest;
    uint32_t code;
    uint32_t arg;
} PACKED syscall_args_t;

typedef void (*syscall_handler_t)(syscall_args_t args);

typedef struct
{
    char*             name;
    uint32_t          code;
    bool              direct;
    syscall_handler_t handler;
} syscall_t;

void syscall_vprintf(syscall_args_t args);
void syscall_methods(syscall_args_t args);
void syscall_vbemode(syscall_args_t args);

static const syscall_t SYSCALL_VPRINTF = { "VPRINTF", 0x00000001, true, syscall_vprintf };
static const syscall_t SYSCALL_METHODS = { "METHODS", 0x00000002, true, syscall_methods };
static const syscall_t SYSCALL_VBEMODE = { "VBEMODE", 0x00000003, true, syscall_vbemode };

void syscall_init();
bool syscall_monitor();
bool syscall_register(syscall_t* syscall);
bool syscall_push(syscall_args_t args);
syscall_args_t syscall_pop();
void syscall_handle_args(syscall_args_t args);
bool syscall_execute(syscall_args_t args);