[BITS 32]

;imports
extern kmain
;extern gdtptr
;extern idtptr
;extern irq_handler

;exports
global idt_flush
global gdt_flush
global tss_flush

GRUB_FLAGS equ 10b
GRUB_MAGIC equ 0x1BADB002
GRUB_CHECK equ -(GRUB_FLAGS + GRUB_MAGIC)

align 4
	dd GRUB_MAGIC
	dd GRUB_FLAGS
	dd GRUB_CHECK

loader:
	mov esp, 0x140000
	push eax					;magic
	push ebx					;adress of Multiboot struct

	call kmain
	jmp $
	
;gdt_flush:
;	lgdt [gdtptr] 
;	mov ax, 0x10
;	mov ds, ax
;	mov es, ax
;	mov fs, ax
;	mov gs, ax
;	mov ss, ax
	
;	jmp 0x08:flush
;flush:
;	ret ; returns back to C-code

;idt_flush:
;	lidt [idtptr]
;	ret
	
;tss_flush:
;	mov ax, 0x2b
;	ltr ax
;	ret

irq_common_stub:
	push eax
    push ecx
    push edx
    push ebx
    
    push ebp
    push esi
    push edi

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	push esp
;	call irq_handler
	mov esp, eax

	pop gs
    pop fs
    pop es
    pop ds

    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    add esp, 8
	sti
    iret

.hang:
	jmp .hang
