WARNINGS := 

CC = i586-pc-chiffos-gcc
CFLAGS := -O0 -std=gnu99 -march=i586 $(WARNINGS) -ggdb3 -static -I/home/kosmick/chiffOS/kernel/h/
LD = i586-pc-chiffos-ld
LDFLAGS := -T../user.ld

SRCFILES := $(shell find . -type f -name '*.c')
OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
DEPFILES := $(patsubst %.c,%.d,$(SRCFILES))

OUTNAME := $(shell basename `pwd`)

all: $(OBJFILES)

	@$(LD) $(LDFLAGS) -o $(OUTNAME) $(OBJFILES)
	#umount /mnt/fat
	
	#mount /dev/loop0  /mnt/fat
	sleep 0.1
	#mv -f $(OUTNAME) /mnt/fat/
	
	
#../../misc/
clean:
	-$(RM) $(wildcard $(OBJFILES) $(DEPFILES) ../misc/$(OUTNAME))

	-include $(DEPFILES)

%.o: %.c Makefile
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

