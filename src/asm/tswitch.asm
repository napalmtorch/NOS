[GLOBAL _thread_switch]
[EXTERN _thread_current]
[EXTERN _thread_next]

_thread_switch:
    cli
    mov EAX, [_thread_current]

    mov [EAX+0],  ESP
    mov [EAX+4],  EBP
    mov [EAX+8],  ESI
    mov [EAX+12], EDI
    mov [EAX+16], EAX
    mov [EAX+20], EBX
    mov [EAX+24], ECX
    mov [EAX+28], EDX

    mov ECX, CR3
    mov [EAX+32], ECX

    pushf
    pop ECX
    mov [EAX+36], ECX

    mov EAX, [_thread_next]

    mov ECX, [EAX+36]
    push ECX
    popf

    mov ESP, [EAX+0]
    mov EBP, [EAX+4]
    mov ESI, [EAX+8]
    mov EDI, [EAX+12]
    mov EBX, [EAX+20]

    mov ECX, [EAX+32]
    mov CR3, ECX

    mov ECX, [EAX+24]
    mov EDX, [EAX+28]

    mov [_thread_current], EAX
    mov EAX, [EAX+16]
    
    sti
    ret