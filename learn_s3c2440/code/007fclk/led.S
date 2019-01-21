/*设置时钟频率：FCLK : HCLK : PCLK = 800M : 400M : 100M*/
.text
.global _start

_start:	
	/* LOCKTIME(0x4C000000) = 0xFFFFFFFF */
	ldr r0, =0x4C000000
	ldr r1, =0xFFFFFFFF
	str r1, [r0]
	
	/* 设置FCLK : HCLK : PCLK = 8 : 4 : 1
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
	
	mov sp,#4096   /* 设置栈顶 */

	bl main
	
halt:
	b halt
	