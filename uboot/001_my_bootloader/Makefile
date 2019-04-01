
CC 		= arm-linux-gcc
LD 		= arm-linux-ld
AR 		= arm-linux-gar
OBJCOPY = arm-linux-objcopy
OBJDUMP = arm-linux-objdump

CFLAGS		:= -Wall -O2
CPPFLAGS	:= -nostdinc -fno-builtin

#export CC LD OBJDUMP OBJCOPY CFLAGS CPPFLAGS

objs := start.o init.o boot.o

boot.bin: $(objs)
	${LD} -Tboot.lds -o boot.elf $^
	${OBJCOPY} -O binary -S boot.elf $@
	${OBJDUMP} -D -m arm boot.elf > boot.dis
	
%.o:%.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<

%.o:%.S
	${CC} ${CPPFLAGS} ${CFLAGS} -c -o $@ $<

clean:
	rm -f *.o *.elf *.bin *.dis



	
