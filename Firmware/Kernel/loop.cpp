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

#include "loop.h"

//stm32
#include "main.h"
#include "stm32f1xx_hal.h"
//Interaction
#include "Button.h"
#include "usart_txrx.h"
#include "stockpile.h"
#include "IS.h"
#include "oled.h"
#include "ui.h"
//Kernel
#include "controller.h"
#include "AS5048.h"



//HAL 类定义
extern DAC_HandleTypeDef hdac;

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_tx;

extern TIM_HandleTypeDef htim1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart1_tx;


//时基
static uint32_t time_100us = 0;
static uint32_t time_1ms = 0;
static uint32_t time_10ms = 0;
static uint32_t time_20ms = 0;
static uint32_t time_50ms = 0;
static uint32_t time_100ms = 0;
static uint32_t time_200ms = 0;
static uint32_t time_500ms = 0;
static uint32_t time_1s = 0;


//变量
static char usart_it_data;		//串口接收数据

//类
AS5048 encode;
Controller control;


//功能
void loop_init(void);
void time_1ms_serve(void);
void time_10ms_serve(void);
void time_20ms_serve(void);
void time_50ms_serve(void);
void time_100ms_serve(void);
void time_200ms_serve(void);
void time_500ms_serve(void);
void time_1s_serve(void);


//主循环
void loop()
{
	//读取Flash数据并使用
	Store_Flash_To_RAM(true);

	//初始化	
	loop_init();

	//使能中断
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_UART_Receive_IT(&huart3, (uint8_t*)&usart_it_data, 1);
	
	while (1)
	{
		if(time_1ms){time_1ms--;time_1ms_serve();}
		if(time_10ms){time_10ms--;time_10ms_serve();}
		if(time_20ms){time_20ms--;time_20ms_serve();}
		if(time_50ms){time_50ms--;time_50ms_serve();}
		if(time_100ms){time_100ms--;time_100ms_serve();}
		if(time_200ms){time_200ms--;time_200ms_serve();}
		if(time_500ms){time_500ms--;time_500ms_serve();}
		if(time_1s){time_1s--;time_1s_serve();}
		
		//编码器校准
		if(control.up_st)
		{
			control.up_st = false;
			HAL_TIM_Base_Stop_IT(&htim1);
			UI_Cover((uint8_t*)"Standardizing...");
			if(control.Standardizing())
			{
				SerialPort_Send_string((char*)"Standardizing_OK!!!\r\n");
				Store_RAM_To_Flash(true);	//将参数写入Flash
				control.Start();			//控制器重新启动
				control.control_mode = Control_Mode_Position;	//位置控制模式
				UI_Cover((uint8_t*)"       OK       ");
				HAL_Delay(1000);
			}
			else
			{
				SerialPort_Send_string((char*)"Standardizing_ERROR!!!\r\n");
				control.control_mode = Control_Mode_Disable;	//失能控制模式
				UI_Cover((uint8_t*)"    ERROR!!!    ");
				HAL_Delay(1000);
			}
			HAL_TIM_Base_Start_IT(&htim1);
		}
	}
}

//
//初始化
//
void loop_init()
{	
	//Interaction初始化
	SSD1306_Init();
	Button_Init();

	//DAC初始化
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 0);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
	
	//LED初始化
	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	
	//中断信号预采集
	EXTI0_IRQHandler();
	EXTI1_IRQHandler();
	EXTI2_IRQHandler();
	
	//控制器启动
	control.Start();
}

//
//普通计时服务
//
void time_1ms_serve(void)
{

}

void time_10ms_serve(void)
{
	//IS指令扫描
	IS_Discern();
}

void time_20ms_serve(void)
{

}

void time_50ms_serve(void)
{
	//UI刷新
	UI_Display(50);
}

void time_100ms_serve(void)
{

}

void time_200ms_serve(void)
{

}

void time_500ms_serve(void)
{

}

void time_1s_serve(void)
{

}


//
//	定时器1中断响应函数	10k
//
void TIM1_UP_IRQHandler(void)
{
	static uint16_t time5000 = 0;

	//___TEST1_H;	
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
	
	control.Callback();
	
	if(++time5000 > 4999)
	{
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		time5000 = 0;
	}
	
	time_100us++;
	if(!(time_100us % 10))
	{
		time_1ms++;
		if(!(time_100us % 100))
		{
			time_10ms++;
			if(!(time_100us % 200))
				time_20ms++;
			if(!(time_100us % 500))
			{
				time_50ms++;
				if(!(time_100us % 1000))
				{
					time_100ms++;
					if(!(time_100us % 2000))
						time_200ms++;
					if(!(time_100us % 5000))
					{
						time_500ms++;
						if(!(time_100us % 10000))
						{
							time_1s++;
						}
					}
				}
			}
		}
	}
	//___TEST1_L;
}

static bool dir_in_flag = true;	//使用内存记录DIR电平
//EN_IN中断
void EXTI0_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	
	if((EN_IN_GPIO_Port->IDR & EN_IN_Pin) != (uint32_t)GPIO_PIN_RESET)
	{	//EN_IN上升沿
		control.control_mode = Control_Mode_Disable;	//失能控制模式
		control.import_mode = Import_Mode_Pulse;		//脉冲输入模式
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);				//失能STEP中断
	}
	else
	{	//EN_IN下降沿
		control.Data_conv_DtP();						//更新脉冲数据
		control.control_mode = Control_Mode_Position;	//位置控制模式
		control.import_mode = Import_Mode_Pulse;		//脉冲输入模式
		HAL_NVIC_EnableIRQ(EXTI2_IRQn);					//使能STEP中断
	}
}

//DIR_IN中断
void EXTI1_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	
	if((DIR_IN_GPIO_Port->IDR & DIR_IN_Pin) != (uint32_t)GPIO_PIN_RESET)
		dir_in_flag = true;
	else
		dir_in_flag = false;
}

//STEP_IN中断
void EXTI2_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	
	if(dir_in_flag == control.dir_pm)
		control.step_count++;
	else
		control.step_count--;
}

//串口接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static uint32_t rx_count = 0;
	
	if(huart->Instance == USART3)
	{
		HAL_UART_Receive_IT(&huart3, (uint8_t*)&usart_it_data, 1);
		Receive_Char(usart_it_data);
		
		rx_count++;
		if(rx_count > 10)
		{
			rx_count = 0;
			HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		}
	}
}
