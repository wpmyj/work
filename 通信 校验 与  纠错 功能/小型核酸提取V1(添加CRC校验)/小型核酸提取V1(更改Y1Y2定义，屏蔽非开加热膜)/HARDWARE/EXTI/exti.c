#include "exti.h"
#include "delay.h" 
#include "pwm.h"
#include "time.h"
#include "moto.h"
#include "usart.h"
#include "DeviceVibration.h"
//#include "DeviceMagnetic.h"


u32 AB_cnt=0;
u16 Current_zcnt=0;
u16 Z_cnt =0;
extern u8 Flag_run;
//#include "beep.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 



//外部中断2服务程序
void EXTI2_IRQHandler(void)
{ 
//	u16 i;
//	for(i=1000; i>0; i--);   //消抖
	delay_ms(10);
	if(XR)
	{		
			TIM_Cmd(TIM1, DISABLE);	
      X0RSTOV = 1;					
			EXTI->IMR&=0xfffb; 				
	}	
	EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE2上的中断标志位 
}

//外部中断3服务程序  
extern u8  Flag_run;
void EXTI3_IRQHandler(void)
{	
//  u16 i;
//	for(i=1000; i>0; i--);   //消抖
	delay_ms(10);
	if(Y0R)
	{			  
			TIM_Cmd(TIM3, DISABLE);
			Y1RSTOV = 1;			 
			EXTI->IMR&=0xfff7; 
//      if(Flag_run == 4)	
//			{
//					BELL=1;
//				  delay_ms(80);
//				  BELL=0;
//			}		
  }
  EXTI_ClearITPendingBit(EXTI_Line3);  //清除LINE3上的中断标志位   	
}

//外部中断4服务程序
void EXTI4_IRQHandler(void)
{
//	u16 i;
//	for(i=1000; i>0; i--);   //消抖
	delay_ms(10);
	if(Y1L)
	{	
			TIM_Cmd(TIM8, DISABLE);
			Y2RSTOV = 1;		 
			EXTI->IMR&=0xffef;		
//      if(Flag_run == 4)	
//			{
//					BELL=1;
//				  delay_ms(80);
//				  BELL=0;
//			}		
	} 	
	EXTI_ClearITPendingBit(EXTI_Line4);//清除LINE4上的中断标志位  
}


u8 TIM1_enable_door = 0;   //记录，关闭所有的定时器前，他们开启|关闭的状态
u8 TIM3_enable_door = 0;
u8 TIM8_enable_door = 0;
u8 suspend_vib_door;
extern u8 Flag_run;

extern u8 TIM1_enable_usart;
extern u8 TIM3_enable_usart;
extern u8 TIM8_enable_usart;
extern char SendBackBuff[20];
void EXTI9_5_IRQHandler(void)
{
	static u8 door_open=0;           //记录外部中断前的状态，打开了=1， 关闭=0	
//	u16 i;
//	for(i=600; i>0; i--);   //消抖
	delay_ms(10);
	if(DOOR == 1) 		 
	{
		if(Flag_run == 1)
		{			
//			Send_Str(":dialogbox=5!\r\n");
				Package("dialogbox","5",SendBackBuff);
			  Send_Str(SendBackBuff);
			//Send_Str(":door=1\r\n");
		}
			
		if((Flag_run == 4))                   //(Run_flag == 4,标志仪器正在运行)如果程序处于运行态，则发送，“运行中门打开”，并做“暂停”操作
		{			
//			Send_Str(":dialogbox=5!\r\n");
				Package("dialogbox","5",SendBackBuff);
				Send_Str(SendBackBuff);
		//	Send_Str(":door=1\r\n");
			Flag_run = 1;
    
			if(((TIM1->CR1 & TIM_CR1_CEN) == 1) )                   //判断关闭所有的定时器前，TIM1是否开启
				{
					TIM_Cmd(TIM1,DISABLE);											//TIM1_enable_door系列变量，标记，收到暂停命令前，各个定时器的开启、关闭状态
					TIM1_enable_door = 1;
				}
				
			if(((TIM3->CR1 & TIM_CR1_CEN) == 1))
				{
					if(Vib == 1)                                     //判断是否处于振动状态，如果处于振动状态，则向震动函数发送暂停标记
					{
							suspend_vib_door = 1;		
					}	
					else
					{
							TIM_Cmd(TIM3,DISABLE);								
					}			
					TIM3_enable_door = 1;							
				}
				if(((TIM8->CR1 & TIM_CR1_CEN) == 1) )
				{
					TIM_Cmd(TIM8,DISABLE);
					TIM8_enable_door = 1;
				}				
		}		
		
	}			
		
//	if(DOOR == 0)
//	{
//		door_open = 0;
//		Send_Str(":door=0\r\n");
//	}	
	EXTI_ClearITPendingBit(EXTI_Line5);//清除LINE5上的中断标志位  
}


	   
//外部中断初始化程序
//初始化PE2~4,PA0为中断输入.

//PD0  PD1


void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	//KEY_Init(); //按键对应的IO口初始化
  
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5; //KEY0 KEY1 KEY2对应引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE2,3,4

	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);//PE2 连接到中断线2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);//PE3 连接到中断线3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);//PE4 连接到中断线4
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource5);//PE5连接到中断线5
	
	/* 配置EXTI_Line2,3,4 */
	EXTI_InitStructure.EXTI_Line =  EXTI_Line2 | EXTI_Line3|EXTI_Line4|EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //笛卮シ�
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);//配置
	
		
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//外部中断4
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//外部中断5
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	   
}


void InitSwitch()
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);//??GPIOF??

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//??????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOD, &GPIO_InitStructure);//???
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//??????
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//????
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//??
  GPIO_Init(GPIOE, &GPIO_InitStructure);//???	

	Switch0 =1;
	Switch1 =1;
	Switch2 =1;
	Switch3 =1;
	Switch4 =1;
	Switch5 =1;
	Switch6 =1;
	Switch7 =1;
	
	BELL=0;
	UV =0;
	BAK =1;  
	FUN  =0;	
}









