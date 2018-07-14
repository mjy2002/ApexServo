/******
	************************************************************************
	******
	** @file		: IS.c/h
	** @brief		: 指令识别
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
	** 	1.识别接收数据包含的指令
	** 	2.Identify the instructions contained in the receiving data
	************************************************************************
	************************************************************************
******/

#include "IS.h"

//调用串口收发
#include "usart_txrx.h"
#include "stockpile.h"
//引用控制器
#include "controller.h"

extern Controller control;

char is_buff[129];

/**************************************		通用工具		****************************************/
typedef enum{
	Seek_Number_Genre_NULL = 0x00U,				//未识别到数
	Seek_Number_Genre_int,						//识别到整形数
	Seek_Number_Genre_float,					//识别到浮点数
}Seek_Number_Genre;

int32_t	seek_number_int32;						//搜索数据 int
float	seek_number_float;						//搜索数据 float

/*
**	GM数据搜索
*/
Seek_Number_Genre GMcode_Seek_Number(char *str)
{
	uint16_t number_length;
	char number[30];
	char *cutp;
	
	//移动到数字位置
	cutp = strpbrk(str, "1234567890+-.");
	if(cutp == NULL)
		return Seek_Number_Genre_NULL;
	//数字拷出
	number_length = strspn(cutp, "1234567890+-.");
	if((!number_length) || (number_length > 29))
		return Seek_Number_Genre_NULL;
	//数字识别
	strncpy(number, cutp, number_length);
	number[number_length] = NULL;
	cutp = strchr(number, '.');
	if(cutp == NULL)
	{
		seek_number_int32 = atoi(number);
		return Seek_Number_Genre_int;
	}
	else
	{
		seek_number_float = atof(number);
		return Seek_Number_Genre_float;		
	}
}
/**************************************		通用工具		****************************************/
//
//	IS指令系统输入
//
void IS_IN(char *str)
{
	char *cutp;
	uint8_t flag;

	/***********************************   pgv_pid   ***********************************/
	//pgvkp
	cutp = strstr(str, "pgvkp");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.pgv_pid.Kp = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.pgv_pid.Kp = (float)seek_number_float;}
		return;
	}	
	//pgvki
	cutp = strstr(str, "pgvki");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.pgv_pid.Ki = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.pgv_pid.Ki = (float)seek_number_float;}
		return;
	}	
	//pgvkd
	cutp = strstr(str, "pgvkd");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.pgv_pid.Kd = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.pgv_pid.Kd = (float)seek_number_float;}
		return;
	}	
	//pgvkq
	cutp = strstr(str, "pgvkq");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.pgv_pid.Kq = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.pgv_pid.Kq = (float)seek_number_float;}
		return;
	}


	/***********************************   p_pid   ***********************************/
	//pkp
	cutp = strstr(str, "pkp");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.p_pid.Kp = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.p_pid.Kp = (float)seek_number_float;}
		return;
	}	
	//pki
	cutp = strstr(str, "pki");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.p_pid.Ki = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.p_pid.Ki = (float)seek_number_float;}
		return;
	}	
	//pkd
	cutp = strstr(str, "pkd");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.p_pid.Kd = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.p_pid.Kd = (float)seek_number_float;}
		return;
	}
	//pkq
	cutp = strstr(str, "pkq");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.p_pid.Kq = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.p_pid.Kq = (float)seek_number_float;}
		return;
	}


	/***********************************   v_pid   ***********************************/
	//vkp
	cutp = strstr(str, "vkp");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.v_pid.Kp = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.v_pid.Kp = (float)seek_number_float;}
		return;
	}	
	//vki
	cutp = strstr(str, "vki");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.v_pid.Ki = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.v_pid.Ki = (float)seek_number_float;}
		return;
	}	
	//vkd
	cutp = strstr(str, "vkd");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.v_pid.Kd = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.v_pid.Kd = (float)seek_number_float;}
		return;
	}
	//vkq
	cutp = strstr(str, "vkq");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.v_pid.Kq = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.v_pid.Kq = (float)seek_number_float;}
		return;
	}

	/***********************************   模式目标   ***********************************/
	//go_pgv
	cutp = strstr(str, "go_pgv");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		control.control_mode = Control_Mode_PosiVelo;
		if(flag == Seek_Number_Genre_int){
			control.control_mode = Control_Mode_PosiVelo;	//二阶角位置模式
			control.import_mode = Import_Mode_Digital;		//数字输入模式
			control.goal_p = (float)seek_number_int32;
		}
		else if(flag == Seek_Number_Genre_float){
			control.control_mode = Control_Mode_PosiVelo;
			control.import_mode = Import_Mode_Digital;
			control.goal_p = (float)seek_number_float;
		}
		return;
	}	
	//go_p
	cutp = strstr(str, "go_p");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int){
			control.control_mode = Control_Mode_Position;
			control.import_mode = Import_Mode_Digital;
			control.goal_p = (float)seek_number_int32;
		}
		else if(flag == Seek_Number_Genre_float){
			control.control_mode = Control_Mode_Position;
			control.import_mode = Import_Mode_Digital;
			control.goal_p = (float)seek_number_float;
		}
		return;
	}	
	//go_v
	cutp = strstr(str, "go_v");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int){
			control.control_mode = Control_Mode_Velocity;
			control.import_mode = Import_Mode_Digital;
			control.goal_v = (float)seek_number_int32;
		}
		else if(flag == Seek_Number_Genre_float){
			control.control_mode = Control_Mode_Velocity;
			control.import_mode = Import_Mode_Digital;
			control.goal_v = (float)seek_number_float;
		}
		return;
	}	
	//go_t
	cutp = strstr(str, "go_t");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int){
			control.control_mode = Control_Mode_Torque;
			control.import_mode = Import_Mode_Digital;
			control.goal_t = (float)seek_number_int32;
		}
		else if(flag == Seek_Number_Genre_float){
			control.control_mode = Control_Mode_Torque;
			control.import_mode = Import_Mode_Digital;
			control.goal_t = (float)seek_number_float;
		}
		return;
	}

	/***********************************   控制参数   ***********************************/
	//spr
	//spr禁止通过指令修改,必须使用标准化函数自动识别并储存
	//sdb
	cutp = strstr(str, "sdb");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.sdb = (uint32_t)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.sdb = (uint32_t)seek_number_float;}
		return;
	}
	//dir_pm
	cutp = strstr(str, "dir_pm");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.dir_pm = (bool)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.dir_pm = (bool)seek_number_float;}
		return;
	}
	//t_pm
	cutp = strstr(str, "t_pm");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.t_pm = (float)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.t_pm = (float)seek_number_float;}
		return;
	}	
	//drive_module
	cutp = strstr(str, "drive_module");
	if(cutp !=NULL){
		flag = GMcode_Seek_Number(cutp);
		if(flag == Seek_Number_Genre_int)			{control.drive_module = (uint32_t)seek_number_int32;}
		else if(flag == Seek_Number_Genre_float)	{control.drive_module = (uint32_t)seek_number_float;}
		return;
	}	

	/***********************************   控制   ***********************************/
	//up_st
	cutp = strstr(str, "up_st");
	if(cutp !=NULL){
		control.up_st = true;
		SerialPort_Send_string("up_st...\r\n");
		return;
	}	
	//disable
	cutp = strstr(str, "disable");
	if(cutp !=NULL){
		control.control_mode = Control_Mode_Disable;
		SerialPort_Send_string("disable\r\n");
		return;
	}
	//save
	cutp = strstr(str, "save");
	if(cutp !=NULL){
		Store_RAM_To_Flash(true);
		SerialPort_Send_string("save\r\n");
		return;
	}
	//display
	cutp = strstr(str, "display");
	if(cutp !=NULL){
		while(!SerialPort_Refer_TX_Leisure());	sprintf(uart_tx_buff, "pgvkp:%11.4f   pgvki:%11.4f  pgvkd:%11.4f  pgvkq:%11.4f\r\n",    control.pgv_pid.Kp, control.pgv_pid.Ki, control.pgv_pid.Kd, control.pgv_pid.Kq);	SerialPort_Send_TX();
		while(!SerialPort_Refer_TX_Leisure());	sprintf(uart_tx_buff, "pkp:%11.4f     pki:%11.4f    pkd:%11.4f    pkq:%11.4f\r\n",      control.p_pid.Kp,   control.p_pid.Ki,   control.p_pid.Kd,   control.p_pid.Kq);		SerialPort_Send_TX();
		while(!SerialPort_Refer_TX_Leisure());	sprintf(uart_tx_buff, "vkp:%11.4f     vki:%11.4f    vkd:%11.4f    vkq:%11.4f\r\n",      control.v_pid.Kp,   control.v_pid.Ki,   control.v_pid.Kd,   control.v_pid.Kq);		SerialPort_Send_TX();
		while(!SerialPort_Refer_TX_Leisure());	sprintf(uart_tx_buff, "go_pgv:%11.4f  go_p:%11.4f   go_v:%11.4f   go_t:%11.4f\r\n",     control.goal_p,     control.goal_p,     control.goal_v,     control.goal_t);		SerialPort_Send_TX();
		while(!SerialPort_Refer_TX_Leisure());	sprintf(uart_tx_buff, "sdb:%11d       dir_pm:%11d,  t_pm:%11.4f   drive_module:%d\r\n", control.sdb,        control.dir_pm,     control.t_pm,       control.drive_module);	SerialPort_Send_TX();
		return;
	}

	/***********************************   读取   ***********************************/
	cutp = strstr(str, "get");
	if(cutp !=NULL){
		while(!SerialPort_Refer_TX_Leisure());	sprintf(uart_tx_buff, "p:%11.4f  v:%11.4f\r\n", control.filter_p, control.filter_v);	SerialPort_Send_TX();
		return;
	}

	/***********************************   帮助   ***********************************/
	cutp = strstr(str, "help");
	if(cutp !=NULL){
		Store_RAM_To_Flash(true);
		SerialPort_Send_string("help\r\n");
		return;
	}
}

//
//
//
void IS_Discern(void)
{
	if(IS_Read_Loop(is_buff))
		IS_IN(is_buff);
}


/************************************************************************************************************/
/******************************************        缓冲环       **********************************************/
/************************************************************************************************************/
#define IS_Loop_Number	3
uint8_t loop_read_pointer = 0;
uint8_t loop_write_pointer = 0;								//环读写指针(解决中断读写冲突)
uint32_t loop_read_count = 0;
uint32_t loop_write_count = 0;								//环读写计数(解决中断读写冲突)
char	loop_buff[IS_Loop_Number][129];	//缓冲环

/******************************************************
**  写环函数
******************************************************/
bool IS_Write_Loop(char *data)
{
	//检查是否处于满缓冲状态
	if((loop_read_pointer == loop_write_pointer) && (loop_read_count != loop_write_count))
	{//缓冲区满了
		return false;
	}

	strcpy(loop_buff[loop_write_pointer], data);

	loop_write_pointer++;
	loop_write_count++;
	if(loop_write_pointer >= IS_Loop_Number)
		loop_write_pointer = 0;
	return true;
}

/******************************************************
**  读环函数
******************************************************/
bool IS_Read_Loop(char *data)
{
	//检查是否处于空缓冲状态
	if((loop_read_pointer == loop_write_pointer) && (loop_read_count == loop_write_count))
	{//缓冲区空了
		return false;
	}

	strcpy(data, loop_buff[loop_read_pointer]);

	loop_read_pointer++;
	loop_read_count++;
	if(loop_read_pointer >= IS_Loop_Number)
		loop_read_pointer = 0;
	return true;
}


/************************************************************************************************************/
/************************************************************************************************************/
/************************************************************************************************************/



