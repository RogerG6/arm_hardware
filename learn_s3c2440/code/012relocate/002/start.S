/*设置时钟频率：FCLK : HCLK : PCLK = 400M : 100M : 50M*/
.text
.global _start

_start:	
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

	bl sdram_init
	bl main
	
halt:
	b halt





