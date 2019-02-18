all : start.o init.o interrupt.o nand_flash.o led_c.o uart.o main.o dma.o exception.o timer_init.o nor_flash.o my_printf.o lib1funcs.o string_utils.o
	#arm-linux-ld -Ttext 0 -Tdata 0x800 start.o led_c.o  init.o uart.o main.o -o sdram.elf
	arm-linux-ld -T sdram.lds $^ -o exception.elf
	arm-linux-objcopy -O binary -S exception.elf exception.bin
	arm-linux-objdump -D exception.elf > exception.dis

%.o : %.c 
	arm-linux-gcc -march=armv4 -c -o $@ $<

%.o : %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
clean:
	rm *.bin *.o *.elf *.dis
	
