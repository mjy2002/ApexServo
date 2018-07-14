/******
	************************************************************************
	******
	** @file		: usart_txrt.cpp/h
	** @brief		: 串口发送接收
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
	** 	1.串口发送接收
	** 	2.Serial port sending and receiving
	************************************************************************
	************************************************************************
******/

#ifndef USART_TXRX_H
#define USART_TXRX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>		//数据标准规范
#include <stdbool.h>

//C语言基本库
#include <stdio.h>		//基本输入输出
#include <stdlib.h>		//基础库
#include <string.h>		//字符串

/*********************************************************************/
/**************************    公开数据    ***************************/
/*********************************************************************/
#include "stm32f1xx_hal.h"
extern uint16_t uart_tx_size;
extern char uart_tx_buff[129];
extern char uart_rx_buff[129];
	
/*********************************************************************/
/****************************    发送    ******************************/
/*********************************************************************/
void SerialPort_Send_0x32(uint32_t data);
void SerialPort_Send_float(float data);
void SerialPort_Send_string(char *spr);
void SerialPort_Send_TX(void);					//直接发送tx_buff
bool SerialPort_Refer_TX_Leisure(void);		//查询发送是否空闲

/*********************************************************************/
/****************************    接收    ******************************/
/*********************************************************************/
#define Receive_End_RandN	0x00U					//使用R&N结尾
#define	Receive_End_RorN	0x01U					//使用R/N结尾
#define	Receive_End			Receive_End_RorN
#define Standard_USART_Receive_Buff_Max		64		//接收缓冲区大小

//接收过程状态枚举
typedef enum{
#if (Receive_End == Receive_End_RandN)
	State_New = 0x00U,
	State_Receive,
	State_End_0x0A,
	State_Error_0x0D,
	State_Error_0x0A,
	State_Error,
#elif (Receive_End == Receive_End_RorN)
	State_New = 0x00U,
	State_Receive,
	State_Error,
#endif
}State_Typedef;

void Receive_Char(char data);						//接收一个字节
void Receive_String(char *data, uint32_t number);	//接收一个字符串
void Receive_Write_Loop(void);						//写环函数

#ifdef __cplusplus
}
#endif

#endif

