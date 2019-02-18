#ifndef _LCD_H
#define _LCD_H

enum {
    NORMAL = 0,
    INVERT = 1,
};

/* NORMAL = 0: 正常极性
 * INVERT = 1：反转极性
 */
typedef struct pins_polarity {
    int de;     /* normal: 高电平时，能够传输数据 */
    int pwren;  /* normal: 高电平时，给lcd提供AVDD */
    int vclk;   /* normal: 在下降沿获取数据 */
    int rgb;    /* normal: 高电平表示1 */
    int hsync;  /* normal: 高脉冲 */
    int vsync;  /* normal: 高脉冲 */
}pins_polarity, * p_pins_polarity;


typedef struct time_sequence {
    /* 垂直方向 */
    int tvp;    /* vsync脉冲宽度 Vertical Pulse width */
    int tvb;    /* 上边黑框 Vertical Back porch */
    int tvf;    /* 下边黑框 Vertical Front porch */

    /* 水平方向 */
    int thp;    /* hsync脉冲宽度 Horizontal Pulse width */
    int thb;    /* 左边黑框 Horizontal Back porch */
    int thf;    /* 右边黑框 Horizontal Front porch */
    int vclk;
}time_sequence, * p_time_sequence;


typedef struct lcd_params {
    char * name,

    /* 引脚极性 */
    pins_polarity pins_pol;

    /* 时序 */
    time_sequence time_seq;

    /* 分辨率， bpp */
    int xres;   /* 列数 */
    int yres;   /* 行数 */
    int bpp;

    /* framebuffer地址 */
    unsigned int fb_base;
}lcd_params, * p_lcd_params;


#endif   /* _LCD_H */

