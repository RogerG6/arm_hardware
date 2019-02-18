# include "../s3c2440_soc.h"

# include "lcd.h"
# include "lcd_controller.h"
# define HCLK 100

void jz2440_lcd_pin_init(void)
{
    /* 初始化引脚，背光引脚 */
    GPBCON &= ~(3 << 0);
    GPBCON |= (1 << 0);

    /* lcd专用引脚 */
    GPCCON = 0xaaaaaaaa;
    GPDCON = 0xaaaaaaaa;

    /* pwr enable */
    GPGCON |= (3 << 8);
}



/* 根据传入的LCD参数设置LCD控制器 */
void s3c2440_lcd_controller_init(p_lcd_params plcdparams)
{
    /* 引脚初始化 */
    jz2440_lcd_pin_init();

    /* [17:8]: CLKVAL    VCLK = HCLK / [(CLKVAL+1) x 2] ( CLKVAL >= 0 )
     *                      9 = 100M / [(CLKVAL + 1) * 2], CLKVAL = 4.5 = 5
     *                 CLKVAL = 100/vclk/2-1
     * [6:5] : 0xb11, tft lcd
     * [4:1] : bpp mode
     * [0]   : LCD video output and the logic enable/disable
     */
    //int clkval = (double)HCLK / plcdparams->time_seq.vclk / 2 - 1 + 0.5;    /* +0.5向上取整 */
    int bppmode = plcdparams->bpp == 8 ? 0xb : \
                  plcdparams->bpp == 16 ? 0xc : \
                  0xd;  /* 0xd: 24bpp and 32bpp */
    int clkval = 5;
    LCDCON1 =(clkval << 8) | (3 << 5) | (bppmode <<1);

    /* [31:24] : VBPD = tvb - 1
     * [23:14] : LINEVAL = lines - 1
     * [13:6]  : VFPD = tvf - 1
     * [5:0]   : VSPW = tvp - 1
     */
    LCDCON2 = ((plcdparams->time_seq.tvb - 1) << 24) | \
              ((plcdparams->yres - 1) << 14)         | \
              ((plcdparams->time_seq.tvf - 1) << 6) | \
              ((plcdparams->time_seq.tvp - 1) << 0);

    /* [25:19] : HBPD = thb - 1
     * [18:8]  : HOZVAL = column - 1
     * [7:0]   : HFPD = thf - 1
     */
     LCDCON3 = ((plcdparams->time_seq.thb - 1) << 19) | \
               ((plcdparams->xres - 1) << 8)         | \
               ((plcdparams->time_seq.thf - 1) << 0);

    /* [7:0] : HSPW = thp - 1 */
     LCDCON4 = ((plcdparams->time_seq.thp - 1) << 0);

    /* [16:15] : VSTATUS
     * [14:13] : HSTATUS
     * [12]    : BPP24BL
     * [11]    : FRM565 1-565
     * [10]    : INVVCLK    This bit controls the polarity of the VCLK active edge
     * [9]     : INVVLINE   This bit indicates the HSYNC pulse polarity
     * [8]     : INVVFRAME  This bit indicates the VSYNC pulse polarity
     * [7]     : INVVD      This bit indicates the VD (video data) pulse polarity
     * [6]     : INVVDEN
     * [5]     : INVPWREN
     * [4]     : INVLEND
     * [3]     : PWREN
     * [2]     : ENLEND
     * [1]     : BSWP
     * [0]     : HWSWP
     */
     int pixel_place = plcdparams->bpp == 8  ? 2 : \
                       plcdparams->bpp == 16 ? 1 : \
                       0;
     LCDCON5 = (1 << 11)                         |\
               (plcdparams->pins_pol.vclk << 10) |\
               (plcdparams->pins_pol.hsync << 9) |\
               (plcdparams->pins_pol.vsync << 8) |\
               (plcdparams->pins_pol.rgb << 7)   |\
               (plcdparams->pins_pol.de << 6)    |\
               (plcdparams->pins_pol.pwren << 5) |\
               (pixel_place);



     /* framebuffer */
     /*
      * [29:21] : LCDBANK   [30:22] of fb
      * [20:0]  : LCDBASEU  [20:1]  of fb
      */
     unsigned int addr;
     addr = (plcdparams->fb_base) & ~(1 << 31);
     LCDSADDR1 = (addr >> 1);

     /* [20:0] : LCDBASEL    [21:1] of fb_end */
     addr = plcdparams->fb_base + plcdparams->xres * plcdparams->yres * plcdparams->bpp / 8;
     addr >>= 1;
     addr &= 0x1fffff;
     LCDSADDR2 = addr;

}

void s3c2440_lcd_controller_enable(void)
{
    /* 背光控制电路：GPB0   */
    GPBDAT |= (1 << 0);

    /* LCDCON5  bit 3: lcd output enable */
    LCDCON5 |= (1 << 3);

    /* [0]   : LCD video output and the logic enable/disable */
    LCDCON1 |= (1 << 0);
}

void s3c2440_lcd_controller_disable(void)
{
    /* 背光控制电路：GPB0   */
    GPBDAT &= ~(1 << 0);

    /* LCDCON5  bit 3: lcd output enable */
    LCDCON5 &= ~(1 << 3);

    /* [0]   : LCD video output and the logic enable/disable */
    LCDCON1 &= ~(1 << 0);
}

lcd_controller s3c2440_lcd_controller = {
    .name = "s3c2440",
    .init = s3c2440_lcd_controller_init,
    .enable = s3c2440_lcd_controller_enable,
    .disable = s3c2440_lcd_controller_disable,
};


/* 将s3c2440_lcd_controller相关程序注册进数组 */
void s3c2440_lcd_controller_add(void)
{
    register_lcd_controller(&s3c2440_lcd_controller);
}





