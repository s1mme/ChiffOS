global main
bits 	16

main:
;mov ax, 0xFE
;out 0x64, ax

    xor ax, ax
    mov ds, ax
    mov ax, 0x4F02
    mov bx, word [0x3600] ; video mode  
int 10h
	

exit:
    hlt
    jmp $   
