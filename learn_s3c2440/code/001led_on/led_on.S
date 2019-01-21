
/*点亮led灯*/
.text
/*配置gpf4为输出引脚
  将0x100写入地址0x56000050*/
.global _start

_start:	ldr r1, =0x56000050
		mov r2, #0x100
		str r2, [r1]
		
/*设置GPF4使其输出高电平
  将0写入地址0x560000054*/
 	    ldr r1, =0x56000054
		mov r2, #0
		str r2, [r1]
/*死循环*/
halt:
	b halt
