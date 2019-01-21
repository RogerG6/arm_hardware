
# include "lcd.h"

void lcd_test(void)
{
    int xres, yres, bpp;
    unsigned int fb;
    int x, y;

    /* 初始化lcd */
    lcd_init();

    /* enable lcd */
    lcd_enable();

    /* 获取lcd参数 */
    get_lcd_params(&xres, &yres, &bpp, &fb);

    unsigned short * p;
    unsigned int * p2;

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
    }
}




