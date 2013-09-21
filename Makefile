WARNINGS := -Wall -ansi \
				 -Wextra -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes \
				 -Wmissing-declarations -Wnested-externs -Winline -Wno-long-long \
				 -Wstrict-prototypes -fno-builtin
				 
LDFLAGS =  -Tkernlink.ld
CFLAGS =   -c -std=gnu99 -march=i586 $(WARNINGS) -m32  -O0 -ggdb3 -ffreestanding  -fno-strict-aliasing -fno-common -I/home/kosmick/chiffOS/kernel/h/
CC = i586-pc-chiffos-gcc
LD = i586-pc-chiffos-ld
NASMFLAGS = -Ox -f elf
RM = rm

PROJDIRS := kernel/src kernel/h asm/
SRCFILES := $(shell find $(PROJDIRS) -type f -name '*.c')
HDRFILES := $(shell find $(PROJDIRS) -type f -name '*.h')
ASMFILES := $(shell find $(PROJDIRS) -type f -name '*.s')
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
OBJFILES += $(patsubst %.s,%.o,$(ASMFILES))

USERSPACEPROG := $(shell find user/ -maxdepth 2 -name 'Makefile' -exec dirname {} \;)

.s.o:
	nasm $(NASMFLAGS) $<
all: $(OBJFILES) link
					
link:
	$(LD) $(LDFLAGS) -o KERNEL $(OBJFILES)  
	@set -e; for prog in $(USERSPACEPROG); do \
		make -C $$prog; \
	done
	sudo mount /dev/sdd1 /mnt
	sudo cp KERNEL /mnt/
	#sudo cp initrd.img /mnt/boot
	sudo umount /mnt
	sudo qemu-system-i386 -kernel KERNEL  -m 2000 -vga std 
	#-initrd initrd.img later
clean:
	-$(RM) $(wildcard $(OBJFILES))
	-$(RM) KERNEL
	@for prog in $(USERSPACEPROG); do \
		make -C $$prog clean; \
		rm -f misc/`basename $$prog` ; \
	done

