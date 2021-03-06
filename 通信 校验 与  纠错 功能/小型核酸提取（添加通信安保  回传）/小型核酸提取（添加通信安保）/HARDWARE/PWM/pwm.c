#include "pwm.h"
#include "usart.h"
#include "moto.h"

 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器PWM 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
////tim3 ch1  tim8 ch2
//PC6 PC7  PA8


u32 PWMX_Num=0;    //3200  4mm;
u32 PWMY1_Num=0;  
u32 PWMY2_Num=0;  // 1600  40.6MM;   40 1mm;


void TIM1_GPIO_Config()
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTF时钟	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); //GPIOF9复用为定时器14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);


  GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8); //GPIOF9复用为定时器8 C 1
 	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3); //GPIOF9复用为定时器3 CH2
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);


	//初始化PF9
	
//	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;           //GPIOF9  //DIR AND ENABLE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);              //初始化Pb6 pb7 
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;           //GPIOF9  //DIR AND ENABLE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure);              //初始化Pb6 pb7 
	

	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;           //GPIOF9  //DIR AND ENABLE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //初始化Pb6 pb7 
	
	
}
void TIM1_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	//TIM14时钟使能    

	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载�
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//初始化定时器14
	
	TIM_OCInitStructure.TIM_Pulse = arr/3;
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR1上的预装载寄存器
 //	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  TIM_ARRPreloadConfig(TIM1,ENABLE);//ARPE使能 
	
#if 1
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE); //?????3????  ceshi	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_TIM10_IRQn; //???3??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //????3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif

  TIM_CtrlPWMOutputs(TIM1,ENABLE);	
	TIM_Cmd(TIM1, DISABLE);  //使能TIM14

										  
}  


void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM14时钟使能   
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载�
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化定时器14
	
	TIM_OCInitStructure.TIM_Pulse = arr/3;
	//初始化TIM34 Channel1 PWM模式	 
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 4OC1

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR1上的预装载寄存器
 //	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_ARRPreloadConfig(TIM3,ENABLE);//ARPE使能 

	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //?????3????  ceshi	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //???3??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01; //????3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
//	TIM_Cmd(TIM1, ENABLE);  //使能TIM14
  TIM_CtrlPWMOutputs(TIM3,ENABLE);
  TIM_Cmd(TIM3, DISABLE); 									  
} 


void TIM8_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	NVIC_InitTypeDef NVIC_InitStructure;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	//TIM14时钟使能    

	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载�
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);//初始化定时器14
	
	TIM_OCInitStructure.TIM_Pulse = arr/3;
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM8在CCR1上的预装载寄存器
// //	TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);
  TIM_ARRPreloadConfig(TIM8,ENABLE);//ARPE使能 

	TIM_ClearFlag(TIM8, TIM_FLAG_Update);

	TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE); //?????3????  ceshi	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn; //???3??
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00; //?????1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //????3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
//	TIM_Cmd(TIM1, ENABLE);  //使能TIM14
  TIM_CtrlPWMOutputs(TIM8,ENABLE);
  TIM_Cmd(TIM8, DISABLE); 
										  
} 



void TIM1_UP_TIM10_IRQHandler(void)
{ 
   if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
   {
			/* Clear TIM2 Capture Compare1 interrupt pending bit*/
			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
   }
	 PWMX_Num++;
	 
   switch(srdx.run_state) 
   {			
			case ACCEL:			
				if(PWMX_Num >= srdx.run_start) 
					{         		 
						 srdx.run_state = RUN;				 
					}						
				if(XResetFlag) 
				 srdx.step_delay= XACC[PWMX_Num];		
				else 
				 srdx.step_delay= XACCReset[PWMX_Num];
				TIM1->CCR1=srdx.step_delay/2;
				TIM1->ARR=srdx.step_delay;
				 
				break;
			
			case RUN:
				if(PWMX_Num >= srdx.decel_start)
				{
					srdx.run_state = DECEL;
				}
				break;
					
			case DECEL:				
				if(PWMX_Num >=srdx.accel_count-1)  //   if(PWMX_Num >srdx.accel_count-1)
				{
					srdx.run_state = STOP;
				}
				if(XResetFlag)  
					srdx.step_delay = XACC[srdx.accel_count-PWMX_Num-1];//shao
				else 
					srdx.step_delay = XACCReset[srdx.accel_count-PWMX_Num-1];
				TIM1->CCR1=srdx.step_delay/2;
				TIM1->ARR=srdx.step_delay;
				break;
					
			case LRUN:				
				if(PWMX_Num >=srdx.accel_count-1)
				{
					srdx.run_state = STOP;
				}
				break;
				
			case STOP:
				PWMX_Num = 0;
				XStopFlag =0;
				TIM_Cmd(TIM1, DISABLE);	
				break;
		}
}


void TIM3_IRQHandler(void)
{ 

   if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
   {
			/* Clear TIM2 Capture Compare1 interrupt pending bit*/
			TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
   }
	 
	  PWMY1_Num++;
	 
		if(Vib == 1)
		{			
			switch(srd1y.run_state) 
      {
				case ACCEL:		
					TIM3->CCR2=YACC[Y1SFlag][PWMY1_Num]>>2;
					TIM3->ARR=YACC[Y1SFlag][PWMY1_Num];
					if(PWMY1_Num >= srd1y.run_start) 
					{
						 srd1y.run_state = RUN;			 
					}	 
					break;
			
				case RUN:
					if(PWMY1_Num >= srd1y.decel_start)
					{
					 srd1y.run_state = DECEL;
					}
					break;
				case DECEL:

					if(PWMY1_Num >=srd1y.accel_count-1)  //   if(PWMX_Num >srdx.accel_count-1)
					{
						srd1y.run_state = STOP;		
					}
					else
					{
						TIM3->ARR=YACC[Y1SFlag][srd1y.accel_count-PWMY1_Num-1];
						TIM3->CCR2 = TIM3->ARR>>2;
					}
				  break;	
		  }
		}	 
		
		
}


void TIM8_UP_TIM13_IRQHandler(void)
{ 

   if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
   {
    /* Clear TIM2 Capture Compare1 interrupt pending bit*/
    TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
   }
	 
	  PWMY2_Num++;
}