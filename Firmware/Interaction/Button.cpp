/******
	************************************************************************
	******
	** @file		: Button.cpp/h
	** @brief		: 按键驱动
	************************************************************************
	******
	** @versions	: 1.0.0
	** @time		: 2018/5/20
	************************************************************************
	******
	** @authors		: unli (China WuHu)
	************************************************************************
	******
	** @explain		:
	** 	1.扫描按键电平的以获得每个按键状态的
	** 	2.Scan the key level to get the status of each key
	************************************************************************
	************************************************************************
******/

#include "button.h"

#include "stm32f1xx_hal.h"

//引用的外部定义及其引用方式


Button button[Button_Num];

void Button_Init(void)
{
	button[0].press_level = GPIO_PIN_RESET;
	button[0].long_time_ms = 500;
	button[0].port = S1_GPIO_Port;
	button[0].pin = S1_Pin;
	
	button[1].press_level = GPIO_PIN_RESET;
	button[1].long_time_ms = 500;
	button[1].port = S2_GPIO_Port;
	button[1].pin = S2_Pin;
	
	button[2].press_level = GPIO_PIN_RESET;
	button[2].long_time_ms = 500;
	button[2].port = S3_GPIO_Port;
	button[2].pin = S3_Pin;
}

void Button_Scan(uint32_t in_time_ms)
{
	uint16_t i;
	bool press;
	
	for(i=0;i<Button_Num;i++)
	{
		button[i].level = HAL_GPIO_ReadPin(button[i].port, button[i].pin);
		if(button[i].level == button[i].press_level)
			press = true;
		else
			press = false;
		switch(button[i].state)
		{
			case Key_bit_Up:
				if(press){
					button[i].state = Key_bit_Press_;
					button[i].time_ms = 0;
				}
			break;
			case Key_bit_Press_:
				if(press){
					button[i].time_ms += in_time_ms;
					if(button[i].time_ms > Suitable_Time_)
					{
						button[i].state = Key_bit_Drop;
						button[i].time_ms = 0;
					}
				}
				else{
					button[i].state = Key_bit_Up;
					button[i].time_ms = 0;
				}
			break;
			case Key_bit_Press:
				if(press){
					button[i].time_ms += in_time_ms;
					if(button[i].time_ms > button[i].long_time_ms)
					{
						button[i].state = Key_bit_Long;
						button[i].time_ms = 0;
					}
				}
				else{
					button[i].state = Key_bit_Rise;
					button[i].time_ms = 0;
				}
			break;
			case Key_bit_Long:
				if(press){
					button[i].state = Key_bit_Long;
				}
				else{
					button[i].state = Key_bit_LongRise;
				}
			break;
			case Key_bit_Drop:
				if(press){
					button[i].state = Key_bit_Press;
					button[i].time_ms = 0;
				}
				else{
					button[i].state = Key_bit_Up;
				}
			break;
			case Key_bit_Rise:
				button[i].state = Key_bit_Up;
			break;
			case Key_bit_LongRise:
				button[i].state = Key_bit_Up;
			break;
		}	
	}
}

//Button::Button(GPIO_TypeDef *_Port, uint16_t _Pin, GPIO_PinState _Press_Level, uint16_t _Long_Time)
//{
//	port = _Port;
//	pin = _Pin;
//	press_level = _Press_Level;
//	long_time = _Long_Time;
//	
//	state = Key_bit_Up;
//	time = 0;
//	level = _Press_Level;
//}

//void Button::Gather(uint16_t interval_ms)
//{
//	bool press;

//	level = HAL_GPIO_ReadPin(port, pin);
//	if(level == press_level)
//		press = true;
//	else
//		press = false;

//	switch(state)
//	{
//		case Key_bit_Up:
//			if(press){
//				state = Key_bit_Press_;
//				time = 0;
//			}
//		break;
//		case Key_bit_Press_:
//			if(press){
//				time += interval_ms;
//				if(time > Suitable_Time_)
//				{
//					state = Key_bit_Drop;
//					time = 0;
//				}
//			}
//			else{
//				state = Key_bit_Up;
//				time = 0;
//			}
//		break;
//		case Key_bit_Press:
//			if(press){
//				time += interval_ms;
//				if(time > long_time)
//				{
//					state = Key_bit_Long;
//					time = 0;
//				}
//			}
//			else{
//				state = Key_bit_Rise;
//				time = 0;
//			}
//		break;
//		case Key_bit_Long:
//			if(press){
//				state = Key_bit_Long;
//			}
//			else{
//				state = Key_bit_LongRise;
//			}
//		break;
//		case Key_bit_Drop:
//			if(press){
//				state = Key_bit_Press;
//				time = 0;
//			}
//			else{
//				state = Key_bit_Up;
//			}
//		break;
//		case Key_bit_Rise:
//			state = Key_bit_Up;
//		break;
//		case Key_bit_LongRise:
//			state = Key_bit_Up;
//		break;
//	}
//}

