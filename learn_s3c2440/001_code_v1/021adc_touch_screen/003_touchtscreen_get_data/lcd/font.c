# include "font.h"
# include "../uart.h"
#if 1
/* 实现写字 */
static unsigned int fb_base;
static int xres, yres, bpp;
/* 获得LCD参数 */
void font_init(void)
{
	get_lcd_params(&xres, &yres, &bpp, &fb_base);
}

#endif


extern const unsigned char fontdata_8x16[];

void fb_print_char(int x, int y, char c, unsigned int color)
{
	int i, j, bit;
	unsigned const char * p = &fontdata_8x16[c * 16]; //根据ascii码在fontdata_8x16[]中获得点阵数据
	unsigned char data;
	
	for (i = y; i < y + 16; i++)
	{
		data = *p++;
		bit = 7;
		for (j = x; j < x + 8; j++)
		{
			if (data & (1 << bit))
				draw_dots(j, i, color);
			
			bit--;
		}
		
	}
}


void put_font(int x, int y, char * str, unsigned int color)
{
	unsigned int i = 0;
	
//	putchar(str[i]);
//	puts("\n\r");
	while (str[i])
	{
		if (str[i] == '\n')
			y += 16;
		else if (str[i] == '\r')
			x = 0;
		else 
		{
			fb_print_char(x, y, str[i], color);
			x += 8;
			if (x >= xres)  /* 换行 */
			{
				x = 0;
				y += 16;
			}
		}
		i++;
	}	
}



