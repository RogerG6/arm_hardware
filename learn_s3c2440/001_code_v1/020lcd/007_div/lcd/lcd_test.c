
# include "lcd.h"
# include "font.h"
# include "geometry.h"
# include "../uart.h"




void lcd_test(void)
{
	static unsigned int fb;
	static int xres, yres, bpp;

    int x, y;

    /* 初始化lcd */
    lcd_init();

    /* enable lcd */
    lcd_enable();

    /* 获取lcd参数 */
    get_lcd_params(&xres, &yres, &bpp, &fb);
	fb_get_lcd_params();  /* 获得draw_dots的参数 */
	font_init();
	
    unsigned short * p;
    unsigned int * p2;
//======================================================
//轮流显示全屏为red/green/blue
	
    if (bpp == 16)
    {
        /* 全屏显示红色 */
        p = (unsigned short *)fb;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = 0xf800;

        /* 全屏显示green */
        p = (unsigned short *)fb;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = 0x7e0;

        /* 全屏显示blue */
        p = (unsigned short *)fb;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p++ = 0x1f;
    }
    else if (bpp == 32)
    {
        /* 全屏显示红色 */
        p2 = (unsigned int *)fb;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p2++ = 0xff0000;

        /* 全屏显示green */
        p2 = (unsigned int *)fb;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
                *p2++ = 0xff00;

        /* 全屏显示blue */
        p2 = (unsigned int *)fb;
        for (x = 0; x < xres; x++)
            for (y = 0; y < yres; y++)
				*p2++ = 0xff;

		/* black */
	   p2 = (unsigned int *)fb;
	   for (x = 0; x < xres; x++)
		   for (y = 0; y < yres; y++)
			   *p2++ = 0;
   }
   
//=====================================================
//在屏幕中显示线、圆

		draw_line(0, 0, xres - 1, 0, 0xff0000);
		draw_line( xres - 1, 0, xres - 1, yres - 1, 0x1234500);
		draw_line(xres - 1, yres - 1, 0, yres - 1, 0xff00);
		draw_line(0, 0, yres - 1, 0, 0xfffff);
		draw_line(0, 0, xres - 1, yres - 1, 0xfffffff);
		draw_line(0, yres - 1, xres - 1, 0, 0xfffffff);	


		draw_circle(xres / 2, yres / 2, yres / 4, 0x12345678);

//=====================================================
//在屏幕中写字
		
		putchar('A');
		puts("\n\r");
    	put_font(10, 10, "www.baidu.com", 0xfffffff);
    	
		putchar('B');
		puts("\n\r");
}




