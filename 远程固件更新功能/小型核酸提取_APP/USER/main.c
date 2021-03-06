

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "exti.h"
#include "timer.h"
#include "spi.h"
#include "pwm.h"
#include "moto.h"
#include "string.h"
#include "DeviceVibration.h"
#include "adc.h"
#include "128S085.h"
#include "adc.h"
//需要完善的功能；
//测试非标模式
//温度校准
//温度超温报警
//振荡时没有计时时间，，思路采用Time5定时器；
//测试接收指令执行, 打开接收温度值命令  继电器状态指令   加热指令   在串口命令处理函数中添加FrameCopyDeal 
//实现暂停指令，思路 通过开关PWM的定时器 Timer1 Timer3 Timer8；
//

//adc 采样会不会影响到震荡实验；adc 采样时间加长；
//加减速的部署测试；
//


#define temp_hot 1
#define temp_cool 0

#define Y1_up 1
#define Y1_down 0
#define Y2_up 1
#define Y2_down 0
#define Y2Y1_up 1
#define Y2Y1_down 0
#define X_forward 1
#define X_backward  0

u8 Flag_run = 0;               //记录仪器运行状态，0：停止；1：暂停；2：复位结束；3：准备就绪（接收到启动指令，且复位结束）；4：运行状态
u8 read = 0;
char SendBackBuff[20]={0};


//

//u16 temp_to_vol[8][90]={
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191},
//												
//												{2070,2048,2026,2004,1982,1960,1940,1920,1900,1880,1859,1839,1821,1802,1785,1767,1745,1733,1716,1699,1683,
//												1663,1647,1632,1618,1603,1590,1577,1564,1551,1539,1526,1513,1503,1491,1481,1469,1459,1449,1440,1431,1422,
//												1410,	1402,1394,1386,1372,1364,1357,1349,1341,1335,1327,1321,1315,1310,1305,1299,1294,1290,1284,1276,1269,
//												1263,1259,1255,1252,1247,1244,1242,1238,1234,1230,1225,1222,1219,1217,  1215,1213,1211,1209,1207,1205,1203,
//												1201,1199,1197,1195,1193,1191}
//												};
//u16 vol_to_temp[8][90]={
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347},
//												
//												{2186,2137,2093,2046,1999,1954,1907,1858,1809,1758,1712,1668,1628,1590,1549,1513,1479,1444,1412,1377,1342,
//												1310,1278,1245,1214,1182,1148,1120,1092,1063,1035,1008,981,955,931,906,882,865,845,827,805,782,760,737,715,
//												694,675,658,642,625,609,593,577,559,545,531,517,504,492,479,464,453,442,431,420,409,398,388,377,367,357,347}											
//											};

											
											
		//测试瓷套下降功能。。。									
											

int main(void)
{ 	
	u16 i;	
	
	SCB->VTOR = FLASH_BASE | 0x10000;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	EXTIX_Init();
	delay_init(168);                               //初始化延时函数              
	uart_init(19200);	                           //初始化串口波特率为115200	
//	delay_ms(100);      //延时	
	InitSwitch();  //control switch to hot     //@lyt,GPIOD,GPIOE,初始化	
	
	TIM1_GPIO_Config();          //168M 脉冲		
	TIM1_PWM_Init(4000-1,42-1); //2M/4000 =500HZ   500  6000  1.25mm/s  6M   1KHZ   600 75ms/s        2ms //  采用4M 主频设置	
	TIM3_PWM_Init(4000-1,42-1);   // 2M  Y1电机PWM初始化	
	TIM8_PWM_Init(4000-1,84-1);   // 2M   Y2电机PWM初始化	
	
  Dac128S_Init();   //外部DAC初始化  V
//	
	delay_ms(1000);

//	X_EN =1;
//	YT_EN =1;
//	YB_EN =1;
//	
//	delay_ms(1000);
//	X_EN =0;
//	YT_EN =0;
//	YB_EN =0;
//	
//	delay_ms(1000);
//	YT_EN =1;
//	X_EN =1;
//	YB_EN =1;
//	
//	delay_ms(1000);
	
	Y1Y2Reset();      //Y轴复位	
	X0Reset();  //等待Y轴复位结束，后，X轴复位  
	
	
	
  bsp_InitADC();   
  
  TIM5_Int_Init(4999-1,8400-1);//40ms 采集周期，32次 进行平均运算一次�   1s
	TIM_Cmd(TIM5,ENABLE);
	
	
	
	
	Tim2Config();	
	TIM_Cmd(TIM2, ENABLE);  



	
	
//  Switch0 =0; 
//  Switch1 =0;   //  打开8路继电器
//	Switch2 =0;
//	Switch3 =0;
//	
//	Switch4 =0;
//	Switch5 =0;
//	Switch6 =0;
//	Switch7 =0;
////	
//////	Switch8 =0;		
//////	
////	
//	Dac128S085_Vol(GetVoltageDACS(90,0),0);
////		Dac128S085_Vol(1200,0);  // 通道温度的设置 1200 -60度；
//delay_ms(1);
////Switch0=!w0r;


//Dac128S085_Vol(GetVoltageDACS(60,1),1);
////	Dac128S085_Vol(1200,1);  	
//delay_ms(1);
////Switch1=!w1r;


//Dac128S085_Vol(GetVoltageDACS(70,2),2);
////		Dac128S085_Vol(1200,2);  
//delay_ms(1);
////Switch2=!w2r;


//Dac128S085_Vol(GetVoltageDACS(80,3),3);
////		Dac128S085_Vol(1200,3); 
//delay_ms(1);
////Switch3=!w3r;


//Dac128S085_Vol(GetVoltageDACS(55,4),4);
////	Dac128S085_Vol(1200,4); 
//delay_ms(1);
////Switch4=!w4r;

//Dac128S085_Vol(GetVoltageDACS(55,5),5);
////	Dac128S085_Vol(1200,5); 
//delay_ms(1);
////Switch5=!w5r;


//Dac128S085_Vol(GetVoltageDACS(55,6),6);
////	Dac128S085_Vol(1200,6); 
//delay_ms(1);
////Switch6=!w6r;


//Dac128S085_Vol(GetVoltageDACS(55,7),7); 
////		Dac128S085_Vol(1200,7);  
//delay_ms(1);
//Switch7=!w7r;
//JRTime=10000;



//	Dac128S085_Vol(GetVoltageDACS(70,0),0);
////		Dac128S085_Vol(1200,0);  // 通道温度的设置 1200 -60度；
////Switch0=!w0r;
//Dac128S085_Vol(GetVoltageDACS(70,1),1);
////	Dac128S085_Vol(1200,1);  	

////Switch1=!w1r;

//Dac128S085_Vol(GetVoltageDACS(70,2),2);
////		Dac128S085_Vol(1200,2);  

////Switch2=!w2r;

//Dac128S085_Vol(GetVoltageDACS(70,3),3);
////		Dac128S085_Vol(1200,3); 

////Switch3=!w3r;
//Dac128S085_Vol(GetVoltageDACS(80,4),4);
////	Dac128S085_Vol(1200,4); 

////Switch4=!w4r;
//Dac128S085_Vol(GetVoltageDACS(80,5),5);
////	Dac128S085_Vol(1200,5); 
////Switch5=!w5r;
//Dac128S085_Vol(GetVoltageDACS(80,6),6);
////	Dac128S085_Vol(1200,6); 
////Switch6=!w6r;
//Dac128S085_Vol(GetVoltageDACS(80,7),7); 


 //MotorX0Reset(0,600,36000);   //
 
//  delay_ms(1000);
//	
//	MotorY1Mov(0,800,4720);      
////	MotoY1Mov(Y1_up,800,(vibbottom0/(2.034*20))*1600);
//	MotorY2Mov(0,800,2800);	
//	MotorY1Vib(6,3,30);
//		
//  delay_ms(2000);
  
  while(1)
	{		
		
		

//		MotorX0Reset(0,600,36000);   //
//	 delay_ms(100);
//      Send_Str_uart4(": waiting  command\r\n");		
      FrameCopyDeal();  //串口通信处理函数，接收触摸屏的指令；
/*************************************照明灯**************************************/
			if(lll==1)			   //判断是否开灯
			{			 	
				lll = 2;
				BAK = 1;	//ligth IO kou
			}
			else if(lll==3)
			{			 	
				lll = 2;
				BAK = 0;			 
			}
			
	/***********************************换磁套下降************************************/
			if(lll==5)
			{
				lll = 2;  
				MotorX0Mov(X_forward,800,xdelt);  /////xdelt ==0;
				MotorY1Mov(Y1_down,600,2000);   //1500
			}	 
				
	/***********************************复位/换磁套上升********************************/
			if(lll==7)
			{
				lll=2;		 
				MotorY2Mov(Y2_down,200,100);		
				Y1Y2Reset();	
        				
			}	
	
	/************************************加热模块,开启/关闭，设置温度********************/
//							  
//					  
//					
//					  Dac128S085_Vol(GetVoltageDACS(BDDac[3],3),3);
//					  Dac128S085_Vol(GetVoltageDACS(BDDac[4],4),4);
//					  Dac128S085_Vol(GetVoltageDACS(BDDac[5],5),5);
//					  Dac128S085_Vol(GetVoltageDACS(BDDac[6],6),6);
//					  Dac128S085_Vol(GetVoltageDACS(BDDac[7],7),7); 



//			Dac128S085_Vol(GetVoltageDACS(BDDac[0],0),0);
//	//		Dac128S085_Vol(1200,0);  // 通道温度的设置 1200 -60度；
//			delay_ms(1);
//			Switch0=!w0r;
//			
//			
//			Dac128S085_Vol(GetVoltageDACS(BDDac[1],1),1);
//		//	Dac128S085_Vol(1200,1);  	
//			delay_ms(1);
//			Switch1=!w1r;
//			
//			
//			Dac128S085_Vol(GetVoltageDACS(BDDac[2],2),2);
//	//		Dac128S085_Vol(1200,2);  
//			delay_ms(1);
//			Switch2=!w2r;
//			
//			
//			Dac128S085_Vol(GetVoltageDACS(BDDac[3],3),3);
//	//		Dac128S085_Vol(1200,3); 
//			delay_ms(1);
//			Switch3=!w3r;
//		
//			
//			Dac128S085_Vol(GetVoltageDACS(BDDac[4],4),4);
//		//	Dac128S085_Vol(1200,4); 
//			delay_ms(1);
//			Switch4=!w4r;
//			
//     Dac128S085_Vol(GetVoltageDACS(BDDac[5],5),5);
//		//	Dac128S085_Vol(1200,5); 
//			delay_ms(1);
//			Switch5=!w5r;
//			
//			
//			Dac128S085_Vol(GetVoltageDACS(BDDac[6],6),6);
//		//	Dac128S085_Vol(1200,6); 
//			delay_ms(1);
//			Switch6=!w6r;
//			
//			
//			Dac128S085_Vol(GetVoltageDACS(BDDac[7],7),7); 
//	//		Dac128S085_Vol(1200,7);  
//			delay_ms(1);
//			Switch7=!w7r;
			
			
			
	/************************************风扇控制**************************************/
			if(lll==16)
			{
					lll=2;
					FUN = 1;
			}
			else if(lll==17)
			{
					lll=2;
					FUN = 0;
			}
	/*****************************电加热膜控制*********************************************/
			switch(lll)
			{
				case 8 :
					Switch0=1;					
					break;
				case 9 :
					Switch0=0;					
					break;
				case 10 :
					Switch1=1;					
					break;
				case 11 :
					Switch1=0;					
					break;
				case 12 :
					Switch2=1;					
					break;
				case 13 :
					Switch2=0;					
					break;
				case 14 :
					Switch3=1;					
					break;
				case 15 :
					Switch3=0;					
					break;
				case 18 :
					Switch4=1;					
					break;
				case 19 :
					Switch4=0;					
					break;
				case 20 :
					Switch5=1;					
					break;
				case 21 :
					Switch5=0;					
					break;
				case 22 :
					Switch6=1;					
					break;
				case 23 :
					Switch6=0;					
					break;
				case 24:
					Switch7=1;					
					break;
				case 25 :
					Switch7=0;					
					break;
        default :
//					Send_Str_uart4(": unknow  lll command");
				  break;
          					
			}
						
	/*****************************************紫外灯****************************************/
			if(uuu==1)		//判读是否消毒
			{
					UV=1;
			}
			else if((uuu==0)||(uuu==3))
			{
					UV=0;
			}
				
	/************************************电机运行程序****************************************/
			if((sss == 3 )	&& (ReadP == WriteP))		  //监测到开始信号，并且读取了所有的输入量
			{		
				if(DOOR == 1) 
				{
						sss = 0;
					  Flag_run = 2;
						Send_Str(":dialogbox=8\r\n");		
					  memset(USART_RX_BUF,0,USART_REC_LEN);
					  ReadP = 0;
					  WriteP = 0;
					  delay_ms(100);				
				}
				else
				{					
//					Send_Str_uart4(" biaocheng_run\r\n");
					Flag_run = 4;                                    //标识仪器正在运行
					Y1RSTOV = 0;
					Y2RSTOV = 0;
					X0RSTOV = 0;		
		
					MovProcess();  
				}			
				
			}       	  
				
			
	/******************************************自由编程************************************/
	//非标模式
	    if((sss == 9)  && (ReadP == WriteP))
			{						
				  if(DOOR == 1)
					{
					  sss=0;
						Flag_run=2;
						Send_Str(":dialogbox=8\r\n");
						memset(USART_RX_BUF,0,USART_REC_LEN);						
						ReadP = 0;
					  WriteP = 0;	
					}
					else
					{
						  if(Flag_run == 9)							   //如果复位结束，且仪器还未启动，则设置X轴偏移
							{
								 FUN  =1;
								//	BAK  =1;
									Switch0=!w0r;
									Switch1=!w1r;
									Switch2=!w2r;
									Switch3=!w3r;
									Switch4=!w4r;
									Switch5=!w5r;
									Switch6=!w6r;
									Switch7=!w7r;	
									if(t0k&&t1k&&t2k&&t3k&&t4k&&t5k&&t6k&&t7k)
									{
										
											Dac128S085_Vol(GetVoltageDACS(t0k,7),7);		
											Dac128S085_Vol(GetVoltageDACS(t1k,5),5);	
											Dac128S085_Vol(GetVoltageDACS(t2k,3),3);//		Dac128S085_Vol(1200,2);  
											Dac128S085_Vol(GetVoltageDACS(t3k,1),1);		
											Dac128S085_Vol(GetVoltageDACS(t4k,4),4);//	Dac128S085_Vol(1200,4); 		
											Dac128S085_Vol(GetVoltageDACS(t5k,6),6);//	Dac128S085_Vol(1200,5); 		
											Dac128S085_Vol(GetVoltageDACS(t6k,0),0);		
											Dac128S085_Vol(GetVoltageDACS(t7k,2),2); 
						   JRFlag =1;
               while(JRTime<180);  //1分钟
             	JRTime=0;
             	JRFlag =0;
									
									}
									else
									{	
										//err;
									}	
								
								
								
								
								 MotorX0Mov(X_forward,600,xdelt);		
//							   Send_Str_uart4(": feibiao\r\n");							
							}
													
							Flag_run = 4;
//							EXTI->IMR|=1<<2;	   //打开中断，程序容易跑飞
//							EXTI->IMR|=1<<3;
//							EXTI->IMR|=1<<4;
							
//							memset(USART_RX_BUF,0,USART_REC_LEN);
//							ReadP = 0;
//							WriteP = 0;
//							
				/************************振动混匀**********************/		
							if(s0p==1)
							{
								s0p=0;
								if(s3p<0)
								{
									MotorX0Mov(X_backward,600,7200*abs(s3p));
								}
								else
								{
									MotorX0Mov(X_forward,600,7200*abs(s3p));
								}
								MotorY1Mov(Y1_down,800,1600);
//								MotoY1Mov(Y1_up,800,((vibbottom0+vibbottom1+vibbottom2+vibbottom3+vibbottom4+vibbottom5+vibbottom6+vibbottom7)/(2.034*20))*1600);
//								vibbottom0=0;
//								vibbottom1=0;
//								vibbottom2=0;
//								vibbottom3=0;
//								vibbottom4=0;
//								vibbottom5=0;
//								vibbottom6=0;
//								vibbottom7=0;
								
								MotorY1Vib(s1p,s2p,s5p*60);								
								MotorY1Mov(Y1_up,800,1600);
								Package("overflag","1",SendBackBuff);
//								Send_Str_uart4(": zhengdang11\r\n");
								Send_Str(SendBackBuff);
//								Send_Str_uart4(": zhengdang\r\n");
							}
							
					/************************等待时间**********************/						
							if(s0p==2)
							{
								s0p=0;
								if(s3p<0)
								{
									MotorX0Mov(X_backward,600,7200*abs(s3p));
								}
								else
								{
									MotorX0Mov(X_forward,600,7200*abs(s3p));
								}
								delay_ms(s5p*60*1000);			
								Package("overflag","1",SendBackBuff);
								Send_Str(SendBackBuff);
//								Send_Str_uart4(": dengdai\r\n");
							}
							
					/************************吸磁珠**********************/						
							if(s0p==3)
							{
								s0p=0;
								if(s3p<0)
								{
									MotorX0Mov(X_backward,600,7200*abs(s3p));
								}
								else
								{
									MotorX0Mov(X_forward,600,7200*abs(s3p));
								}							
								MotorY1Y2Mov(Y2Y1_down,s4p,1600);
								delay_ms(5);
								MotorY1Y2Mov(Y2Y1_up,s4p,1600);
								Package("overflag","1",SendBackBuff);
								Send_Str(SendBackBuff);
//								Send_Str_uart4(": xi\r\n");
							}
							
					/************************放磁珠**********************/					
							if(s0p==4)
							{
								s0p=0;
								if(s3p<0)
								{
									MotorX0Mov(X_backward,600,7200*abs(s3p));
								}
								else
								{
									MotorX0Mov(X_forward,600,7200*abs(s3p));
								}
								MotorY1Y2Mov(Y2Y1_down,s4p,1600);
								MotorY2Mov(Y2_up,800,1600);
								MotorY1Mov(Y1_up,800,1600);
								Package("overflag","1",SendBackBuff);
								Send_Str(SendBackBuff);
//								Send_Str_uart4(": fang\r\n");
							}
							
					/************************流程结束**********************/			
							if(usy==1)
							{
								usy=0;
//								Y1Y2Reset();
//								X0Reset();	
								for(i=30000;i>0;i--);      //延时	
								Package("overflag","2",SendBackBuff);
								Send_Str(SendBackBuff);
//								Send_Str_uart4(": end\r\n");
								delay_ms(200);
								__set_FAULTMASK(1);
								NVIC_SystemReset();
//								Send_Str_uart4(": end2\r\n");
							}
					}
			}
	
				     
  	} 	 	 
 }
		
 
	




