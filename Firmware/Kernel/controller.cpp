/******
	************************************************************************
	******
	** @file		: controller.cpp/h
	** @brief		: 伺服控制程序
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
	** 	1.控制环路与编码器数据矫正
	** 	2.Control loop and encoder data correction
	************************************************************************
	************************************************************************
******/

#include "controller.h"

//STM32
#include "main.h"
#include "stm32f1xx_hal.h"
//Interaction
#include "usart_txrx.h"
#include "stockpile.h"
//Kernel
#include "as5048.h"
#include "sin_form.h"
//Math
#include <stdlib.h>
#include "arm_math.h"

//引用的外部定义及其引用方式
extern DAC_HandleTypeDef hdac;
extern AS5048 encode;	//引用encode
#define drive_a_Analog(reg)	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, reg);
#define drive_b_Analog(reg)	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, reg);
#define drive_a1_h()	Quick_GPIO_H(DSA1_GPIO_Port, DSA1_Pin)
#define drive_a1_l()	Quick_GPIO_L(DSA1_GPIO_Port, DSA1_Pin)
#define drive_a2_h()	Quick_GPIO_H(DSA2_GPIO_Port, DSA2_Pin)
#define drive_a2_l()	Quick_GPIO_L(DSA2_GPIO_Port, DSA2_Pin)
#define drive_a3_h()	Quick_GPIO_H(DSA3_GPIO_Port, DSA3_Pin)
#define drive_a3_l()	Quick_GPIO_L(DSA3_GPIO_Port, DSA3_Pin)
#define drive_b1_h()	Quick_GPIO_H(DSB1_GPIO_Port, DSB1_Pin)
#define drive_b1_l()	Quick_GPIO_L(DSB1_GPIO_Port, DSB1_Pin)
#define drive_b2_h()	Quick_GPIO_H(DSB2_GPIO_Port, DSB2_Pin)
#define drive_b2_l()	Quick_GPIO_L(DSB2_GPIO_Port, DSB2_Pin)
#define drive_b3_h()	Quick_GPIO_H(DSB3_GPIO_Port, DSB3_Pin)
#define drive_b3_l()	Quick_GPIO_L(DSB3_GPIO_Port, DSB3_Pin)


//矫正数据表Flash位置
float *CTA = (float*)0x08020000;

//
//	PID清空函数
//
void PID_Empty(PID_Typedef *st)
{
	//参数量(置入缺省值)
	st->Kp =  st->Ki =  st->Kd = st->Kq = 0.0f;
	//过程量
	st->OUTp = st->OUTi = st->OUTd = st->OUTq =  0.0f;
	st->error = st->error_l= 0.0f;
	st->overshoot = false;
}

//
//	控制器构造函数
//
Controller::Controller()
{
	//控制参数
	up_spr(200);							//电机步进数据		默认200
	sdb = 32;								//每步细分数		默认32
	dir_pm = true;							//旋转方向			默认正转
	t_pm = 4.72f;							//力矩->电流		默认1.7A->0.36Nm
	drive_module = Drive_Module_Standard;	//驱动模组			默认标准版驱动模组(A4950)
	//iu_pm = 1.0f;		//电流->DAC数据比例	0.1R电阻
	//控制实体
	PID_Empty(&pgv_pid);
	pgv_pid.Kp = 15.0f;
	pgv_pid.Ki = 0.0f;
	pgv_pid.Kd = 0.0f;
	pgv_pid.Kq = 1.0f;
	pgv_pid.OUTq = 0.0f;
	PID_Empty(&p_pid);
	p_pid.Kp = 0.015f;
	p_pid.Ki = 0.0f;//0.025f;
	p_pid.Kd = 0.02f;
	PID_Empty(&v_pid);
	v_pid.Kp = 0.0005f;
	v_pid.Ki = 0.01f;
	v_pid.Kd = 0.0f;
	//滤波数据
	gpvLPF = 10.0f;			//位置速度滤波数据
	gpvLPFa = exp(-2.0f * 3.41459f * gpvLPF / Controller_Freq);
	gpvLPFb = 1.0f - gpvLPFa;
	pLPF = 10.0f;			//位置滤波数据
	pLPFa = exp(-2.0f * 3.41459f * pLPF / Controller_Freq);
	pLPFb = 1.0f - pLPFa;
	vLPF = 10.0f;			//速度滤波数据
	vLPFa = exp(-2.0f * 3.41459f * vLPF / Controller_Freq);
	vLPFb = (1.0f - vLPFa);
	iLPF = 10.0f;			//电流输出滤波数据
	iLPFa = exp(-2.0f * 3.41459f * iLPF / Controller_Freq);
	iLPFb = (1.0f - iLPFa);
	//过程量
	control_mode = Control_Mode_Position;	//控制模式
	import_mode = Import_Mode_Pulse;		//输入模式
	step_count = 0;							//记录输入脉冲数
	circle_count = 0;						//旋转圈数
	up_st = false;							//标准化指令
	//输入输出
	enread = 0;													//编码器数据
	last_sensor_p = 0.0f;										//记录传感器原始
	last_read_p = 0.0f;											//记录读取原始
	last_filter_gpv = last_filter_p = last_filter_v = 0.0f;		//记录滤波数据
	last_goal_p = 0.0f;											//记录目标数据
	sensor_p = 0.0f;											//传感器原始
	read_p = 0.0f;												//读取原始
	filter_gpv = filter_p = filter_v = filter_a = 0.0f;			//滤波数据
	goal_p = goal_v = goal_t = 0.0f;							//目标数据
	out_p = out_i = 0.0f;										//输出数据
	// filter_p = filter_pv = filter_v = filter_a = filter_t = filter_i = 0.0f;	//滤波数据
	// goal_p   = goal_pv   = goal_v   = goal_a   = goal_t   = goal_i   = 0.0f;	//目标数据
	// out_p    = out_pv    = out_v    = out_a    = out_t    = out_i    = 0.0f;	//输出数据
}

//
//	更新spr
//
void Controller::up_spr(uint32_t _spr)
{
	spr = _spr;									//每转步数
	map = (float)(sin3600_num * spr / 360 / 4);	//map = (float)sin3600_num / (360.0f / ((float)spr/4.0f));
	esa = 360.0f / (float)spr;					//每步角度
}

//
//  控制器启动
//
void Controller::Start(void)
{
	PhaseOut_S(0, 0.5f);
	HAL_Delay(200);
	
	uint16_t enread;
	if(!encode.ReadSmoothingEncoderData(&enread))
	    return;
	goal_p = last_sensor_p = sensor_p = CTA[enread];	//启动位置设置为目标位置
	Data_conv_DtP();									//获得起始脉冲数
}

//
//  数据变换PtD
//
void Controller::Data_conv_PtD(void)
{
	goal_p = (float)step_count / (float)(spr * sdb) * 360.0f;
}

//
//  数据变换DtP
//
void Controller::Data_conv_DtP(void)
{
	step_count = goal_p * (float)(spr * sdb) / 360.0f;		
}

//
//	给调试位置速度
//
void Controller::Debug_new_goal(void)
{
	static uint32_t count = 0;
	static uint32_t time = 0;
	static uint32_t cccc = 1;
	if((control_mode == Control_Mode_PosiVelo) || (control_mode == Control_Mode_Position))
	{
		time++;
		if(time >= 4)//cccc)
		{
			time = 0;
			count++;
			if(count <= 3600)
			{
				goal_p = 360.0f * sin3600[count] / sin3600_max;
			}
			else if(count >= 7200)
			{
				count = 0;
			}
		}
	}
	else if(control_mode == Control_Mode_Velocity)
	{
		time++;
		if(time >= cccc)
		{
			time = 0;
			count++;
			if(count >= 3600)
			{
				count = 0;
				cccc *= 2;
				if(cccc >= 8)
					cccc = 1;
			}
			goal_v = 2000.0f * sin3600[count] / sin3600_max;
		}
	}
	import_mode = Import_Mode_Digital;
}

//
//	控制回调函数
//
void Controller::Callback(void)
{	
	//获取脉冲目标
	if(Import_Mode_Pulse == import_mode)
	{
		Data_conv_PtD();
	}
	//Debug_new_goal();
	
	//读取编码器数据/角度回环
	uint16_t enread;
	if(!encode.ReadEncoderData(&enread))
		return;
	//读取角度
	last_sensor_p = sensor_p;
	sensor_p = CTA[enread];
	//回环
	if((sensor_p - last_sensor_p) > 180.0f)			circle_count -= 1;
	else if((sensor_p - last_sensor_p) < -180.0f)	circle_count += 1;
	last_read_p = read_p;
	read_p = sensor_p + 360.0f * (float)circle_count;

	//滤波
	//滤波估算的目标速度
	last_filter_gpv	= filter_gpv;
	filter_gpv = gpvLPFa * filter_gpv + gpvLPFb * (goal_p - last_goal_p) * Controller_Freq;
	//滤波位置
	last_filter_p	= filter_p;
	filter_p = pLPFa * filter_p + pLPFb * read_p;
	//滤波速度
	last_filter_v	= filter_v;
	filter_v = vLPFa * filter_v + vLPFb * (read_p - last_read_p) * Controller_Freq;
	//滤波加速度
	filter_a = (filter_v - last_filter_v) * Controller_Freq;
	
	//控制环
	if(Control_Mode_PosiVelo == control_mode)//二阶角位置模式(位置部分)
	{
		pgv_pid.error_l = pgv_pid.error;
		pgv_pid.error = (goal_p - read_p);
		if(pgv_pid.error > 360.0f)			pgv_pid.error = 360.0f;
		else if(pgv_pid.error < -360.0f)		pgv_pid.error = -360.0f;
		pgv_pid.OUTp = pgv_pid.Kp * pgv_pid.error;							//Kp
		pgv_pid.OUTi += pgv_pid.Ki * pgv_pid.error / Controller_Freq;		//Ki
		pgv_pid.OUTd = pgv_pid.Kd * (pgv_pid.error - pgv_pid.error_l);		//Kd
		pgv_pid.OUTq = pgv_pid.Kq * filter_gpv;								//Kq
		goal_v = pgv_pid.OUTp + pgv_pid.OUTi + pgv_pid.OUTd + pgv_pid.OUTq;
	}

	if(Control_Mode_Position == control_mode)//角位置模式
	{
		p_pid.error_l = p_pid.error;
		p_pid.error = (goal_p - read_p);
		if(p_pid.error > 360.0f)		p_pid.error = 360.0f;
		else if(p_pid.error < -360.0f)	p_pid.error = -360.0f;
		p_pid.OUTp = p_pid.Kp * p_pid.error;								//Kp
		p_pid.OUTi += p_pid.Ki * p_pid.error / Controller_Freq;				//Ki
		p_pid.OUTd = p_pid.Kd * (p_pid.error - p_pid.error_l);				//Kd
		if(p_pid.OUTi > 0.3f)			p_pid.OUTi = 0.3f;
		else if(p_pid.OUTi < -0.3f)		p_pid.OUTi = -0.3f;
		goal_t = p_pid.OUTp + p_pid.OUTi + p_pid.OUTd;
	}

	if((Control_Mode_Velocity == control_mode) || (Control_Mode_PosiVelo == control_mode))//角速度模式
	{
		v_pid.error_l = v_pid.error;
		v_pid.error = (goal_v - filter_v);
		// 积分分离 (某些应用不能使用分离)
		if((v_pid.overshoot) && (v_pid.error_l * v_pid.error <= 0.0f))
		{
			v_pid.OUTi = 0.0f;
			v_pid.overshoot = false;
		}
		v_pid.OUTp = v_pid.Kp * v_pid.error;							//Kp
		v_pid.OUTi += v_pid.Ki * v_pid.error / Controller_Freq;			//Ki
		v_pid.OUTd = -v_pid.Kd * filter_a;								//Kd
		if(v_pid.OUTi > 0.3f)			{v_pid.OUTi = 0.3f;v_pid.overshoot = true;}
		else if(v_pid.OUTi < -0.3f)		{v_pid.OUTi = -0.3f;v_pid.overshoot = true;}
		goal_t = (v_pid.OUTp + v_pid.OUTi + v_pid.OUTd);
	}

	if(Control_Mode_Torque == control_mode)//力矩模式
	{
		// goal_t = goal_t;
	}
	
	if(Control_Mode_Disable == control_mode)//失能模式
	{
		PhaseOut_Stop();
		return;
	}
	
	//输出电流
	out_i = t_pm * goal_t;
	if(out_i > 15.0f)    out_i = 15.0f;
	if(out_i < -15.0f)   out_i = -15.0f;

	//相位控制
	if(out_i > 0.0f)		out_p = sensor_p + esa;		//叠加超前角
	else if(out_i < 0.0f)	out_p = sensor_p - esa;		//叠减超前角
	if(out_p >= 360.0f)		out_p -= 360.0f;
	else if(out_p < 0.0f)	out_p += 360.0f;
	
	//相位输出
	PhaseOut_A(out_p, fabs(out_i));

	//保存数据
	last_goal_p		= goal_p;
}

//
//	标准化
//
//#define REMA(a,b)	((a+b)%b)
uint32_t REMA(uint32_t a,uint32_t b)
{
	return (a+b)%b;
}

//
//	标准化
//
bool Controller::Standardizing(void)
{
	uint16_t EncodeData[401];
	uint32_t count;
	int32_t	sub_data;                     //差值
	int32_t Bstep, Sstep, rcd_x, rcd_y;   //过程量
	float angle;                          //输出角
	bool dir;                             //数据方向

	//Go
	PhaseOut_S(0, 0.5f);
	HAL_Delay(1000);
	if(!encode.ReadSmoothingEncoderData(&EncodeData[0]))
		return false;

	//自动寻找并读取电机一周数据
	const uint32_t TSN_NUM = 3;
	const uint32_t TSN[TSN_NUM] = {100, 200, 400};
	//const float TSA[TSN_NUM] = {3.6f, 1.8f, 0.9f};
	Bstep=1;
	for(count=0;;)
	{
		for(; Bstep<TSN[count]+1; Bstep++)
		{
			PhaseOut_S(Bstep, 0.5f);
			HAL_Delay(250);
			if(!encode.ReadSmoothingEncoderData(&EncodeData[Bstep]))    //读取编码器滤波数据
			{
				PhaseOut_S(0, 0.0f);
				return false;
			}
		}
		sub_data = EncodeData[Bstep-1] - EncodeData[0];
		if(sub_data > AS5048_DPI/2)			sub_data -= AS5048_DPI;
		else if(sub_data < -AS5048_DPI/2)	sub_data += AS5048_DPI;
		if((sub_data > -AS5048_DPI/(Bstep-1)) && (sub_data < AS5048_DPI/(Bstep-1)))
		{
			spr = TSN[count];
			map = (float)(sin3600_num * spr / 360 / 4);
			esa = 360.0f / (float)spr;		//esa = TSA[count];
			break;
		}
		if(++count >= TSN_NUM)
		{
			PhaseOut_S(0, 0.0f);
			return false;
		}
	}
	PhaseOut_S(0, 0.0f);

	//检查数据方向
	sub_data = EncodeData[1] - EncodeData[0];
	if(sub_data > AS5048_DPI/2)			sub_data -= AS5048_DPI;
	else if(sub_data < -AS5048_DPI/2)	sub_data += AS5048_DPI;
	if(sub_data == 0)		return false;
	else if(sub_data > 0)	dir = true;
	else					dir = false;
	for(Bstep=1; Bstep<spr; Bstep++)
	{
	    sub_data = EncodeData[REMA(Bstep+1, spr)] - EncodeData[REMA(Bstep, spr)];
	    if(sub_data > AS5048_DPI/2)			sub_data -= AS5048_DPI;
	    else if(sub_data < -AS5048_DPI/2)	sub_data += AS5048_DPI;
	    if(sub_data == 0)				return false;
	    else if((sub_data > 0) && !dir)	return false;
	    else if((sub_data < 0) && dir)	return false;
	}

	//检查边沿
	if(dir)
	{
	    count = 0;
	    for(Bstep=0; Bstep<spr; Bstep++)
	    {
	        sub_data = EncodeData[REMA(Bstep+1, spr)] - EncodeData[REMA(Bstep, spr)];
	        if(sub_data < 0)
	        {       
	            count++;
	            rcd_x = Bstep;//使用区间前标
	            rcd_y = AS5048_DPI-1 - EncodeData[REMA(Bstep, spr)];
	        }
	    }
	    if(1 != count)
	        return false;
	}
	else 
	{
	    count = 0;
	    for(Bstep=0; Bstep<spr; Bstep++)
	    {
	        sub_data = EncodeData[REMA(Bstep+1, spr)] - EncodeData[REMA(Bstep, spr)];
	        if(sub_data > 0)
	        {       
	            count++;
	            rcd_x = Bstep+1;//使用区间后标
	            rcd_y = AS5048_DPI-1 - EncodeData[REMA(Bstep+1, spr)];
	        }
	    }
	    if(1 != count)
	        return false;
	}

	//运算校准数据并写入Flash
	CTA_Empty();
	CTA_Start();
	if(dir)
	{
		for(Bstep=rcd_x; Bstep<rcd_x+spr+1; Bstep++)
		{
			sub_data = EncodeData[REMA(Bstep+1, spr)] - EncodeData[REMA(Bstep, spr)];
			if(sub_data < 0)        sub_data += AS5048_DPI;
			//
			if(Bstep == rcd_x)//开始边缘
			{
				for(Sstep=rcd_y; Sstep<sub_data; Sstep++)
				{
					angle = esa * 0.001f * (float)REMA((1000*Bstep+1000*Sstep/sub_data), 1000*spr);
					CTA_Write(*(uint32_t*)&angle);
				}
			}
			else if(Bstep == rcd_x+spr)//结束边缘
			{
				for(Sstep=0; Sstep<rcd_y; Sstep++)
				{
					angle = esa * 0.001f * (float)REMA((1000*Bstep+1000*Sstep/sub_data), 1000*spr);
					CTA_Write(*(uint32_t*)&angle);
				}
			}
			else//中间
			{
				for(Sstep=0; Sstep<sub_data; Sstep++)
				{
					angle = esa * 0.001f * (float)REMA((1000*Bstep+1000*Sstep/sub_data), 1000*spr);
					CTA_Write(*(uint32_t*)&angle);
				}
			}
		}
	}
	else
	{
		for(Bstep=rcd_x+spr; Bstep>rcd_x-1; Bstep--)
		{
			sub_data = EncodeData[REMA(Bstep, spr)] - EncodeData[REMA(Bstep+1, spr)];
			if(sub_data < 0)        sub_data += AS5048_DPI;
			//
			if(Bstep == rcd_x+spr)//开始边缘
			{
				for(Sstep=rcd_y; Sstep<sub_data; Sstep++)
				{
					angle = esa * 0.001f * (float)REMA(1000*Bstep-1000*Sstep/sub_data, 1000*spr);
					CTA_Write(*(uint32_t*)&angle);
				}
			}
			else if(Bstep == rcd_x)//结束边缘
			{
				for(Sstep=0; Sstep<rcd_y; Sstep++)
				{
					angle = esa * 0.001f * (float)REMA(1000*Bstep-1000*Sstep/sub_data, 1000*spr);
					CTA_Write(*(uint32_t*)&angle);
				}
			}
			else//中间
			{
				for(Sstep=0; Sstep<sub_data; Sstep++)
				{
					angle = esa * 0.001f * (float)REMA(1000*Bstep-1000*Sstep/sub_data, 1000*spr);
					CTA_Write(*(uint32_t*)&angle);
				}
			}
		}
	}
	CTA_End();

	return true;
}

//
//	相位输出停止
//
void Controller::PhaseOut_Stop(void)
{
	drive_a_Analog(0);
	drive_b_Analog(0);
	
	if(drive_module == Drive_Module_Standard)
	{
		//标准版驱动模组
		drive_a2_l();
		drive_a3_l();
		drive_b2_l();
		drive_b3_l();
	}
	else if(drive_module == Drive_Module_Power)
	{
		//动力版驱动模组
		drive_a1_l();
		drive_a2_l();
		drive_a3_l();
		drive_b1_l();
		drive_b2_l();
		drive_b3_l();
	}
}

//
//	相位输出
//
void Controller::PhaseOut_A(float a, float i)
{
	uint32_t angle_a, angle_b;
	int32_t sin_a, sin_b;
	int32_t sin_a_abs, sin_b_abs;
	float u;
	uint32_t reg;
	uint32_t reg_a, reg_b;
	//获得比例
	angle_a = (uint32_t)(map * a) % sin3600_num;
	angle_b = (angle_a + sin3600_num/4) % sin3600_num;
	sin_a = sin3600[angle_a];
	sin_b = sin3600[angle_b];
	sin_a_abs = abs(sin_a);
	sin_b_abs = abs(sin_b);	
	//	arm_abs_q31(&sin_a, &sin_a_abs, 1);
	//	arm_abs_q31(&sin_b, &sin_b_abs, 1);
	//获得电压
	if(drive_module == Drive_Module_Standard)
		u = i;
	else if(drive_module == Drive_Module_Power)
		u = i * 0.2f;
	if(u > 3.3f)	u = 3.3f;
	reg = (uint32_t)(u / 3.3f * 4095.0f);
	if(reg > 4095)	reg = 4095;
	reg_a = reg * sin_a_abs / sin3600_max;
	reg_b = reg * sin_b_abs / sin3600_max;
	//Out
	drive_a_Analog(reg_a);
	drive_b_Analog(reg_b);
	
	if(drive_module == Drive_Module_Standard)
	{
		//标准版驱动模组
		if(sin_a > 0)		{drive_a3_h(); drive_a2_l();}
		else if(sin_a < 0)	{drive_a3_l(); drive_a2_h();}
		else				{drive_a3_h(); drive_a2_h();}
		if(sin_b > 0)		{drive_b3_h(); drive_b2_l();}
		else if(sin_b < 0)	{drive_b3_l(); drive_b2_h();}
		else				{drive_b3_h(); drive_b2_h();}
	}
	else if(drive_module == Drive_Module_Power)
	{
		//动力版驱动模组
		if(sin_a > 0)		{drive_a1_h(); drive_a2_h(); drive_a3_h();}
		else if(sin_a < 0)	{drive_a1_h(); drive_a2_h(); drive_a3_l();}
		else				{drive_a1_h(); drive_a2_l(); drive_a3_l();}
		if(sin_b > 0)		{drive_b1_h(); drive_b2_h(); drive_b3_h();}
		else if(sin_b < 0)	{drive_b1_h(); drive_b2_h(); drive_b3_l();}
		else				{drive_b1_h(); drive_b2_l(); drive_b3_l();}
	
	}
}

//
//	相位输出
//
void Controller::PhaseOut_S(uint32_t s, float i)
{
	uint32_t angle_a, angle_b;
	int32_t sin_a, sin_b;
	int32_t sin_a_abs, sin_b_abs;
	float u;
	uint32_t reg;
	uint32_t reg_a, reg_b;
	//获得比例
	angle_a = (uint32_t)(s) % sin4_num;
	angle_b = (angle_a + sin4_num/4) % sin4_num;
	sin_a = sin4[angle_a];
	sin_b = sin4[angle_b];
	sin_a_abs = abs(sin_a);
	sin_b_abs = abs(sin_b);
	//	arm_abs_q31(&sin_a, &sin_a_abs, 1);
	//	arm_abs_q31(&sin_b, &sin_b_abs, 1);
	//获得电压
	if(drive_module == Drive_Module_Standard)
		u = i;
	else if(drive_module == Drive_Module_Power)
		u = i * 0.2f;
	reg = (uint16_t)(u / 3.3f * 4095.0f);
	if(reg > 4095)	reg = 4095;
	reg_a = reg * sin_a_abs / sin4_max;
	reg_b = reg * sin_b_abs / sin4_max;
	//Out
	drive_a_Analog(reg_a);
	drive_b_Analog(reg_b);
	if(drive_module == Drive_Module_Standard)
	{
		//标准版驱动模组
		if(sin_a > 0)		{drive_a3_h(); drive_a2_l();}
		else if(sin_a < 0)	{drive_a3_l(); drive_a2_h();}
		else				{drive_a3_h(); drive_a2_h();}
		if(sin_b > 0)		{drive_b3_h(); drive_b2_l();}
		else if(sin_b < 0)	{drive_b3_l(); drive_b2_h();}
		else				{drive_b3_h(); drive_b2_h();}
	}
	else if(drive_module == Drive_Module_Power)
	{
		//动力版驱动模组
		if(sin_a > 0)		{drive_a1_h(); drive_a2_h(); drive_a3_h();}
		else if(sin_a < 0)	{drive_a1_h(); drive_a2_h(); drive_a3_l();}
		else				{drive_a1_h(); drive_a2_l(); drive_a3_l();}
		if(sin_b > 0)		{drive_b1_h(); drive_b2_h(); drive_b3_h();}
		else if(sin_b < 0)	{drive_b1_h(); drive_b2_h(); drive_b3_l();}
		else				{drive_b1_h(); drive_b2_l(); drive_b3_l();}
	}
}

