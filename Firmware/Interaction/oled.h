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

#ifndef OLED_H
#define OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
	
//OLED RAM
//128*64
//共分为8行，128列，每行列为8个LED竖向排列

enum{
	SSD1306_Form_Fx8 = 0,
	SSD1306_Form_Fx16,
};

/***********************  SSD1306库  ***********************/
//基本数据写入
void SSD1306_Write_Data	(uint8_t data);	//写数据
void SSD1306_Write_Cmd	(uint8_t cmd);	//写命令
//基本功能
void SSD1306_Init		(void);					//OLED初始化
void SSD1306_Display_ON	(void);					//开启OLED显示
void SSD1306_Display_OFF(void);					//关闭OLED显示
void SSD1306_Clear		(void);					//OLED清屏
void SSD1306_Set_Pos	(uint8_t x, uint8_t y);	//设置定位
void SSD1306_DrawPoint	(uint8_t x, uint8_t y);	//OLED显示一个点
//数据功能
void SSD1306_Show_String	(uint8_t x, uint8_t y, uint8_t form, uint8_t *str);	 	//显示一个字符串
void SSD1306_Show_Chinese	(uint8_t x, uint8_t y, uint8_t *str);	 				//显示一个汉字串
void SSD1306_Show_StrChi	(uint8_t x, uint8_t y, uint8_t *str);	 				//显示一个混合串
void SSD1306_Show_Num		(uint8_t x, uint8_t y, uint8_t form, int32_t num);		//显示一个整形数
void SSD1306_Show_FloatNum	(uint8_t x, uint8_t y, uint8_t form, float num);		//显示一个浮点数
//图形功能


/***********************  虚拟空间库  ***********************/
//基本数据写入
void Virtual_Upload	(void);	//上传虚拟空间数据
//基本功能
void Virtual_SSD1306_Clear	(void);						//OLED清屏
void Virtual_SSD1306_Set_Pos(uint8_t x, uint8_t y);		//设置定位
//数据功能
void Virtual_SSD1306_Show_String	(uint8_t x, uint8_t y, uint8_t form, uint8_t *str);	 	//显示一个字符串
void Virtual_SSD1306_Show_Chinese	(uint8_t x, uint8_t y, uint8_t *str);	 				//显示一个汉字串
void Virtual_SSD1306_Show_StrChi	(uint8_t x, uint8_t y, uint8_t *str);	 				//显示一个混合串
void Virtual_SSD1306_Show_Num		(uint8_t x, uint8_t y, uint8_t form, int32_t num);		//显示一个整形数
void Virtual_SSD1306_Show_FloatNum	(uint8_t x, uint8_t y, uint8_t form, float num);		//显示一个浮点数
//图形功能




// void OLED_WR_Byte(uint8_t dat,uint8_t cmd);	    
// void OLED_Display_On(void);
// void OLED_Display_Off(void);	   							   		    
// void OLED_Init(void);
// void OLED_Clear(void);
// void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
// void OLED_Fill(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t dot);
// void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr);
// void OLED_ShowString(uint8_t x,uint8_t y, uint8_t *p);	 
// void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size);
// void OLED_Set_Pos(uint8_t x, uint8_t y);
// void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);
// void OLED_DrawBMP(uint8_t x0, uint8_t y0,uint8_t x1, uint8_t y1,uint8_t BMP[]);

#ifdef __cplusplus
}
#endif

#endif
