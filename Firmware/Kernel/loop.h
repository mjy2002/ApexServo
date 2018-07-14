/******
	************************************************************************
	******
	** @file		: loop.cpp/h
	** @brief		: 主程序环
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
	** 	1.主程序环
	** 	2.The main program loop
	************************************************************************
	************************************************************************
******/

#ifndef LOOP_H
#define LOOP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

//引用的实例
#include "stm32f1xx_hal.h"

void loop(void);
	 
//STM32中断向量函数，必须使用C形式声明
void TIM1_UP_IRQHandler(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
	 
#ifdef __cplusplus
}
#endif

#endif	//LOOP_H
