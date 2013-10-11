global main
bits 	16

main:
;mov ax, 0xFE
;out 0x64, ax
.switch:
    xor ax, ax
    mov ds, ax
    mov ax, 0x4F02
    mov bx, 282 ; video mode

    int 10h
jmp exit
.getmode:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov di, 0x3600
    mov ax, 0x4F01
    mov cx, 282 ; video mode
    int 10h
jmp exit
.GetVGAInfo:
    xor ax, ax
    mov es, ax
    mov di, 0x3400
    mov ax, 0x4F00
    int 10h
jmp exit	

exit:
    hlt
    jmp $   
