objs = start.o init.o nand_flash.o interrupt.o led_c.o uart.o main.o exception.o timer_init.o nor_flash.o my_printf.o lib1funcs.o string_utils.o

objs += lcd/font_8x16.o
objs += lcd/font.o
objs += lcd/framebuffer.o
objs += lcd/geometry.o
objs += lcd/lcd.o
objs += lcd/lcd_4.3.o
objs += lcd/lcd_controller.o
objs += lcd/lcd_test.o
objs += lcd/s3c2440_lcd_controller.o

objs += adc/adc.o
objs += adc/adc_test.o

objs += touchscreen/touchscreen.o
objs += touchscreen/test_ts.o


all : $(objs)
	#arm-linux-ld -Ttext 0 -Tdata 0x800 start.o led_c.o  init.o uart.o main.o -o sdram.elf
	arm-linux-ld -T sdram.lds $^ libgcc.a  -o lcd.elf
	arm-linux-objcopy -O binary -S lcd.elf lcd.bin
	arm-linux-objdump -D lcd.elf > lcd.dis

%.o : %.c
	arm-linux-gcc -march=armv4 -c -o $@ $<

%.o : %.S
	arm-linux-gcc -march=armv4 -c -o $@ $<
clean:
	rm *.bin *.o *.elf *.dis

