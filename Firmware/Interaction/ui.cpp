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

#include "ui.h"


//Interaction
#include "oled.h"
#include "button.h"
//Kernel
#include "controller.h"

//引用的外部定义及其引用方式
extern Controller control;

uint16_t page = 0;

void UI_Display(uint32_t time_ms)
{
	Button_Scan(50);

	//Button_Left
	if(Button_Left.state == Key_bit_Drop)
	{
		if(page > 0)	page--;
	}
	
	//Button_Middle
	if(Button_Middle.state == Key_bit_Press)
	{
		control.control_mode = Control_Mode_Disable;	//失能控制模式
	}
	else if((Button_Middle.state == Key_bit_Rise) || (Button_Middle.state == Key_bit_LongRise))
	{
		control.up_st = true;	//标准化指令
	}
	
	//Button_Right
	if(Button_Right.state == Key_bit_Drop)
	{
		if(page < 3)	page++;
	}
	
	if(page == 0)
	{
		Virtual_SSD1306_Clear();
		Virtual_SSD1306_Show_String(0, 0, SSD1306_Form_Fx16, (uint8_t*)"Read_Data");
		Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"enre:");
		Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"sens:");
		Virtual_SSD1306_Show_String(0, 6, SSD1306_Form_Fx16, (uint8_t*)"read:");
		Virtual_SSD1306_Show_Num(40, 2, SSD1306_Form_Fx16, (int32_t)control.enread);
		Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.sensor_p);
		Virtual_SSD1306_Show_FloatNum(40, 6, SSD1306_Form_Fx16, (float)control.read_p);
		Virtual_Upload();
	}
	else if(page == 1)
	{
		Virtual_SSD1306_Clear();
		Virtual_SSD1306_Show_String(0, 0, SSD1306_Form_Fx16, (uint8_t*)"Filter_Data");
		Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"p   :");
		Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"v   :");
		Virtual_SSD1306_Show_String(0, 6, SSD1306_Form_Fx16, (uint8_t*)"a   :");
		Virtual_SSD1306_Show_FloatNum(40, 2, SSD1306_Form_Fx16, (float)control.filter_p);
		Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.filter_v);
		Virtual_SSD1306_Show_FloatNum(40, 6, SSD1306_Form_Fx16, (float)control.filter_a);
		Virtual_Upload();
	}
	else if(page == 2)
	{
		Virtual_SSD1306_Clear();
		//刷输入模式
		if(Import_Mode_Pulse == control.import_mode)
		{
			Virtual_SSD1306_Show_String(0, 0, SSD1306_Form_Fx16, (uint8_t*)"Import_Pulse");
		}
		else if(Import_Mode_Digital == control.import_mode)
		{
			Virtual_SSD1306_Show_String(0, 0, SSD1306_Form_Fx16, (uint8_t*)"Import_Digital");
		}
		//刷控制模式
		if(Control_Mode_PosiVelo == control.control_mode)
		{
			Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"Control_PosiVelo");
			Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"go_p:");
			Virtual_SSD1306_Show_String(0, 6, SSD1306_Form_Fx16, (uint8_t*)"re_p:");
			Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.goal_p);
			Virtual_SSD1306_Show_FloatNum(40, 6, SSD1306_Form_Fx16, (float)control.read_p);
		}
		else if(Control_Mode_Position == control.control_mode)
		{
			Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"Control_Position");
			Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"go_p:");
			Virtual_SSD1306_Show_String(0, 6, SSD1306_Form_Fx16, (uint8_t*)"re_p:");
			Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.goal_p);
			Virtual_SSD1306_Show_FloatNum(40, 6, SSD1306_Form_Fx16, (float)control.read_p);
		}
		else if(Control_Mode_Velocity == control.control_mode)
		{
			Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"Control_Velocity");
			Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"go_v:");
			Virtual_SSD1306_Show_String(0, 6, SSD1306_Form_Fx16, (uint8_t*)"fi_v:");
			Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.goal_v);
			Virtual_SSD1306_Show_FloatNum(40, 6, SSD1306_Form_Fx16, (float)control.filter_v);
		}
		else if(Control_Mode_Torque == control.control_mode)
		{
			Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"Control_Torque");
			Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"go_t:");
			Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.goal_t);
		}
		else if(Control_Mode_Disable == control.control_mode)
		{
			Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"Control_Disable");
		}
		Virtual_Upload();
	}
	else if(page == 3)
	{	
		Virtual_SSD1306_Clear();
		Virtual_SSD1306_Show_String(0, 0, SSD1306_Form_Fx16, (uint8_t*)"Out_Data");
		Virtual_SSD1306_Show_String(0, 2, SSD1306_Form_Fx16, (uint8_t*)"phas:");
		Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, (uint8_t*)"elec:");
		Virtual_SSD1306_Show_FloatNum(40, 2, SSD1306_Form_Fx16, (float)control.out_p);
		Virtual_SSD1306_Show_FloatNum(40, 4, SSD1306_Form_Fx16, (float)control.out_i);
		Virtual_Upload();
	}
}

void UI_Cover(uint8_t *str)
{
	Virtual_SSD1306_Clear();
	Virtual_SSD1306_Show_String(0, 4, SSD1306_Form_Fx16, str);
	Virtual_Upload();
}










