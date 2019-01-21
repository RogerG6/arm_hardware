
.text
.global _start

_start:	
	ldr r0, =0x53000000
	ldr r1, =0
	str r1,[r0]
	
	ldr r0, =0
	ldr r1, [r0]  /* 将原0地址的数据读出备份*/
	str r0, [r0]  /* 0->[0]*/
	ldr r2, [r0]  /* r2 = [r0]*/
	cmp r0, r2    /* 比较r0,r2, 如果相等，则为nand启动*/
	ldr sp, =0x40000000 + 4096
	moveq sp,#4096    /* nand启动*/
	streq r1, [r0]    /* 恢复0地址原来的值*/

	bl main
	
halt:
	b halt




