# include "../s3c2440_soc.h"


/*
---------------------------------|
|								 |
|	 a					 b		 |
|	-|-					-|-      |
|			   e				 |
|		      -|-				 |
|	 d					 c		 |
|	-|-					-|-      |
|								 |
|								 |
|--------------------------------|
*/
/* 五个点在ts上的坐标 */
static volatile double g_kx;
static volatile double g_ky;
static int g_ts_xy_swap;
static volatile int g_ts_xc, g_ts_yc;
//static volatile int g_lcd_x, g_lcd_y;


static unsigned int fb_base;
static int xres, yres, bpp;


int get_lcd_x_frm_ts(int x)
{
	return (x - g_ts_xc) * g_kx + xres / 2;
}

int get_lcd_y_frm_ts(int y)
{
	return (y - g_ts_yc) * g_ky + yres / 2;
}

unsigned int Abs(int a, int b)
{
	return (a > b ? (a - b) : (b - a));
}

/* 获得触摸屏坐标的原始数据 */
void get_ts_xy(int * px, int * py)
{
	int sum_x = 0, sum_y = 0, sum_cnt = 0;
	int x = 0, y = 0;
	
	int pressure;
	
	do
	{
		ts_read_raw(&x, &y, &pressure);
	}while (pressure == 0);

		*px = x;
		*py = y;

	do 
	{
		sum_x += *px;
		sum_y += *py;

		sum_cnt++;
		if (sum_cnt == 128)
			break;

		ts_read_raw(&x, &y, &pressure);
		if (Abs(x, *px) < 5 && Abs(y, *py) < 5)  /* 如果两点间距>5，则丢弃 */
		{
			*px = x;
			*py = y;
		}
		
		printf("ts_x = %d, ts_y = %d, sum_cnt = %d\n\r", x, y, sum_cnt);

	}while (pressure);

	*px = sum_x / sum_cnt;
	*py = sum_y / sum_cnt;
	printf("return : ts_x = %d, ts_y = %d\n\r", *px, *py);
}


/* 判断y方向一样  ,     x方向变化的2个点xy是否对换 */
int Is_xy_swap(int x1, int y1, int x2, int y2)
{
	int dx;
	int dy;
	dx = x2 - x1;
	dy = y2 - y1;
	if (dx < 0)
		dx = -dx;
	if (dy < 0)
		dy = -dy;
	if (dx > dy)
		return 0;
	else
		return 1;
}

void swap_ts_xy(int * x, int * y)
{
	int tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;
}



/* 校准，获得公式 */
void ts_calibrate(void)
{
	int a_ts_x, a_ts_y;
	int b_ts_x, b_ts_y;
	int c_ts_x, c_ts_y;
	int d_ts_x, d_ts_y;
	int e_ts_x, e_ts_y;

	get_lcd_params(&xres, &yres, &bpp, &fb_base);

	/*a点*/
	fb_disp_cross(50, 50, 0xff00);
	get_ts_xy(&a_ts_x, &a_ts_y);
	fb_disp_cross(50, 50, 0);

	/*b点*/
	fb_disp_cross(xres - 50, 50, 0xff00);	
	get_ts_xy(&b_ts_x, &b_ts_y);
	fb_disp_cross(xres - 50, 50, 0);
	
	/*c点*/
	fb_disp_cross(xres - 50, yres - 50, 0xff00);	
	get_ts_xy(&c_ts_x, &c_ts_y);
	fb_disp_cross(xres - 50, yres - 50, 0);
	
	/*d点*/
	fb_disp_cross(50, yres - 50, 0xff00);
	get_ts_xy(&d_ts_x, &d_ts_y);
	fb_disp_cross(50, yres - 50, 0);
	
	/*e点*/
	fb_disp_cross(xres/2, yres/2, 0xff00);
	get_ts_xy(&e_ts_x, &e_ts_y);
	fb_disp_cross(xres/2, yres/2, 0);

	g_ts_xy_swap = Is_xy_swap(a_ts_x, a_ts_y, b_ts_x, b_ts_y);
	if (g_ts_xy_swap)
	{
		swap_ts_xy(&a_ts_x, &a_ts_y);
		swap_ts_xy(&b_ts_x, &b_ts_y);
		swap_ts_xy(&c_ts_x, &c_ts_y);
		swap_ts_xy(&d_ts_x, &d_ts_y);		
		swap_ts_xy(&e_ts_x, &e_ts_y);
	}

	g_kx = ((double)(2 * (xres - 100))) / (b_ts_x + c_ts_x - a_ts_x - d_ts_x);	
	g_ky = ((double)(2 * (yres - 100))) / (d_ts_y + c_ts_y - a_ts_y - b_ts_y);

	g_ts_xc = e_ts_x;
	g_ts_yc = e_ts_y;

	printf("A lcd data: x = %d, y = %d\n\r", get_lcd_x_frm_ts(a_ts_x), get_lcd_y_frm_ts(a_ts_y));
	printf("B lcd data: x = %d, y = %d\n\r", get_lcd_x_frm_ts(b_ts_x), get_lcd_y_frm_ts(b_ts_y));
	printf("C lcd data: x = %d, y = %d\n\r", get_lcd_x_frm_ts(c_ts_x), get_lcd_y_frm_ts(c_ts_y));
	printf("D lcd data: x = %d, y = %d\n\r", get_lcd_x_frm_ts(d_ts_x), get_lcd_y_frm_ts(d_ts_y));
	printf("E lcd data: x = %d, y = %d\n\r", get_lcd_x_frm_ts(e_ts_x), get_lcd_y_frm_ts(e_ts_y));

}

/* 通过公式，获得lcd上的坐标 */
int ts_read(int * lcd_x, int * lcd_y, int * pressure)
{
	int ts_x, ts_y, ts_pressure;
	int tmp_x, tmp_y;

	ts_read_raw(&ts_x, &ts_y, &ts_pressure);

	if (g_ts_xy_swap)
	{
		swap_ts_xy(&ts_x, &ts_y);
	}
	
	tmp_x = (ts_x - g_ts_xc) * g_kx + xres / 2;	
	tmp_y = (ts_y - g_ts_yc) * g_ky + yres / 2;

	if (tmp_x < 0 || tmp_x >= xres || tmp_y < 0 || tmp_y >= yres)
		return -1;

	*lcd_x = tmp_x;
	*lcd_y = tmp_y;
	*pressure = ts_pressure;
	return 0;
}




