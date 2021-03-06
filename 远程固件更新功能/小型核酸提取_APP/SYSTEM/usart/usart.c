#include "sys.h"
#include "usart.h"	
#include "exti.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;    
//  while((USART1->SR&0X40)==0);  
	return ch;
}
#endif
 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	

u8 Sending=0;
u8 dat_ok=0;
//uchar revbuf[600] = {0};
u16 EEprom[50] = {0};

u8 HeadOk = 0;
u16 HeadPosition = 0;
u8 HeaderOk = 0;
u8 reving = 0;

unsigned char FrameDeal[16] = {0};
u16 WriteP = 0;
u16 ReadP = 0;


u8 t1k =0;
u8 t2k =0;
u8 t3k =0;
u8 t4k =0;
u8 t5k =0;
u8 t6k =0;
u8 t7k =0;
u8 t0k =0;


u8 w0r =0;
u8 w1r =0;
u8 w2r =0;
u8 w3r =0;	
u8 w4r =0;
u8 w5r =0;
u8 w6r =0;
u8 w7r =0;

u8 vibbottom0=0;
u8 vibbottom1=0;
u8 vibbottom2=0;
u8 vibbottom3=0;
u8 vibbottom4=0;
u8 vibbottom5=0;
u8 vibbottom6=0;
u8 vibbottom7=0;
u8 vibbottom8=0;




u16 FrameLen=0;

u8 sss=0;    //启动/停止/暂停/复位结束/继续运行
u8 ppp=0;

u16 lll=0;
u16 uuu=0;
u16 tzw=0;
u16 s0p=0;
//u16 usk=0;
//u16 usr=0;
//u16 uss=0;
//u16 us0=0;
//u16 us9=0;
 u16 s1p=0;
 u16 s2p=0;
 short s3p=0;
 u16 s4p=0;
 u16 s5p=0;

u16 usy=0;

u16 xdelt=0;
u16 y1firstmov=0;
u16 y2firstmov=0;
	   	   	   	   
unsigned char BufId[5];
unsigned char BufValue[5];

u16 NumValue=0;

unsigned char WorkStatusBuf1[15]={0x3A,0x73,0x3D,0x74,0x72,0x75,0x65,0x38,0x65,0x0D,0x0A,'\0'};	   //s=1
unsigned char WorkStatusBuf2[15]={0x3A,0x68,0x3D,0x74,0x72,0x75,0x65,0x39,0x39,0x0D,0x0A,'\0'};	   //h=1
unsigned char WorkStatusBuf3[15]={0x3A,0x6A,0x3D,0x74,0x72,0x75,0x65,0x39,0x37,0x0D,0x0A,'\0'};	   //j=1
unsigned char WorkStatusBuf4[15]={0x3A,0x77,0x31,0x3D,0x74,0x72,0x75,0x65,0x35,0x39,0x0D,0x0A,'\0'}; //w1
unsigned char WorkStatusBuf5[15]={0x3A,0x77,0x32,0x3D,0x74,0x72,0x75,0x65,0x35,0x38,0x0D,0x0A,'\0'}; //w2	
unsigned char WorkStatusBuf6[15]={0x3A,0x77,0x33,0x3D,0x74,0x72,0x75,0x65,0x35,0x37,0x0D,0x0A,'\0'}; //w3
unsigned char WorkStatusBuf7[15]={0x3A,0x74,0x3D,0x74,0x72,0x75,0x65,0x38,0x64,0x0D,0x0A,'\0'};	   //t
unsigned char WorkStatusBuf8[15]={0x3A,0x66,0x3D,0x74,0x72,0x75,0x65,0x39,0x62,0x0D,0x0A,'\0'};	   //f
unsigned char WorkStatusBuf9[15]={0x3A,0x7A,0x3D,0x66,0x61,0x6C,0x73,0x65,0x33,0x63,0x0D,0x0A,'\0'};       //z
unsigned char WorkStatusBuf10[15]={0x3A,0x6F,0x3D,0x74,0x72,0x75,0x65,0x39,0x32,0x0D,0x0A,'\0'};		   //o
unsigned char WorkStatusBuf11[18]={0x3A,0x73,0x61,0x76,0x65,0x6F,0x6B,0x3D,0x74,0x72,0x75,0x65,0x37,0x36,0x0D,0x0A,'\0'};  //saveok
unsigned char WorkStatusBuf12[15]={0x3A,0x6F,0x76,0x65,0x72,0x3D,0x31,0x64,0x34,0x0D,0x0A,'\0'};


unsigned char SendBackBuf[20]={0};




u8 Frame_Store[Max_Frame_Count][RS232_REC_LEN]; //帧缓存数组，最多可存放10条帧指令
u8 RS232_RX_BUF[RS232_REC_LEN];

//接收到的数据长度
uint16_t RS232_RX_CNT=0;
uint16_t Flag_Deal = 0;
uint16_t  Circle_Frame_REC = 0; //指令缓冲区，接收指令的循环数
uint16_t  Circle_Frame_Deal = 0;//指令缓冲区，执行指令的循环数

u8   SeqNum_Frame_REC = 0; //帧缓存数组中目前帧存储位置
u8   SeqNum_Frame_Deal =0; //帧缓存数组中目前帧执行位置



//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;  
	

//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
//	//DMA??????  
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
//	NVIC_Init(&NVIC_InitStructure);  
//	
//	    //DMA????  
//	DMA_DeInit(DMA1_Stream6);  
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;   
//	//????  
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);  
//	//????  
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Tx_Buf_Gsm;  
//	//dma????  
//	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;  
//	//??DMA??????????  
//	DMA_InitStructure.DMA_BufferSize = TX_LEN_GSM;  
//	//??DMA???????,????  
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
//	//??DMA???????  
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
//	//??????  
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
//	//??????  
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;  
//	//??DMA?????  
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
//	//??DMA?????  
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  
//		
//	//????FIFO?????????????? : ???FIFO??    
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      
//	//???FIFO????  
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;          
//	//???Burst????????   
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         
//	//???Burst???????? */    
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;   

//	//??DMA1???           
//	DMA_Init(DMA1_Stream6, &DMA_InitStructure);    
//	//????  
//	DMA_ITConfig(DMA1_Stream6,DMA_IT_TC,ENABLE);     

//	//???DMA??    
//	//??DMA??  
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  
//	//DMA????  
//	DMA_DeInit(DMA1_Stream5);  
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;  
//	//????  
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART2->DR);  
//	//????  
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)Rx_Buf_Gsm;  
//	//dma????  
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;  
//	//??DMA??????????  
//	DMA_InitStructure.DMA_BufferSize = RX_LEN_GSM;  
//	//??DMA???????,????  
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  
//	//??DMA???????  
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  
//	//??????  
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  
//	//??????  
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  
//	//??DMA?????  
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  
//	//??DMA?????  
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;  
//		
//	//????FIFO?????????????? : ???FIFO??    
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;      
//	//???FIFO????  
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;          
//	//???Burst????????   
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;         
//	//???Burst???????? */    
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;   
//		
//	//??DMA1???           
//	DMA_Init(DMA1_Stream5, &DMA_InitStructure);    
//	//????  
//	DMA_Cmd(DMA1_Stream5,ENABLE)
		
		
		
		
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口1 
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

#endif

  

}


void DMA1_Stream6_IRQHandler(void)  
{  
    //gsm_dma_tx_irq_handler();  
}




extern u16 MotorY1Vib_t;
extern u8 Vib;
//extern u8 door_open;
extern u8 TIM1_enable_door;
extern u8 TIM3_enable_door;
extern u8 TIM8_enable_door;
extern u8 suspend_vib_door;
extern u8 Flag_run;

u8 suspend_vib_command;
u8 TIM1_enable_usart = 0;
u8 TIM3_enable_usart = 0;
u8 TIM8_enable_usart = 0;

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
	u8 vib_state;
  u16 VarPosition = 0;
	u16 ValuePosition = 0;	
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		 USART_RX_BUF[WriteP]=USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据，同时硬件自动清除中断

		if(USART_RX_BUF[WriteP]==':')
		{
			HeadOk = 1;
			HeadPosition = WriteP;			
		}
		if(HeadOk)
		{
			if((USART_RX_BUF[WriteP]=='\n') && (USART_RX_BUF[WriteP-1]=='\r'))
			{
				if(HeadPosition+3>=USART_REC_LEN)
				{
					VarPosition = HeadPosition+3 - USART_REC_LEN;
					ValuePosition = HeadPosition+5 - USART_REC_LEN;
				}
				else
				{
					VarPosition = HeadPosition+3;
					ValuePosition = HeadPosition+5;					
				}
				if((USART_RX_BUF[VarPosition] == 's')&&(USART_RX_BUF[ValuePosition] == 0x33)) //0x31 1 2 3
				{
					
						if(Flag_run == 2)                      //Flag_run == 2，X0Reset()函数中赋值 ，标志复位结束   
						{
								sss= 3;                       //接收到启动命令
							  Flag_run = 3;
						}						
						else                             //如果复位没有结束，则向上位机发送报文
						{
							Send_Str(":dialogbox=7\r\n");
							memset(USART_RX_BUF,0,USART_REC_LEN);
							ReadP = 0;
							WriteP = 0;
//							NVIC_SystemReset();
						}
					  
				}
				if((USART_RX_BUF[VarPosition] == 's')&&(USART_RX_BUF[ValuePosition] == 0x39)) //0x31 1 2 3
				{
					
						if(Flag_run == 2)                      //Flag_run == 2，X0Reset()函数中赋值 ，标志复位结束   
						{
								sss=9;                    //接收到启动命令
							  Flag_run = 9;
						}						
						else                             //如果复位没有结束，则向上位机发送报文
						{
							Send_Str(":dialogbox=7\r\n");
							memset(USART_RX_BUF,0,USART_REC_LEN);
							ReadP = 0;
							WriteP = 0;
//							NVIC_SystemReset();
						}
					  
				}
				
				if((USART_RX_BUF[VarPosition] == 's')&&(USART_RX_BUF[ValuePosition] == 0x35))
				{
				  	sss= 5;    					//接收到结束命令
						memset(USART_RX_BUF,0,USART_REC_LEN);						
						ReadP = 0;
						WriteP = 0;
					  delay_ms(200);
					  __set_FAULTMASK(1);
            NVIC_SystemReset();
//					RSTSRC |= (1<<4);			////////////////////////////////				
				}
				
				if((USART_RX_BUF[VarPosition] == 's')&&(USART_RX_BUF[ValuePosition] == 0x31)) //添加暂停功能
				{
//						sss= 1;                           //接收到暂停命令
					  Flag_run=1;
						if((TIM1->CR1 & TIM_CR1_CEN) == 1)                    //判断关闭所有的定时器，TIM1是否开启
						{
							TIM_Cmd(TIM1,DISABLE);													//TIM1_enable_usart系列变量，标记，收到暂停命令前，各个定时器的开启、关闭状态
							TIM1_enable_usart = 1;
						}
						if((TIM3->CR1 & TIM_CR1_CEN) == 1)
						{
							if(Vib == 1)                                     //判断是否处于振动状态，如果处于振动状态，则向震动函数发送暂停标记
							{
									suspend_vib_command = 1;		
							}	
							else
							{
									TIM_Cmd(TIM3,DISABLE);								
							}			
							TIM3_enable_usart = 1;							
						}
						if((TIM8->CR1 & TIM_CR1_CEN) == 1)
						{
							TIM_Cmd(TIM8,DISABLE);
							TIM8_enable_usart = 1;
						}			
						
				}
				if((USART_RX_BUF[VarPosition] == 's')&&(USART_RX_BUF[ValuePosition] == 0x30)) //添加继续功能
				{					
					  if(DOOR == 1)
						{
						//	Send_Str(":dialogbox=8\r\n");					
              Send_Str(":door=1\r\n");
							
						}
						
						
						else
						{
					//			door_open = 0;
							 Send_Str(":door=0\r\n");
								Flag_run= 4;                                    //接收到继续运行命令
								if((TIM1_enable_usart == 1) || ((TIM1_enable_door == 1)))               //判断关闭所有的定时器前，判断 TIM1是否开启，如果之前开启，现在就打开TIM1     
								{
									TIM_Cmd(TIM1,ENABLE);
									TIM1_enable_usart = 0;   
									TIM1_enable_door = 0;
								}
								if((TIM3_enable_usart == 1) || (TIM3_enable_door == 1))
								{ 
									if(Vib == 1)                             
									{
											suspend_vib_command = 0;		
											suspend_vib_door = 0;
									}	
									else
									{
											TIM_Cmd(TIM3,ENABLE);									
									}		
									TIM3_enable_usart = 0;
									TIM3_enable_door = 0;
								}
								if((TIM8_enable_usart == 1) || (TIM8_enable_door == 1))
								{
									TIM_Cmd(TIM8,ENABLE);
									TIM8_enable_usart = 0;
									TIM8_enable_door = 0;
								}	
						}									
				}				
				HeadOk = 0;		
			}
		}

		WriteP++;
		if(WriteP >= USART_REC_LEN)
		{
			WriteP = 0;
		} 
  } 
} 
		

void SendOneByte(unsigned char c)
 {
//	    while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //等待上一次发送结束
      USART_SendData(USART1,c); //发送数据
   	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //等待发送结束		
 }
void Send_Str(unsigned char* str)
 {
	 while((*str) != '\0')
	{
		SendOneByte(*str);
		str++;
	}
 } 
 
 
void SendOneByte_uart4(unsigned char c)
 {
	 
      USART_SendData(UART4,c); //发送数据
   	  while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET); //等待发送结束		
 } 
 
void Send_Str_uart4(unsigned char* str)
 {
	 while((*str) != '\0')
	{
		SendOneByte_uart4(*str);
		str++;
	}
 }


 

 void FrameCopyDeal(void)
 {
	 while(ReadP != WriteP)
	{

		reving = USART_RX_BUF[ReadP++];
		if(ReadP>USART_REC_LEN-1) ReadP=0;

		if(dat_ok == 0)
		{
			if(HeaderOk)
			{
				FrameDeal[FrameLen] = reving;
				if((FrameDeal[FrameLen]=='\n') && (FrameDeal[FrameLen-1]=='\r'))
				{
				  dat_ok = 1;
				  HeaderOk=0;				
				}
				FrameLen++;			
			}

		}

		if(!HeaderOk && (reving==':'))  
 		{ 
			HeaderOk=1;   
			FrameLen=0;
		}
//unsigned char BufId[5];
//unsigned char BufValue[5];
    // 	if(0 == DataCheck(BufId,BufValue))
		if(dat_ok)
  	{
		
	//			unsigned char tempbuf[4] = {0};
//				u16 tm = 0;
//				u16 ts = 0; 
			//	int num = 0;

				unsigned char i=0,cmd_len=0,temp_len=0;
				unsigned char * cmd_name;
				unsigned char * cmd_value;
				unsigned char * cmd_index ;

				dat_ok = 0;
				cmd_len = strlen(FrameDeal);
				
				cmd_name = FrameDeal;
				
				cmd_index = &FrameDeal[0];
				while ('=' != *cmd_index)
				{
					cmd_index++;
					temp_len++;	

					if(temp_len >= 4)
					{
						Send_Str("Err");				
						break;
					 }
				}

				*cmd_index='\0';
				cmd_value = cmd_index+1;
				while ('\n' != *cmd_index)
				{
					cmd_index++;
				}
				*(cmd_index-1)='\0';
				strcpy(BufId,cmd_name);
				strcpy(BufValue,cmd_value);
				memset(FrameDeal,0,30);

				NumValue = my_atoi(BufValue);

			switch(BufId[2])
			{
				case 'l':
					lll = NumValue;
					break;	
				case 'u':			
					uuu = NumValue;
					break;
				case 'w':			
					tzw = NumValue;
					break;
				case 'y':			
					usy = NumValue;
					break;
			   //L
				case 'a'://
					EEprom[0] = NumValue;	
					break;				
				case 'b':			//
					EEprom[1] = NumValue;						
					break;				
				case 'c':			//
					EEprom[2] = NumValue;
					break;				
				case 'd':			//
					EEprom[3] = NumValue;
					SendBack_L();
					break;
				//H					
				case 'e':			//
//					switch(BufId[1])
//						{
//							case 'h':
					EEprom[4] = NumValue;	
					SendBack_L();
					break;

//						}				
//					break;				
				case 'f':			
					switch(BufId[1])
						{
							case 'h':				//
								EEprom[5] = NumValue;	
								SendBack_L();
								break;
							case '1':				//
								y1firstmov = NumValue;
								break;
							case '2':      //
								y2firstmov = NumValue;
								break;
							
							case '3'://
								xdelt = NumValue;
								break;
						}		
					break;				
				case 'g':			//
					EEprom[6] = NumValue;
					break;				
				case 'h':			
					EEprom[7] = NumValue;
					break;				
				case 'i':			//
					EEprom[8] = NumValue;
					break;				
				case 'j':			
					EEprom[9] = NumValue;
					SendBack_H();
					break;				
				case 'k':
				
						switch(BufId[1])
						{
							case '0':			//设定温度值
								t0k = NumValue;	
								break;
							case '1':     //
								t1k = NumValue;	
								break;
							case '2':			//
								t2k = NumValue;	
								break;
							case '3':			//
								t3k = NumValue;	
								break;
							case '4':			//
								t4k = NumValue;	
								break;
							case '5':			//
								t5k = NumValue;	
								break;
							case '6':
								t6k = NumValue;	
								break;
							case '7':
								t7k = NumValue;	
								break;
						}
						break;
				
				case 'v':			
					EEprom[10] = NumValue;
					break;
				//J
				case 'm':			//
					EEprom[11] = NumValue;	
					break;				
				case 'n':			//
					EEprom[12] = NumValue;	
					break;				
				case 'o':			//
					EEprom[13] = NumValue;
					break;				
				case 'p':			//
						switch(BufId[1])
						{
							case '0':
									s0p = NumValue;
								break;
							case '1':
							  	s1p = NumValue;
								break;
							case '2':
								  s2p = NumValue;
								break;
							
							case '3':
							  	s3p = NumValue;
								break;
							case '4':
							  	s4p = NumValue;
								break;
							case '5':
							  	s5p = NumValue;
								break;
						}		
					break;
				
				case 'q':			//
					EEprom[14] = NumValue;
					break;	
      	case 'r':			//
				
						switch(BufId[1])
						{
							case '0':
								w0r = NumValue;	
								break;
							case '1':
								w1r = NumValue;	
								break;
							case '2':
								w2r = NumValue;	
								break;
							case '3':
								w3r = NumValue;	
								break;
							case '4':
								w4r = NumValue;	
								break;
							case '5':
								w5r = NumValue;	
								break;
							case '6':
								w6r = NumValue;	
								break;
							case '7':
								w7r = NumValue;	
								break;
						}
					break;
				
  				
				case 't':			//
					EEprom[15] = NumValue;
					break;				
				case 'x':			
					EEprom[16] = NumValue;
					SendBack_J();
					break;
				//1
				case 'A':			//
					EEprom[17] = NumValue;	
					break;				
				case 'B':			//
					EEprom[18] = NumValue;	
					break;				
				case 'C':			//
					EEprom[19] = NumValue;
					break;				
				case 'D':			//
					EEprom[20] = NumValue;
					break;							
				case 'E':			//
					EEprom[21] = NumValue;
					break;				
				case 'F':			
					EEprom[22] = NumValue;
					SendBack_1();
					break;
				//2
				case 'G':			//
					EEprom[23] = NumValue;	
					break;				
				case 'H':			//
					EEprom[24] = NumValue;	
					break;				
				case 'I':			//
						EEprom[25] = NumValue;
					break;				
				case 'J':			//
					EEprom[26] = NumValue;
					break;							
				case 'K':			//
					EEprom[27] = NumValue;
					break;				
				case 'L':			
					EEprom[28] = NumValue;
					SendBack_2();
					break;
				//3
				case 'M':			//
					EEprom[29] = NumValue;	
					break;				
				case 'N':			//
					EEprom[30] = NumValue;	
					break;				
				case 'O':			//
					EEprom[31] = NumValue;
					break;				
				case 'P':			//
					EEprom[32] = NumValue;
					break;							
				case 'Q':			//
					EEprom[33] = NumValue;
					break;				
				case 'R':			//
					EEprom[34] = NumValue;
					SendBack_3();
					break;
				//T
				case 'S':			//
					EEprom[35] = NumValue;	
					break;				
				case 'T':			//
					EEprom[36] = NumValue;										
					break;				
				case 'U':			//
					EEprom[37] = NumValue;
					break;				
				case 'V':			//
					EEprom[38] = NumValue;
					break;							
				case 'W':			//
					EEprom[39] = NumValue;
					break;				
				case 'X':			
					EEprom[40] = NumValue;
					SendBack_T();
					break;
				//S				
				case 'Y':			//
					EEprom[41] = NumValue;	
					break;							
				case 'Z':			//
					EEprom[42] = NumValue;	
					break;				
				case 'z':			//
					EEprom[43] = NumValue;
					SendBack_S();
					break;	
        case '_':
					switch (BufId[1])
					{
						case '0':
							vibbottom0 = NumValue;
							break;
						case '1':
							vibbottom1 = NumValue;
							break;
						case '2':
							vibbottom2 = NumValue;
							break;
						case '3':
							vibbottom3 = NumValue;
							break;
						case '4':
							vibbottom4 = NumValue;
							break;
						case '5':
							vibbottom5 = NumValue;
							break;
						case '6':
							vibbottom6 = NumValue;
							break;
						case '7':
							vibbottom7 = NumValue;
							break;
						case'8':
							vibbottom8 = NumValue;
							break;						
					}					
					break;

				default:
					break;
			}
							
		}   

	}

}

 //USART1发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void USART1_Send_Data(u8 *buf,u8 len)
{
	u8 t;
	for(t=0;t<len;t++)		//循环发送数据
	{
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //等待发送结束		
    USART_SendData(USART1,buf[t]); //发送数据
	}	 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //等待发送结束		
}
 
 
 
/*************************************************************************************
函数名：DataCheck( )
函数功能：解析接收到的ASCII数据指令
函数参数：id (存储解析后的ID数据),value (存储解析后的VALUE数据)
函数返回值：返回解析状态信息 0：解析正确1：解析错误
*************************************************************************************/

unsigned char  DataCheck(unsigned char* id,unsigned char* value)
{
	int num = 0;
	unsigned char i=0,cmd_len=0,temp_len=0;
	unsigned char * cmd_name;
	unsigned char * cmd_value;
	unsigned char * cmd_index ;
	unsigned char temp[3] = {0};
	unsigned char Lrc_tmp[3] = {0};
	if(dat_ok)
	{
		dat_ok = 0;

			num = 0;
			cmd_len = strlen(FrameDeal);
			for(i=0;i<cmd_len-4;i++)
			{			
				num += FrameDeal[i];//num=num+sendbuf[i];				
			}
			num %= 255;
			num = (~num)+1;
			num &= 255;			
			//memset( temp , 0x00 , 4);
		    sprintf(temp, "%02x", num);

			memcpy(Lrc_tmp,&FrameDeal[i],2);

			if(strcmp(temp,Lrc_tmp) == 0)
			{
			 	cmd_name = FrameDeal;
				if(!(strchr(FrameDeal,'=') && strchr(FrameDeal,'\n')))
				{
					//Send_Str("CMD need '=' and '\n'!");
					//Send_Str("\n");
					return 2; //return for error
				}

				cmd_index = &FrameDeal[0];
				while ('=' != *cmd_index)
				{
					cmd_index++;
					temp_len++;
				}
				if(temp_len >= 20)
				{
					//Send_Str("Invalid CMD!");
					//Send_Str("\n");
					return (2);//return for error
				}
				*cmd_index='\0';
				cmd_value = cmd_index+1;
				while ('\n' != *cmd_index)
				{
					cmd_index++;
				}
				*(cmd_index-3)='\0';
				strcpy(id,cmd_name);
				strcpy(value,cmd_value);
				memset(FrameDeal,0,30);
				return 0;
			}
			else
			{
//				Send_Str("LRC Failed!\n");
				memset(FrameDeal,0,30);
				return 2;
			}
	}				 	
	return 1;
}

/*************************************************************************************
函数名：Package
函数功能：打包要发送的一条数据指令，指令格式：":ID=VALUE(LRC校验)\r\n"
函数参数：id (数据指令要包含的ID参数 [字符串首地址]),
Value (数据指令要包含的VALUE参数 [字符串首地址])
Tempbuf (打包完成后存储的数组名)
函数返回值：无
*************************************************************************************/
void Package(unsigned char* id,unsigned char* value,unsigned char* tempbuf)
{
	int num;
	unsigned char i = 0;
	unsigned char temp[2];
	tempbuf[i++] = ':';
	tempbuf[i] = '\0';
	
	//strcat(tempbuf,':');
	strcat(tempbuf,id);
	strcat(tempbuf,"=");
	strcat(tempbuf,value);
	strcat(tempbuf,"\r\n");
	
//	strcat(tempbuf,"\0");

//		num = 0;		              //LRC   
////		i++; //Remove ":" to add
//		while(tempbuf[i] != '\0')
//		{
//			num += tempbuf[i];//num=num+sendbuf[i];
//			i++;
//		}
//		num %= 255;
//		num = (~num)+1;
//		num &= 255;			
//		//memset( temp , 0x00 , 4);
//	    sprintf(temp, "%02x", num);
//		memcpy(&tempbuf[i],&temp[0] ,2);
//		                                   //add LRC Verify

//	i += 2;
//	tempbuf[i++] = '\r';
//	tempbuf[i++] = '\n';
//	tempbuf[i] = '\0';
} 

/*************************************************************************************
函数名：my_atoi()
函数功能：字符串转换为int型数据
函数参数：str (要转换的字符串首地址)
函数返回值：转换后的int型数据
*************************************************************************************/
int my_atoi(const char * str)
{
	u16 res=0,begin=0;
 	unsigned char minus=0;
 	while(*str != '\0')
 	{
  		if(begin==0&&(('0'<=*str&&*str<='9')|| *str=='-') )   //从第一个数字或者'-'号开始
  		{
   			begin=1;
   			if(*str == '-')
   			{
    			minus=1;
    			str++;
   			}
  		}
  		else if( begin==1&&(*str<'0'||*str>'9') )     //遇到第一个非数字，退出
   		break;
  		if(begin==1)
   			res=res*10+(*str-'0');                       //计算
  			str++;
 	}
 	return minus? -res : res;
} 


/*************************************************************************************
函数名：SendBack()
函数功能：回传EEprom[50]的元素
函数参数：无
函数返回值：无
*************************************************************************************/

void SendBack_L(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[0]);							
	Package("fla",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[1]);							
	Package("alb",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
						
	tm = floor(EEprom[2]/60);
	ts = EEprom[2]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("lzm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
					
	sprintf(tempbuf,"%d",ts);							
	Package("lzs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[3]/60);
	ts = EEprom[3]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("ldm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
					
	sprintf(tempbuf,"%d",ts);							
	Package("lds",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);


}

void SendBack_H(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[4]);							
	Package("fhe",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[5]);							
	Package("ahf",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[6]/60);
	ts = EEprom[6]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("hzm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("hzs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[7]/60);
	ts = EEprom[7]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("hd1m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("hd1s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[8]/60);
	ts = EEprom[8]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("hxm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("hxs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[9]/60);
	ts = EEprom[9]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("hd2m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("hd2s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	sprintf(tempbuf,"%d",EEprom[10]);							
	Package("vjv",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
}

void SendBack_J(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[11]);							
	Package("fjm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[12]);							
	Package("ajn",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[13]/60);
	ts = EEprom[13]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("jzm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("jzs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[14]/60);
	ts = EEprom[14]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("jd1m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("jd1s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[15]/60);
	ts = EEprom[15]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("jxm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("jxs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[16]/60);
	ts = EEprom[16]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("jd2m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("jd2s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
}

void SendBack_1(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[17]);							
	Package("f1A",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[18]);							
	Package("a1B",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[19]/60);
	ts = EEprom[19]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w1zm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("w1zs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[20]/60);
	ts = EEprom[20]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w1d1m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w1d1s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[21]/60);
	ts = EEprom[21]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w1xm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w1xs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[22]/60);
	ts = EEprom[22]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w1d2m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w1d2s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
}

void SendBack_2(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[23]);							
	Package("f2G",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[24]);							
	Package("a2H",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[25]/60);
	ts = EEprom[25]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w2zm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("w2zs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[26]/60);
	ts = EEprom[26]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w2d1m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w2d1s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[27]/60);
	ts = EEprom[27]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w2xm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w2xs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[28]/60);
	ts = EEprom[28]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w2d2m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w2d2s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
}

void SendBack_3(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[29]);							
	Package("f3M",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[30]);							
	Package("a3N",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[31]/60);
	ts = EEprom[31]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w3zm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("w3zs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[32]/60);
	ts = EEprom[32]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w3d1m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w3d1s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[33]/60);
	ts = EEprom[33]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w3xm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w3xs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[34]/60);
	ts = EEprom[34]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("w3d2m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("w3d2s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

}

void SendBack_T(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[35]);							
	Package("ftS",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[36]);							
	Package("atT",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[37]/60);
	ts = EEprom[37]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("tzm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("tzs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[38]/60);
	ts = EEprom[38]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("td1m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("td1s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[39]/60);
	ts = EEprom[39]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("txm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("txs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);

	tm = floor(EEprom[40]/60);
	ts = EEprom[40]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("td2m",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	sprintf(tempbuf,"%d",ts);							
	Package("td2s",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
}

void SendBack_S(void)
{
	unsigned char tempbuf[4] = {0};
	u16 tm = 0;
	u16 ts = 0;

	sprintf(tempbuf,"%d",EEprom[41]);							
	Package("fsY",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	sprintf(tempbuf,"%d",EEprom[42]);							
	Package("asZ",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);
	
	tm = floor(EEprom[43]/60);
	ts = EEprom[43]%60;
	sprintf(tempbuf,"%d",tm);							
	Package("szm",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);					
	sprintf(tempbuf,"%d",ts);							
	Package("szs",tempbuf,SendBackBuf);
	Send_Str(SendBackBuf);	
}

void SendBack(void)
{
	SendBack_L();
	SendBack_H();
	SendBack_J();
	SendBack_1();
	SendBack_2();
	SendBack_3();
	SendBack_T();
	SendBack_S();
} 


#endif	

 



