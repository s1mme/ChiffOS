WARNINGS := 

CC = i586-pc-chiffos-gcc
CFLAGS := -O0 -std=gnu99 -march=i586 $(WARNINGS) -ggdb3 -static -I/home/kosmick/chiffOS/kernel/h/ -I/home/kosmick/chiffOS_newlib/newlib-build/i586-pc-chiffos/include/
LD = i586-pc-chiffos-ld
LDFLAGS := -T../user.ld 

NASMFLAGS = -Ox -f elf
PROJDIRS := 
SRCFILES := $(shell find . -type f -name '*.c')

LIBS = libc.a
SourceDir= /home/kosmick/chiffOS/libc/
LIBS_collection=$(addprefix $(SourceDir),$(LIBS))


OBJFILES += $(patsubst %.c,%.o,$(SRCFILES))
ASMFILES := $(shell find $(PROJDIRS) -type f -name '*.s')
DEPFILES := $(patsubst %.c,%.d,$(SRCFILES))
OBJFILES += $(patsubst %.s,%.o,$(ASMFILES))

OUTNAME := $(shell basename `pwd`)
.s.o:
	nasm $(NASMFLAGS) $<
all: $(OBJFILES) link

link:
	@$(LD) $(LDFLAGS) -o $(OUTNAME) $(OBJFILES) $(LIBS_collection)
	if ! (mount | grep -q /mnt/fat32); then \
   	mount /dev/loop6 /mnt/fat32; \
	fi
	
	
	sleep 0.1
	mv -f $(OUTNAME) /mnt/fat32/

#../../misc/
clean:
	-$(RM) $(wildcard $(OBJFILES) $(DEPFILES) ../misc/$(OUTNAME))

	-include $(DEPFILES)

%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

