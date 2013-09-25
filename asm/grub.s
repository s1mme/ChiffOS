;=======================================================  ;
;  kernel entry point!                                                    ;
;=======================================================  ; 
[BITS 32]
section .text
align 4
;imports
extern kmain

GRUB_FLAGS equ 10b
GRUB_MAGIC equ 0x1BADB002
GRUB_CHECK equ -(GRUB_FLAGS + GRUB_MAGIC)

align 4
	dd GRUB_MAGIC
	dd GRUB_FLAGS
	dd GRUB_CHECK

loader:
	mov esp, 0x190000
	push eax					;magic
	push ebx					;adress of Multiboot struct

	call kmain
	jmp $
