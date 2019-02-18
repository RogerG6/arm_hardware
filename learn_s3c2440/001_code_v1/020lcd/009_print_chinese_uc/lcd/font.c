# include "font.h"
# include "../uart.h"
# include "/work/tools/gcc-3.4.5-glibc-2.3.6/distributed/arm-linux/include/stdio.h"
//# include "../my_printf.h"

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


/* 打印8*16英文字符 */
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
	
	while (str[i])
	{
	
		if ((x + 8) >= xres)	/* 换行 */
		{
			x = 0;
			y += 16;
		}
		if (str[i] == '\n')
			y += 16;
		else if (str[i] == '\r')
			x = 0;
		else 
		{
			fb_print_char(x, y, str[i], color);
			x += 8;
		}
		i++;
	}	
}

void put_2_char(int x, int y, char ch, char cl, unsigned int color, FILE * fphzk)
{
	int offset, i, j, k = 0;
	unsigned char buffer[32];
	
	offset = (94 * (unsigned int)(ch - 0xa0 - 1)+(cl - 0xa0 - 1)) * 32;
	fseek(fphzk, offset, SEEK_SET);
	fread(buffer, 1, 32, fphzk);
	
	for (i = 0; i < 16; i++)
	{
		if ((x + 16) >= xres)
		{
			x = 0;
			y += 16;
		}
		for (j = 0; j < 2; j++)
		{
			fb_print_char(x, y, buffer[k], color);
			k++;
			x += 8;
		}
		
	}
	
}

int put_chinese(int x, int y, char * str, unsigned int color)
{
		FILE* fphzk = NULL;
	
	   int i = 0, k, j;
	
	   fphzk = fopen("HZK16C", "rb");
	   if(fphzk == NULL)
	   {
		   printf("error hzk16\n");
		   return 1;
	   }
	   while (str[i])
	   {
	   	   put_2_char(x, y, str[i], str[i+1], color, fphzk);
		   i += 2;
	   }
	   fclose(fphzk);
	   fphzk = NULL;
	   return 0;
}

