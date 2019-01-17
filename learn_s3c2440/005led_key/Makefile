all:
	arm-linux-gcc -c -o led_c.o led_c.c
	arm-linux-gcc -c -o led.o led.S
	arm-linux-ld -Ttext 0 led.o led_c.o -o led.elf
	arm-linux-objcopy -O binary -S led.elf led.bin
	arm-linux-objdump -D led.elf > led.dis

clean:
	rm *.o *.bin *.elf *.dis
