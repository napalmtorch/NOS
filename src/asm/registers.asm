[GLOBAL regread_cr0]
[GLOBAL regread_cr2]
[GLOBAL regread_cr3]
[GLOBAL regread_cr4]
[GLOBAL regread_ds]
[GLOBAL regread_ss]
[GLOBAL regwrite_cr0]
[GLOBAL regwrite_cr3]
[GLOBAL regwrite_ds]

regread_cr0:
    mov EAX, CR0
    retn

regread_cr2:
    mov EAX, CR2
    retn

regread_cr3:
    mov EAX, CR3
    retn

regread_cr4:
    mov EAX, CR4
    retn

regread_ds:
    mov EAX, 0
    mov AX, DS
    retn

regread_ss:
    mov EAX, 0
    mov AX, DS
    retn

regwrite_cr0:
    mov EAX, [ESP+4]
    mov CR0, EAX
    ret

regwrite_cr3:
    mov EAX, [ESP+4]
    mov CR3, EAX
    ret

regwrite_ds:
    mov EAX, [ESP+4]
    mov DS, AX
    ret