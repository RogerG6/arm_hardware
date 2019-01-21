/*设置时钟频率：FCLK : HCLK : PCLK = 400M : 100M : 50M*/
.text
.global _start

_start:	
		b reset					/* reset vector */
		ldr pc, und_addr		/* und vector */
		
und_addr:
	.word do_und
reset:
	/* LOCKTIME(0x4C000000) = 0xFFFFFFFF */
	ldr r0, =0x4C000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]
	
	/* 设置FCLK : HCLK : PCLK = 8 : 2 : 1
	 * 配置CLKDIVN*/
	ldr r0, =0x4C000014
	ldr r1, =0x5
	str r1, [r0]
	
	/* 设置CPU工作于异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xc0000000   //R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0
	
	/* 配置MPLLCON，使其输出频率为400M */
	ldr r0, =0x4c000004
	ldr r1, =(92 << 12) | (1 << 4) | (1 << 0)
	str r1, [r0]
	
	ldr r0, =0x53000000  /* 关闭看门狗 */
	ldr r1, =0
	str r1,[r0]
	
	/* 设置内存: sp 栈 */
	/* 分辨是nor/nand启动
	 * 写0到0地址, 再读出来
	 * 如果得到0, 表示0地址上的内容被修改了, 它对应ram, 这就是nand启动
	 * 否则就是nor启动
	 */
	mov r1, #0
	ldr r0, [r1] /* 读出原来的值备份 */
	str r1, [r1] /* 0->[0] */ 
	ldr r2, [r1] /* r2=[0] */
	cmp r1, r2   /* r1==r2? 如果相等表示是NAND启动 */
	ldr sp, =0x40000000+4096 /* 先假设是nor启动 */
	moveq sp, #4096  /* nand启动 */
	streq r0, [r1]   /* 恢复原来的值 */

	bl sdram_init   /* 重定位前，需使用相对地址跳转 */
	
	
	/* 重定位text, rodata, data段*/
	bl relocate
		
	/* 清除bss段*/
	bl clear_bss
	
	ldr pc, =sdram      /* 重定位后立马跳到sdram执行，以防以下代码在4K外 */
sdram:	
	bl uart_init
	

	/* 故意在此加入一条未定义指令 */
bad_code:
	.word 0xeeadc0de   			/* undefined instruction */
	
	
		
	ldr pc, =main   /* 重定位后，需使用绝对地址跳转 */

halt:
	b halt

do_und: /* 执行至这里之前：
		 * 1. lr_und中保存被中断模式中下一条指令的地址
		 * 2. spsr_und中保存被中断模式中cpsr的值
		 * 3. cpsr中的[4:0]被设置为11011，进入und mode
		 * 4. 跑到0x4的地址执行
		 */
		
		/* 保存现场 */
		ldr sp, =0x33d00000
		stmdb	sp!, {r0-r12, lr}
		
		
		/* 调用异常处理函数 */
		sub r0, lr, #4
		ldr r1, [r0]
		bl und_func
		
		
		/* 恢复现场 */
		ldmia	sp!, {r0-r12, pc}^ 			/* ^会把spsr的值恢复到cpsr中 */






