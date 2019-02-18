
static volatile int g_ts_x, g_ts_y;
static volatile int g_lcd_x, g_lcd_y;


void touchscreen_test(void)
{
	int pressure;
	touchscreen_init();

	delay(100000);
	clear_ts(0);
	put_font(70, 70, "Touch the cross to calibrate!", 0xff);

	ts_calibrate();
	put_font(70, 156, "Calibrate done! Draw!", 0xff);

	while(1)
	{
		if (ts_read(&g_lcd_x, &g_lcd_y, &pressure) == 0)
		{
			printf("lcd data: x = %d, y = %d\n\r", g_lcd_x, g_lcd_y);

			if (pressure)
				draw_dots(g_lcd_x, g_lcd_y, 0xff00);
		}
	}
	
}
