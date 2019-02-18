all : start.o  init.o timer.o uart.o exception.o main.o interrupt.o my_printf.o lib1funcs.o
	#arm-linux-ld -Ttext 0 -Tdata 0x800 start.o led_c.o  init.o uart.o main.o -o sdram.elf
	arm-linux-ld -T sdram.lds start.o init.o uart.o exception.o timer.o interrupt.o my_printf.o lib1funcs.o main.o -o  int.elf
	arm-linux-objcopy -O binary -S  int.elf  int.bin
	arm-linux-objdump -D  int.elf >  int.dis

%.o : %.c 
	arm-linux-gcc -c -o $@ $<

%.o : %.S
	arm-linux-gcc -c -o $@ $<
clean:
	rm *.bin *.o *.elf *.dis
	
