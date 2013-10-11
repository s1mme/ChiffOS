; start.asm

[BITS 16]
extern main
;extern _exit

mov esp, 0x190000

   
   call main
      
  ; call _exit
    jmp  $
