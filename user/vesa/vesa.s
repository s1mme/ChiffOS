global main
bits 	16

main:
;mov ax, 0xFE
;out 0x64, ax

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov di, 0x3600
    mov ax, 0x4F01
    mov cx, word [0x3600] ; video mode
    int 10h	

    hlt
    jmp $   
