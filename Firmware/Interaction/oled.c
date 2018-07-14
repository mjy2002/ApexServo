/******
************************************************************************
******
** @file		: oled.h
** @brief		: oled显示
************************************************************************
******
** @versions	: 1.0.0
** @time		: 2018/6/20
************************************************************************
******
** @authors		: unli (China WuHu)
************************************************************************
******
** @explain		:
** 	1.oled
** 	2.oled
************************************************************************
************************************************************************
******/

#include "oled.h"

//STM32
#include "main.h"
#include "stm32f1xx_hal.h"
//Interaction
#include "oled.h"
#include "oled_form.h"

//引用的外部定义及其引用方式
extern SPI_HandleTypeDef hspi2;
#define oled_spi_tyd		hspi2
#define oled_spi_transmit(buff, size, time)		HAL_SPI_Transmit(&oled_spi_tyd, buff, size, time)
#define oled_res_h()	Quick_GPIO_H(OLED_RES_GPIO_Port, OLED_RES_Pin)
#define oled_res_l()	Quick_GPIO_L(OLED_RES_GPIO_Port, OLED_RES_Pin)
#define oled_dc_h()		Quick_GPIO_H(OLED_DC_GPIO_Port, OLED_DC_Pin)
#define oled_dc_l()		Quick_GPIO_L(OLED_DC_GPIO_Port, OLED_DC_Pin)
#define oled_cs_h()		Quick_GPIO_H(OLED_NSS_GPIO_Port, OLED_NSS_Pin)
#define oled_cs_l()		Quick_GPIO_L(OLED_NSS_GPIO_Port, OLED_NSS_Pin)


/***********************  SSD1306库  ***********************/

void SSD1306_Write_Data	(uint8_t data)
{		  
	oled_dc_h();  
	oled_cs_l();
	oled_spi_transmit(&data, 1, 1000000);
	oled_cs_h(); 		
}


void SSD1306_Write_Cmd	(uint8_t data)
{		  
	oled_dc_l();  
	oled_cs_l();
	oled_spi_transmit(&data, 1, 1000000);
	oled_cs_h(); 		
}

void SSD1306_Init(void)
{
	oled_res_l();
	HAL_Delay(100);
	oled_res_h(); 

	SSD1306_Write_Cmd(0xAE);//--turn off oled panel
	SSD1306_Write_Cmd(0x00);//---set low column address
	SSD1306_Write_Cmd(0x10);//---set high column address
	SSD1306_Write_Cmd(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	SSD1306_Write_Cmd(0x81);//--set contrast control register
	SSD1306_Write_Cmd(0xCF); // Set SEG Output Current Brightness
	SSD1306_Write_Cmd(0xA1);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	SSD1306_Write_Cmd(0xC8);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	SSD1306_Write_Cmd(0xA6);//--set normal display
	SSD1306_Write_Cmd(0xA8);//--set multiplex ratio(1 to 64)
	SSD1306_Write_Cmd(0x3f);//--1/64 duty
	SSD1306_Write_Cmd(0xD3);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	SSD1306_Write_Cmd(0x00);//-not offset
	SSD1306_Write_Cmd(0xd5);//--set display clock divide ratio/oscillator frequency
	SSD1306_Write_Cmd(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
	SSD1306_Write_Cmd(0xD9);//--set pre-charge period
	SSD1306_Write_Cmd(0xF1);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	SSD1306_Write_Cmd(0xDA);//--set com pins hardware configuration
	SSD1306_Write_Cmd(0x12);
	SSD1306_Write_Cmd(0xDB);//--set vcomh
	SSD1306_Write_Cmd(0x40);//Set VCOM Deselect Level
	SSD1306_Write_Cmd(0x20);//-Set Page Addressing Mode (0x00/0x01/0x02)
	SSD1306_Write_Cmd(0x02);//
	SSD1306_Write_Cmd(0x8D);//--set Charge Pump enable/disable
	SSD1306_Write_Cmd(0x14);//--set(0x10) disable
	SSD1306_Write_Cmd(0xA4);// Disable Entire Display On (0xa4/0xa5)
	SSD1306_Write_Cmd(0xA6);// Disable Inverse Display On (0xa6/a7) 
	SSD1306_Write_Cmd(0xAF);//--turn on oled panel

	SSD1306_Clear();
	SSD1306_Set_Pos(0,0);
}

void SSD1306_Display_ON(void)
{
	SSD1306_Write_Cmd(0X8D);  //SET DCDC命令
	SSD1306_Write_Cmd(0X14);  //DCDC ON
	SSD1306_Write_Cmd(0XAF);  //DISPLAY ON
}

void SSD1306_Display_OFF(void)
{
	SSD1306_Write_Cmd(0X8D);  //SET DCDC命令
	SSD1306_Write_Cmd(0X10);  //DCDC OFF
	SSD1306_Write_Cmd(0XAE);  //DISPLAY OFF
}

void SSD1306_Clear(void)
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		SSD1306_Set_Pos(0, i);
		for(j=0;j<128;j++)
			SSD1306_Write_Data(0);
	}
}

void SSD1306_Set_Pos(uint8_t x, uint8_t y)
{
	SSD1306_Write_Cmd(0xb0+y);
	SSD1306_Write_Cmd(((x&0xf0)>>4)|0x10);
	SSD1306_Write_Cmd((x&0x0f)|0x01); 
}

void SSD1306_DrawPoint(uint8_t x, uint8_t y)
{
  SSD1306_Set_Pos(x,y); 	
  SSD1306_Write_Cmd(0xb0+(y>>3));
  SSD1306_Write_Cmd(((x&0xf0)>>4)|0x10);
  SSD1306_Write_Cmd((x&0x0f)|0x00);
  SSD1306_Write_Data(0x01<<(y%8)); 	 	
}

void SSD1306_Show_String(uint8_t x, uint8_t y, uint8_t form, uint8_t *str)
{
	uint32_t i,j;

	if(form == SSD1306_Form_Fx8)
	{	
		for(j=0; str[j]!='\0'; j++,x+=6)
		{
			if(x > (128-6))		{x = 0; y+=1;}
			if(y > (8-1))		{y = 0;}
			SSD1306_Set_Pos(x, y);
			for(i=0; i<6; i++)
				SSD1306_Write_Data(ASCII_F6x8[str[j]][i]);
		}
	}
	else if(form == SSD1306_Form_Fx16)
	{
		for(j=0; str[j]!='\0'; j++,x+=8)
		{
			//检查位置
			if(x > (128-8))		{x = 0; y+=2;}
			if(y > (8-2))		{y = 0;}
			//刷入数据
			SSD1306_Set_Pos(x, y);
			for(i=0; i<8; i++)
				SSD1306_Write_Data(ASCII_F8x16[str[j]][i]);
			SSD1306_Set_Pos(x, y+1);
			for(i=0; i<8; i++)
				SSD1306_Write_Data(ASCII_F8x16[str[j]][i+8]);
		}
	}
}

void SSD1306_Show_Chinese(uint8_t x, uint8_t y, uint8_t *str)
{
	uint32_t i,j,k;
	bool f;
	
	for(j=0; str[j]!='\0'; j+=2,x+=14)
	{
		//搜索字符
		f = false;
		k = 0;
		while(Chinese_F14x16_Idx[k] > 127)
		{
			if((Chinese_F14x16_Idx[k] == str[j]) && (Chinese_F14x16_Idx[k+1] == str[j+1]))
			{
				f = true;
				k = k/2;
				break;
			}
			k += 2;
		}
		//检查位置
		if(x > (128-14))	{x = 0; y += 2;}
		if(y > (8-2))		{y = 0;}
		//刷入数据
		if(f)
		{
			SSD1306_Set_Pos(x, y);
			for(i=0; i<14; i++)
				SSD1306_Write_Data(Chinese_F14x16[k][i]);
			SSD1306_Set_Pos(x, y+1);
			for(i=0; i<14; i++)
				SSD1306_Write_Data(Chinese_F14x16[k][i+14]);
		}
		else
		{
			SSD1306_Set_Pos(x, y);
			for(i=0; i<14; i++)
				SSD1306_Write_Data(0);
			SSD1306_Set_Pos(x, y+1);
			for(i=0; i<14; i++)
				SSD1306_Write_Data(0);
		}
	}
}

void SSD1306_Show_StrChi(uint8_t x, uint8_t y, uint8_t *str)
{
	uint32_t j;
	uint8_t strr[3];

	while(str[j]!='\0')
	{
		if(str[j] > 127)
		{
			//显示汉字
			strr[0] = str[j];
			strr[1] = str[j+1];
			strr[2] = '\0';
			SSD1306_Show_Chinese(x, y, strr);
			x += 14;
			j += 2;
		}
		else
		{
			//显示字符
			strr[0] = str[j];
			strr[1] = '\0';
			SSD1306_Show_String(x, y, SSD1306_Form_Fx16, strr);
			x += 8;
			j += 1;
		}
	}
}

void SSD1306_Show_Num(uint8_t x, uint8_t y, uint8_t form, int32_t num)
{
	uint8_t NumBuff[20];
	if(form == SSD1306_Form_Fx8)
		sprintf((char*)NumBuff, "%10d", (int32_t)num);
	else if(form == SSD1306_Form_Fx16)
		sprintf((char*)NumBuff, "%8d", (int32_t)num);
	SSD1306_Show_String(x, y, form, NumBuff);
}

void SSD1306_Show_FloatNum(uint8_t x, uint8_t y, uint8_t form, float num)
{
	uint8_t NumBuff[20];
	if(form == SSD1306_Form_Fx8)
		sprintf((char*)NumBuff, "%5.4f", (float)num);
	else if(form == SSD1306_Form_Fx16)
		sprintf((char*)NumBuff, "%5.2f", (float)num);
	SSD1306_Show_String(x, y, form, NumBuff);
}


/***********************  虚拟空间库  ***********************/
uint8_t location_x = 0;
uint8_t location_y = 0;
uint8_t Virtual_Data[8][128];

void Virtual_Upload(void)
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		SSD1306_Set_Pos(0, i);
		for(j=0;j<128;j++)
			SSD1306_Write_Data(Virtual_Data[i][j]);
	}
}

void Virtual_SSD1306_Clear(void)
{
	uint8_t i,j;
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
			Virtual_Data[i][j] = 0;
	}
}

void Virtual_SSD1306_Set_Pos(uint8_t x, uint8_t y)
{
	location_x = x;
	location_y = y;
}

void Virtual_SSD1306_Show_String(uint8_t x, uint8_t y, uint8_t form, uint8_t *str)
{
	uint32_t i,j;

	if(form == SSD1306_Form_Fx8)
	{	
		for(j=0; str[j]!='\0'; j++,x+=6)
		{
			if(x > (128-6))		{x = 0; y+=1;}
			if(y > (8-1))		{y = 0;}
			for(i=0; i<6; i++)
				Virtual_Data[y][x+i] = ASCII_F6x8[str[j]][i];
		}
	}
	else if(form == SSD1306_Form_Fx16)
	{
		for(j=0; str[j]!='\0'; j++,x+=8)
		{
			//检查位置
			if(x > (128-8))		{x = 0; y+=2;}
			if(y > (8-2))		{y = 0;}
			//刷入数据
			for(i=0; i<8; i++)
				Virtual_Data[y][x+i] = ASCII_F8x16[str[j]][i];
			for(i=0; i<8; i++)
				Virtual_Data[y+1][x+i] = ASCII_F8x16[str[j]][i+8];
		}
	}
}

void Virtual_SSD1306_Show_Chinese(uint8_t x, uint8_t y, uint8_t *str)
{
	uint32_t i,j,k;
	bool f;
	
	for(j=0; str[j]!='\0'; j+=2,x+=14)
	{
		//搜索字符
		f = false;
		k = 0;
		while(Chinese_F14x16_Idx[k] > 127)
		{
			if((Chinese_F14x16_Idx[k] == str[j]) && (Chinese_F14x16_Idx[k+1] == str[j+1]))
			{
				f = true;
				k = k/2;
				break;
			}
			k += 2;
		}
		//检查位置
		if(x > (128-14))	{x = 0; y += 2;}
		if(y > (8-2))		{y = 0;}
		//刷入数据
		if(f)
		{
			for(i=0; i<14; i++)
				Virtual_Data[y][x+i] = Chinese_F14x16[k][i];
			for(i=0; i<14; i++)
				Virtual_Data[y+1][x+i] = Chinese_F14x16[k][i+14];
		}
		else
		{
			for(i=0; i<14; i++)
				Virtual_Data[y][x+i] = 0;
			for(i=0; i<14; i++)
				Virtual_Data[y+1][x+i] = 0;
		}
	}
}

void Virtual_SSD1306_Show_StrChi(uint8_t x, uint8_t y, uint8_t *str)
{
	uint32_t j;
	uint8_t strr[3];

	while(str[j]!='\0')
	{
		if(str[j] > 127)
		{
			//显示汉字
			strr[0] = str[j];
			strr[1] = str[j+1];
			strr[2] = '\0';
			Virtual_SSD1306_Show_Chinese(x, y, strr);
			x += 14;
			j += 2;
		}
		else
		{
			//显示字符
			strr[0] = str[j];
			strr[1] = '\0';
			Virtual_SSD1306_Show_String(x, y, SSD1306_Form_Fx16, strr);
			x += 8;
			j += 1;
		}
	}
}

void Virtual_SSD1306_Show_Num(uint8_t x, uint8_t y, uint8_t form, int32_t num)
{
	uint8_t NumBuff[20];
	if(form == SSD1306_Form_Fx8)
		sprintf((char*)NumBuff, "%11d", (int32_t)num);
	else if(form == SSD1306_Form_Fx16)
		sprintf((char*)NumBuff, "%11d", (int32_t)num);
	Virtual_SSD1306_Show_String(x, y, form, NumBuff);
}

void Virtual_SSD1306_Show_FloatNum(uint8_t x, uint8_t y, uint8_t form, float num)
{
	uint8_t NumBuff[20];
	if(form == SSD1306_Form_Fx8)
		sprintf((char*)NumBuff, "%11.3f", (float)num);
	else if(form == SSD1306_Form_Fx16)
		sprintf((char*)NumBuff, "%11.3f", (float)num);
	Virtual_SSD1306_Show_String(x, y, form, NumBuff);
}


// //向SSD1106写入一个字节。
// //dat:要写入的数据/命令
// //cmd:数据/命令标志 0,表示命令;1,表示数据;
// void OLED_WR_Byte(uint8_t dat,uint8_t cmd)
// {	
// 	uint8_t i;			  
// 	if(cmd)
// 		OLED_DC_Set();
// 	else 
// 		OLED_DC_Clr();		  
// 	OLED_CS_Clr();
// 	for(i=0;i<8;i++)
// 	{			  
// 		OLED_SCLK_Clr();
// 		if(dat&0x80)
// 			OLED_SDIN_Set();
// 		else 
// 			OLED_SDIN_Clr();
// 		OLED_SCLK_Set();
// 		dat<<=1;   
// 	}				 		  
// 	OLED_CS_Set();
// 	OLED_DC_Set();   	  
// } 

// //设置定位
// void OLED_Set_Pos(uint8_t x, uint8_t y) 
// { 
// 	OLED_WR_Byte(0xb0+y,OLED_CMD);
// 	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
// 	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
// }

// //开启OLED显示    
// void OLED_Display_On(void)
// {
// 	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
// 	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
// 	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
// }

// //关闭OLED显示     
// void OLED_Display_Off(void)
// {
// 	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
// 	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
// 	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
// }		

// //清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
// void OLED_Clear(void)  
// {  
// 	uint8_t i,n;		    
// 	for(i=0;i<8;i++)  
// 	{  
// 		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
// 		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
// 		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
// 		for(n=0;n<128;n++)
// 			OLED_WR_Byte(0,OLED_DATA); 
// 	} //更新显示
// }

// //在指定位置显示一个字符,包括部分字符
// //x:0~127
// //y:0~63
// //mode:0,反白显示;1,正常显示				 
// //size:选择字体 16/12 
// void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr)
// {      	
// 	uint8_t c=0,i=0;	
// 	c=chr-' ';//得到偏移后的值			
// 	if(x>Max_Column-1){x=0;y=y+2;}
// 	if(SIZE ==16)
// 	{
// 		OLED_Set_Pos(x,y);	
// 		for(i=0;i<8;i++)
// 			OLED_WR_Byte(F8x16[c*16+i],OLED_DATA);
// 		OLED_Set_Pos(x,y+1);
// 		for(i=0;i<8;i++)
// 			OLED_WR_Byte(F8x16[c*16+i+8],OLED_DATA);
// 	}
// 	else {	
// 		OLED_Set_Pos(x,y+1);
// 		for(i=0;i<6;i++)
// 			OLED_WR_Byte(F6x8[c][i],OLED_DATA);
// 	}
// }

// //m^n函数
// uint32_t oled_pow(uint8_t m,uint8_t n)
// {
// 	uint32_t result=1;	 
// 	while(n--)result*=m;    
// 	return result;
// }	

// //显示2个数字
// //x,y :起点坐标	 
// //len :数字的位数
// //size:字体大小
// //mode:模式	0,填充模式;1,叠加模式
// //num:数值(0~4294967295);	 		  
// void OLED_ShowNum(uint8_t x,uint8_t y,u32 num,uint8_t len,uint8_t size)
// {         	
// 	uint8_t t,temp;
// 	uint8_t enshow=0;						   
// 	for(t=0;t<len;t++)
// 	{
// 		temp=(num/oled_pow(10,len-t-1))%10;
// 		if(enshow==0&&t<(len-1))
// 		{
// 			if(temp==0)
// 			{
// 				OLED_ShowChar(x+(size/2)*t,y,' ');
// 				continue;
// 			}else enshow=1; 

// 		}
// 		OLED_ShowChar(x+(size/2)*t,y,temp+'0'); 
// 	}
// } 

// //显示一个字符号串
// void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr)
// {
// 	uint8_t j=0;
// 	while (chr[j]!='\0')
// 	{
// 		OLED_ShowChar(x,y,chr[j]);
// 		x+=8;
// 		if(x>120){x=0;y+=2;}
// 		j++;
// 	}
// }

// //显示汉字
// void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
// {      			    
// 	uint8_t t,adder=0;
// 	OLED_Set_Pos(x,y);	
// 	for(t=0;t<16;t++)
// 	{
// 		OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
// 		adder+=1;
// 	}	
// 	OLED_Set_Pos(x,y+1);	
// 	for(t=0;t<16;t++)
// 	{	
// 		OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
// 		adder+=1;
// 	}					
// }

// /***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
// void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[])
// { 	
// 	uint32_t j=0;
// 	uint8_t x,y;

// 	if(y1%8==0) y=y1/8;      
// 	else y=y1/8+1;
// 	for(y=y0;y<y1;y++)
// 	{
// 		OLED_Set_Pos(x0,y);
// 		for(x=x0;x<x1;x++)
// 		{      
// 			OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
// 		}
// 	}
// } 

// //初始化SSD1306					    
// void OLED_Init(void)
// { 	
// 	OLED_RST_Set();
// 	delay_ms(100);
// 	OLED_RST_Clr();
// 	delay_ms(200);
// 	OLED_RST_Set(); 

// 	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
// 	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
// 	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
// 	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
// 	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
// 	OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
// 	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
// 	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
// 	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
// 	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
// 	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
// 	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
// 	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
// 	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
// 	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
// 	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
// 	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
// 	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
// 	OLED_WR_Byte(0x12,OLED_CMD);
// 	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
// 	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
// 	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
// 	OLED_WR_Byte(0x02,OLED_CMD);//
// 	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
// 	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
// 	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
// 	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
// 	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel

// 	OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
// 	OLED_Clear();
// 	OLED_Set_Pos(0,0); 	
// }  









