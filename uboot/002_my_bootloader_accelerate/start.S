

.globl _start
_start:	
	/* 1. 关watchdog */
	ldr r0, =0x53000000  
	ldr r1, =0
	str r1,[r0]

	/* 2. 设置时钟 */
	/* 配置CLKDIVN使得：FCLK : HCLK : PCLK = 8 : 2 : 1 */
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

	/* 启动icache */
	bl enable_icache
	
	/* 3. 初始化SDRAM和串口       */
	ldr sp, =0x34000000
	bl sdram_init
	bl uart0_init

	/* 4. 重定位：将flash中的代码复制到SDRAM中 */
	bl nand_init

	adr r0, _start
	ldr r1, =0x33F80000
	ldr r2, =__bss_start
	sub r2, r2, r1
	bl copy2sdram

	/* 5、清bss段 */
	bl clear_bss

	/* 6. 跳到main函数执行 */
	ldr lr, halt
	ldr pc, =main
halt:
	b halt


enable_icache:
		/* 设置协处理器使能icache */
		mrc p15, 0, r0, c1, c0, 0
		orr r0, r0, #(1 << 12)   /* r0 = r0 | (1<<2) */
		mcr p15, 0, r0, c1, c0, 0
		mov pc, lr


