#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/clk.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include <asm/mach/map.h>
#include <asm/arch/regs-lcd.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/fb.h>

struct lcd_regs {
	unsigned long	lcdcon1;
	unsigned long	lcdcon2;
	unsigned long	lcdcon3;
	unsigned long	lcdcon4;
	unsigned long	lcdcon5;
    unsigned long	lcdsaddr1;
    unsigned long	lcdsaddr2;
    unsigned long	lcdsaddr3;
    unsigned long	redlut;
    unsigned long	greenlut;
    unsigned long	bluelut;
    unsigned long	reserved[9];
    unsigned long	dithmode;
    unsigned long	tpal;
    unsigned long	lcdintpnd;
    unsigned long	lcdsrcpnd;
    unsigned long	lcdintmsk;
    unsigned long	lpcsel;
};


static struct fb_info * info;
static volatile unsigned long *gpbcon;
static volatile unsigned long *gpbdat;
static volatile unsigned long *gpccon;
static volatile unsigned long *gpdcon;
static volatile unsigned long *gpgcon;
static volatile struct lcd_regs *s3c_lcd_regs;
static u32 pseudo_palette[16];


/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int s3c_lcdfb_setcolreg(unsigned int regno, unsigned int red,
			     unsigned int green, unsigned int blue,
			     unsigned int transp, struct fb_info *info)
{
	unsigned int val;
	
	if (regno > 16)
		return 1;

	/* 用red,green,blue三原色构造出val */
	val  = chan_to_field(red,	&info->var.red);
	val |= chan_to_field(green, &info->var.green);
	val |= chan_to_field(blue,	&info->var.blue);
	
	//((u32 *)(info->pseudo_palette))[regno] = val;
	pseudo_palette[regno] = val;
	return 0;
}



static struct fb_ops s3c_fb_ops = {
	.owner		= THIS_MODULE,
	.fb_setcolreg	= s3c_lcdfb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};

static int lcd_init(void)
{
	/* 1. 分配fb_info */
	info = framebuffer_alloc(0, NULL);

	/* 2. 设置fb_info */
	/* 2.1 设置fix para */
	strcpy(info->fix.id, "s3c_lcd");
	info->fix.smem_len = 480*272*2;
	info->fix.type = FB_TYPE_PACKED_PIXELS;
	info->fix.visual = FB_VISUAL_TRUECOLOR;  /* 真彩 */
	info->fix.line_length = 480*2;	/* 一行有多少BYTE */
	
	/* 2.2 设置var para */
	info->var.xres           = 480;
	info->var.yres 		     = 272;
	info->var.xres_virtual   = 480;
	info->var.yres_virtual   = 272;
	info->var.bits_per_pixel =16;

	/* RGB */	
	info->var.red.offset = 11;
	info->var.red.length = 5;
	info->var.green.offset = 5;
	info->var.green.length = 6;
	info->var.blue.offset = 0;
	info->var.blue.length = 5;

	info->var.activate = FB_ACTIVATE_NOW;

	/* 2.3 设置操作函数 */
	info->fbops = &s3c_fb_ops;

	/* 2.4 其他的设置 */
	info->pseudo_palette = pseudo_palette;
	//s3c_lcd->screen_base  = ;  /* 显存的虚拟地址 */ 
	info->screen_size   = 480*272*16/8;

		
	/* 3. 硬件操作 */
	/* 3.1 配置相关引脚为lcd */
	gpbcon = ioremap(0x56000010, 8);
	gpbdat = gpbcon + 1;
	gpccon = ioremap(0x56000020, 4);
	gpdcon = ioremap(0x56000030, 4);
	gpgcon = ioremap(0x56000060, 4);

	/* VD引脚 */
	*gpccon = 0xaaaaaaaa;
	*gpdcon = 0xaaaaaaaa;

	/* gpb0配置成输出引脚，输出低电平，背光 */
	*gpbcon &= ~(3);
	*gpbcon |= 1;
	*gpbdat &= ~1;

	/* PWREN */
	*gpgcon |= (3<<8);

	/* 3.2 配置LCD 控制器 */
	s3c_lcd_regs = ioremap(0x4d000000, sizeof(struct lcd_regs));

	/* CLKVAL [17:8] : VCLK = HCLK / [(CLKVAL+1) x 2]
	 *				   10MHz = 100MHz / [(CLKVAL+1) x 2]
	 *				   CLKVAL = 4
	 * PNRMODE[6:5] : 0b11 TFT
	 * BPPMODE[4:1] : 0xc  16bpp
	 * bit[0]  : 0 = Disable the video output and the LCD control signal.
	 */
	s3c_lcd_regs->lcdcon1 = (4<<8) | (3<<5) | (0xc<<1);

	/* VBPD [31:24] : VBPD+1=T0-T1-T2=4
	 *				  VBPD = 3
	 * LINEVAL [23:14] : 320=LINEVAL+1
	 *					 LINEVAL=319
	 * VFPD [13:6] : VFPD+1=T2-T5=2
	 *				 VFPD=1
	 * VSPW[5:0] : VSPW+1=1
	 *			   VSPW=0
	 */
	s3c_lcd_regs->lcdcon2 = (1<<24) | (271<<14) | (1<<6) | (9);


	/* HBPD[25:19] : HBPD+1=T6-T7-T8=17
	 * HOZVAL[18:8] : HOZVAL+1=240
	 * HFPD[7:0] : HFPD+1=T8-T11=11
	 */
	s3c_lcd_regs->lcdcon3 = (1<<19) | (479<<8) | (1<<0);

	/* HSPW[7:0] : HSPW+1=T7=5 */
	s3c_lcd_regs->lcdcon4 = 40;

	/* 信号极性
	 * FRM565[11] : 1 = 5:6:5 Format
	 * INVVCLK[10] : 0 = The video data is fetched at VCLK falling edge
	 * INVVLINE[9] : 1 = Inverted
	 * INVVFRAME[8] : 1 = Inverted
	 * INVVD[7] : 0 = Normal
	 * INVVDEN[6] : 0 = Normal
	 * PWREN[3] : 0 = Disable PWREN signal
	 * BSWP[1] : 0 = Swap Disable 
	 * HWSWP[0] : 1 = Swap Enable 像素在fb中高低位排列方式
	 */
	s3c_lcd_regs->lcdcon5 = (1<<11) | (3<<8) | (1<<0);

	/* 3.3 分配fb并将地址告诉lcd         controller */
	info->screen_base = dma_alloc_writecombine(NULL, info->fix.smem_len, &info->fix.smem_start, GFP_KERNEL);

	s3c_lcd_regs->lcdsaddr1 = (info->fix.smem_start>>1) & ~(3<<30);
	s3c_lcd_regs->lcdsaddr2 = ((info->fix.smem_start + info->fix.smem_len)>>1) & 0x1fffff;
	s3c_lcd_regs->lcdsaddr3 = 480; 	/* 一行有多少半字 */

	s3c_lcd_regs->lcdcon1 |= 1;			/* 启动lcd控制器 */
	s3c_lcd_regs->lcdcon5 |= (1<<3);	/* 启动lcd本身 */
	*gpbdat |= 1;	/* 输出高电平，使能背光 */

	/* 4. 注册 */
	register_framebuffer(info);

	return 0;
}

static void lcd_exit(void)
{
	unregister_framebuffer(info);
	s3c_lcd_regs->lcdcon1 &= ~(1<<0); /* 关闭LCD控制器 */
	*gpbdat &= ~1;     /* 关闭背光 */
	dma_free_writecombine(NULL, info->fix.smem_len, info->screen_base, info->fix.smem_start);
	iounmap(s3c_lcd_regs);
	iounmap(gpbcon);
	iounmap(gpccon);
	iounmap(gpdcon);		
	iounmap(gpgcon);		
	framebuffer_release(info);
}

module_init(lcd_init);
module_exit(lcd_exit);
MODULE_LICENSE("GPL");


