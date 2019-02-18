# include "s3c2440_soc.h"

void Isr_dma(int irq)
{
	puts("Tc = 0, transition finished!\n\r");
}



void dma_init(void)
{
	//GPBCON |= (2<<20) | (2<<18);
	
	unsigned int src;
	char * str = "hello world!!!\n\r";
	src = (unsigned int)str;

	/* 将源数据地址设置为内存地址 */
	DISRC0 = src;

	
	/* src AHB，源数据地址自增方式设为increment */
	DISRCC0 = 0;

	/* 将目标数据地址设置成uart0 receive buffer */
	DIDST0 = (volatile unsigned int)(0x50000020);

	/* 中断发生时间: TC=0
	 * 目的地：APB
	 * 地址自增：fixed
	 */
	DIDSTC0 = 0x3;

	/* [31]: handshake mode
	 * [30]: AHB clock
	 * [29]: 当TC=0时，发生中断
	 * [28]: transfer size:unit
	 * [27]: single service
	 * [26:24]: DMA0, uart0
	 * [23]: source是hardware
	 * [22]: no auto-reload
	 * [21:20]: data size: 00-BYTE
	 * [19:0]: Initial transfer count: 16unit
	 */
	DCON0 = (1<<31) | (0<<30) | (1<<29) | (0<<27) | (1<<24) | (1<<23) | (1<<22) | (16<<0);


	register_irq(Isr_dma, 17);
}


void dma_memory_2_uart(void)
{
	/* DMA 0 mask trigger register, 启动dma
	 * STOP   :[2]	if TC=0 || 当前传输完成，then stop dma transfer
	 * ON_OFF :[1]	turn on/off dma
	 * SW_TRIG:[0]	Trigger the DMA channel in S/W request mode.
	 */
	DMASKTRIG0 = 0x2;
#if 0
	printf("current statue: %d\n\r", (DSTAT0>>20) & 0x3);
	printf("current TC    : 0x%08x\n\r", DSTAT0 & (0xfffff));
	printf("STOP bit      : %d\n\r", (DMASKTRIG0>>2) & 1);
	printf("ON/OFF bit    : %d\n\r", (DMASKTRIG0>>1) & 1);
	//DMASKTRIG0 |= (1 << 2);
#endif
}

void dma_test(void)
{
	
	dma_memory_2_uart();
	
}




