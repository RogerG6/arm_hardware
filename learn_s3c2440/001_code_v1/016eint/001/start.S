/*设置时钟频率：FCLK : HCLK : PCLK = 400M : 100M : 50M*/
.text
.global _start

_start:	
		b reset							/*  vector 0: reset*/
		ldr pc, und_addr				/*  vector 4: und*/
		ldr pc, swi_addr				/*  vector 8: swi*/
		b halt							/*  vector c: abort prefetch*/
		b halt							/*  vector 10: abort data*/
		b halt							/*  vector 14: Reserved*/
		ldr pc, irq_addr				/*  vector 18: IRQ*/
		b halt							/*  vector 1c: FIQ*/

und_addr:
	.word do_und
swi_addr:
	.word do_swi
irq_addr:
	.word do_irq

reset:
	ldr r0, =0x53000000  /* 关闭看门狗 */
	ldr r1, =0
	str r1,[r0]

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

	/* 设置cpu的cpsr, 使能中断 */
	mrs r0, cpsr              
	bic r0, r0, #0xf      /* 修改M4-M0为0b10000, 进入usr模式 */
	bic r0, r0, #(1<<7)   /* 清除I位，使能中断 */
	msr cpsr, r0

	/* 设置sp_usr */
	ldr sp, =0x33f00000	


	ldr pc, =sdram      /* 重定位后立马跳到sdram执行，以防以下代码在4K外 */
sdram:	
		bl uart_init
		bl interrupt_ctrl_init
		bl key_int
		bl led_init
	
	/* 故意在此加入一条未定义指令 */
bad_code:
	.word 0xeeadc0de   			/* undefined instruction */
	
	swi 0x123			/* 执行此命令，触发SWI异常，进入0x8执行 */
		
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
		ldr sp, =0x34000000       /* 设置sp_svc */
		stmdb	sp!, {r0-r12, lr}
		
		
		/* 调用und异常处理函数 */
		sub r0, lr, #4
		mrs r1, cpsr
		bl und_func
		
		/* 恢复现场 */
		ldmia	sp!, {r0-r12, pc}^ 			/* ^会把spsr的值恢复到cpsr中 */

do_swi:
		/* 执行至这里之前：
		 * 1. lr_swi中保存被中断模式中下一条指令的地址
		 * 2. spsr_swi中保存被中断模式中cpsr的值
		 * 3. cpsr中的[4:0]被设置为10011，进入Supervisor mode
		 * 4. 跑到0x8的地址执行
		 */
		
		/* 保存现场 */
		ldr sp, =0x33e00000
		stmdb	sp!, {r0-r12, lr}
		
		mov r4, lr            /* 此处必须用r4-r11保存lr的值，因为在调用C的过程中会改变lr的值，详见ATPCS */
		
		/* 调用swi异常处理函数 */
		sub r0, r4, #4
		mrs r1, cpsr
		bl swi_func
		
		sub r0, r4, #4
		ldr r0, [r0]
		bl print_swi_val       /* 打印swi的异常值 */
		
		/* 恢复现场 */
		ldmia	sp!, {r0-r12, pc}^ 			/* ^会把spsr的值恢复到cpsr中 */

do_irq:
		/* 执行至这里之前：
		 * 1. lr_irq中保存被中断模式中下一条指令的地址
		 * 2. spsr_irq中保存被中断模式中cpsr的值
		 * 3. cpsr中的[4:0]被设置为10010，进入irq mode
		 * 4. 跑到0x18的地址执行
		 */
		
		/* 保存现场 */
		/* lr-4 是程序返回地址 */
		ldr sp, =0x33d00000
		sub lr, lr, #4
		stmdb	sp!, {r0-r12, lr}
	
		/* 调用swi异常处理函数 */
		bl handle_irq
				
		/* 恢复现场 */
		ldmia	sp!, {r0-r12, pc}^ 			/* ^会把spsr的值恢复到cpsr中 */




