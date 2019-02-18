# 1. 原理

1. **lcd_controller介绍**<br><br>S3C2440A的lcd_controller包含一个逻辑单元，它能将存储在系统内存video buffer中的image信息传输到外部lcd设备中<br><br>S3C2440A支持：<br>    STN LCD<br>    TFT LCD：1、2、4、8bpp的调色板接口，以及16、24bpp的无调色板真彩显示

![](image/lcd_原理图.png)

* 上图是lcd原理图<br>其中：VDD : <br>            DE   : <br>            VEE : <br>            R0-B5 : 数据口<br>            CLK : <br>            YU : <br>            XL : <br>            YL : <br>            XR : 

![](image/lcd_controller.png)

* 上图是lcd_controller的运行过程图，具体过程如下：<br>1. REGBANK：拥有17个可编程寄存器集和256x16调色板内存，用于配置lcd控制器。<br>2. TIMEGEN：包含可编程的逻辑单元以支持外部不同的lcd driver的不同接口时序和速率，它能产生VFRAME、<br>     VLINE、 VCLK、 VM等信号。<br>3. 当我们把video data写入相应的显存(即video buffer)中时<br>4. LCDCDMA是lcd_controller专用的DMA通道。当LCDCDMA中的FIFO为空或部分空时，它会获取总线的<br>    控制权，将video buffer中的data传输到FIFO中。LCDCDMA有2个FIFO，当使用“双扫”模式时，FIFOH/FIFOL<br>    分别传输上/下半屏的数据；当使用“单扫“模式，只用FIFOH。<br>5. VIDPRCS将FIFO中的数据转换成合适的格式并将其发送至`VD[23:0]`，最后数据就会显示在lcd上。

![](image/lcd_timing.png)

* 上图是lcd芯片手册中的时序图<br>1. 一开始，发出Vsync低信号并保持tvp时间，Vsync拉高，再过tvb，此时DE信号使能，同时，拉低Hsync信号并保持           thp，再过thb，电子枪开始获取数据，在CLK信号的下降沿获取数据并发射到lcd屏的像素点上。<br>2. 经过thd，lcd屏上一行数据接收并发送完毕，再经过thf，Hsync信号再次拉低，开始第二行的数据接收并发射。        然后循环上述操作，直到最后一个像素点的数据打印在屏幕上。<br>3. 当最后一个像素点的数据打印在屏幕上后，再过tvf后，Vsync信号拉低，一帧的数据接收并发射完毕，<br>    开始发射下      一帧的视频数据。如此往复循环……<br><br>

---

2. **数据格式**<br>lcd显示屏的某一瞬间，即一幅画面就是一帧（1frame），一帧有多行组成，一行有多个像素组成，则每个像素显示什么颜色就是由数据格式决定的。

   1. 8bpp (bits per pixel)

      每个像素点有8bit数据来表示，但是8bit只能表示256种颜色，因此，我们可以在指定的内存区域中写入相应的调色板。当给定一个数值时，由该数字去调色板中取出对应的颜色。<br>其中，调色板的像素格式支持565和555I两种格式。

   2. 16bpp

      每个像素点有16 bit数据来表示，那么4BYTE可以表示2个像素，像素的排列顺序可以由编程控制。

      1. 565 format

         高5bit表示红，中5bit表示绿，低5bit表示蓝

      2. 555I format

         555I又称为RGBA (A: Alpha, 代表透明度)

   3. 24bpp

      每个像素由24bit的数据来表示，RGB每种原色占据8bit，为DMA传输方便，用32bit来装24bit的数据，空余1BYTE不用，至于是低8bit还是高8bit，可以软件设定。

---

3. **虚拟显示（virtual display）**

![](image/lcd_virtual_display.png)

* 上图便是虚拟显示的示意图

* 定义：所谓的虚拟显示就是，如果当前有一张很大的图片，但是在4.3寸的屏幕上无法完全显示，它支持只显示图片的一部分，但是可以通过移动窗口来达到浏览全图的目的。

* 原理：当我们划动屏幕时，视口的开始地址和结束地址会相应的变化，但是PAGEWIDTH和OFFSIZE是不变的，从而达到浏览全屏的效果。

# 2. 实现

### 2.1 步骤

1. 初始化lcd相关的PIN，eg. 背光引脚、lcd数据引脚、电源使能引脚etc

2. 根据LCD datasheet中各个时序参数，设置lcd controller的相应的时序参数，eg. tvp/thp/tvb etc

3. 设置数据格式、引脚信号极性，eg. 获取数据时电平变化方向、数据格式（565、555I）

4. 设置显存frame buffer的始末地址，外设LCD的xres/yres/bpp

5. 使能LCD controller，即可操作lcd屏幕画线等。<br>注意：在屏幕上显示图片或文字，都是点亮相应的像素点，因此一切画图的最基本的操作都是画点。

### 2.2 代码

* 4.3 寸TFT（见github/arm_hd/learn_s3c2440）

* 虚拟显示（待实现）
