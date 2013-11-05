global main
bits 	16

main:
    xor ax, ax
    mov es, ax
    mov di, 0x3400
    mov ax, 0x4F00
    int 10h
 
exit:
    hlt
    jmp $   
