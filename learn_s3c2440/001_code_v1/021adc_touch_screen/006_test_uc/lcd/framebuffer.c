# include "lcd.h"

/* 实现画点 */

static unsigned int fb_base;
static int xres, yres, bpp;

/* 获得LCD参数 */
void fb_get_lcd_params(void)
{
	get_lcd_params(&xres, &yres, &bpp, &fb_base);
}

unsigned short Convert32bppto16bpp(unsigned int color)
{
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = color & 0xff;

	/* 565 */
	r >>= 3;
	g >>= 2;
	b >>= 3;

	return ((r << 11) | (g << 5) | b);
}


void draw_dots(int x, int y, unsigned int color)
{
	unsigned char * pc;  /* 8bpp */
	unsigned short * pw;  /* 16bpp */
	unsigned int * pdw;  /* 32bpp */

	unsigned int pixel_base = fb_base + (xres * y + x) * bpp / 8;
	
	switch (bpp)
	{
		case 8:
			pc = (unsigned char *)pixel_base;
			*pc = color;
			break;
		case 16:
			pw = (unsigned short *)pixel_base;
			*pw = Convert32bppto16bpp(color);
			break;
		case 32:
			pdw = (unsigned int *)pixel_base;
			*pdw = color;
			break;
	}
}

/* 全屏显示黑色 */
void clear_ts(unsigned int color)
{
    unsigned short * p;
    unsigned int * p2;
    int x, y;
    
    /* 获取lcd参数 */
    get_lcd_params(&xres, &yres, &bpp, &fb_base);

    if (bpp == 16)
    {
        p = (unsigned short *)fb_base;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = color;
    }
    else if (bpp == 32)
    {
	   p2 = (unsigned int *)fb_base;
	   for (x = 0; x < xres; x++)
		   for (y = 0; y < yres; y++)
			   *p2++ = color;
    }
    else if (bpp == 8)
    {
		 /* black */
		p2 = (unsigned int *)fb_base;
		for (x = 0; x < xres; x++)
			for (y = 0; y < yres; y++)
				*p2++ = color;
    }

}


