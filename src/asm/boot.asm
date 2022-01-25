[BITS 32]
[GLOBAL _mboot]
[GLOBAL _start]
[GLOBAL _stack_top]
[GLOBAL _stack_bottom]
[GLOBAL _boot_page_dir0]
[GLOBAL _boot_page_dir0_end]
[EXTERN _kernel_entry]
[EXTERN _kernel_start]
[EXTERN _kernel_end]

KBASE equ 0xC0000000
KPAGE_NUM equ (KBASE >> 22)

MBOOT_PAGE_ALIGN    equ 1 << 0
MBOOT_MEM_INFO      equ 1 << 1 
MBOOT_VID_INFO      equ 1 << 2
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_VID_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .data
align 4096
_boot_page_dir0:
    dd 0x00000083
    times (KPAGE_NUM - 1) dd 0
    dd 0x00000083
    times (1024 - KPAGE_NUM - 1) dd 0
    times 520192 db 0
_boot_page_dir0_end:

section .multiboot.text
align 4
_mboot:
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM   
    dd _mboot
    dd 0
    dd 0
    dd 0
    dd _start
    dd 0
    dd 800, 600, 32

section .text
_start:
    cli
    mov ecx, (_boot_page_dir0 - KBASE)
    mov cr3, ecx

    mov ecx, cr4
    or  ecx, 0x00000010
    mov cr4, ecx

    mov ecx, cr0
    or  ecx, 0x80000000
    mov cr0, ecx

    lea ecx, [_start_higher_half]
    jmp ecx

_start_higher_half:
    cli
    mov  ebp, _stack_top            ; set stack base
    mov  esp, ebp                   ; set stack pointer
    add  ebx, KBASE
    push ebx                        ; push multiboot address to stack
    call _kernel_entry              ; jump to kernel entry
    cli                             ; disable interrupts
    hlt                             ; halt the system

section .bss
align 4096
_stack_bottom:
    resb 512 * 1024
_stack_top: