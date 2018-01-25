/**
  ******************************************************************************
  * File Name: bsp.c
  * Author: Vector
  * Version: V1.0.0
  * Date: 2018 - 1 - 5
  * Brief: 板级支持包，提供基本外设如GPIO、TIM的时钟使能函数，及RCC时钟源获取
  ******************************************************************************
  * History
  *
  *  1.Date：2018-1-5
  *		 Author: Vector
  *    Modification: 建立文件，第一次建立基本函数
  *
  ******************************************************************************
  */

/*
  ******************************************************************************
  *                              INCLUDE FILES
  ******************************************************************************
*/
# include "bsp.h"



/*
*********************************************************************************************************
*                                          bsp_Config
*
* Description: 板级资源初始化函数，初始化如按键、定时器、串口等常用功能
*             
* Arguments  : 无
*
* Reutrn     : 无
*
* Note(s)    : 该函数的初始化功能可在bsp.h里面配置，去掉不需要使用的功能
*********************************************************************************************************
*/
void bsp_Config(void)
{	
	bsp_tim_SoftConfig();
//# if OS_SUPPORT < 1u
//	bsp_tim_CreateTimer(1,20,bsp_key_Scan, TIMER_MODE_AUTO);
//	bsp_tim_SetTimerCB(1,bsp_key_Scan);
//# endif
	
//# if USE_KEY > 0u
//	bsp_key_Config();
//# endif
	
# if USE_LED > 0u
	bsp_led_Config();		        
# endif
	
# if USE_USART > 0u
	bsp_UsartConfig();
# endif
	
# if USE_MALLOC > 0u
		bsp_mem_Config();
# endif


}


/*
*********************************************************************************************************
*                                       bsp_SoftReset   
*
* Description: 软件复位内核
*             
* Arguments  : 无
*
* Reutrn     ：无
*
* Note(s)    : 无
*********************************************************************************************************
*/
void bsp_SoftReset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

/*
*********************************************************************************************************
*                                   bsp_GetTIMRCC       
*
* Description: 获取定时器外设时钟变量定义
*             
* Arguments  : TIMx:要获取定时器时钟的定时器编号，为TIM_TypeDef结构体指针
*
* Reutrn     ：返回值为定时器外设时钟变量
*
* Note(s)    : 
*********************************************************************************************************
*/
uint32_t bsp_GetTIMRCC(TIM_TypeDef *TIMx)
{
	uint32_t rcc = 0;
	
	if(TIMx == TIM1)	rcc = RCC_APB2Periph_TIM1;
	else if(TIMx == TIM2) rcc = RCC_APB1Periph_TIM2;
	else if(TIMx == TIM3) rcc = RCC_APB1Periph_TIM3;
	else if(TIMx == TIM4) rcc = RCC_APB1Periph_TIM4;
	else if(TIMx == TIM5) rcc = RCC_APB1Periph_TIM5;
	else if(TIMx == TIM6) rcc = RCC_APB1Periph_TIM6;
	else if(TIMx == TIM7) rcc = RCC_APB1Periph_TIM7;
	else if(TIMx == TIM8) rcc = RCC_APB2Periph_TIM8;
	else if(TIMx == TIM9) rcc = RCC_APB2Periph_TIM9;
	else if(TIMx == TIM10) rcc = RCC_APB2Periph_TIM10;
	else if(TIMx == TIM11) rcc = RCC_APB2Periph_TIM11;
	else if(TIMx == TIM12) rcc = RCC_APB1Periph_TIM12;
	else if(TIMx == TIM13) rcc = RCC_APB1Periph_TIM13;
	else if(TIMx == TIM14) rcc = RCC_APB1Periph_TIM14;
	
	return rcc;
}

/*
*********************************************************************************************************
*                                      bsp_GetGPIORCC    
*
* Description: 获取GPIO外设时钟变量
*             
* Arguments : GPIOx:要获取时钟变量的GPIO端口，为GPIO_TypeDef结构体指针
*
* Reutrn：
*
* Note(s)   : 返回值为相应的GPIO端口时钟变量
*********************************************************************************************************
*/
uint32_t bsp_GetGPIORCC(GPIO_TypeDef* GPIOx)
{
	uint32_t rcc = 0;
	
# ifdef VECTOR_F1
	if(GPIOx == GPIOA) rcc = RCC_APB2Periph_GPIOA;
	else if(GPIOx == GPIOB) rcc = RCC_APB2Periph_GPIOB;
	else if(GPIOx == GPIOC) rcc = RCC_APB2Periph_GPIOC;
	else if(GPIOx == GPIOD) rcc = RCC_APB2Periph_GPIOD;
	else if(GPIOx == GPIOE) rcc = RCC_APB2Periph_GPIOE;
	else if(GPIOx == GPIOF) rcc = RCC_APB2Periph_GPIOF;
	else if(GPIOx == GPIOG) rcc = RCC_APB2Periph_GPIOG;
# elif defined VECTOR_F4
	if(GPIOx == GPIOA) rcc = RCC_AHB1Periph_GPIOA;
	else if(GPIOx == GPIOB) rcc = RCC_AHB1Periph_GPIOB;
	else if(GPIOx == GPIOC) rcc = RCC_AHB1Periph_GPIOC;
	else if(GPIOx == GPIOD) rcc = RCC_AHB1Periph_GPIOD;
	else if(GPIOx == GPIOE) rcc = RCC_AHB1Periph_GPIOE;
	else if(GPIOx == GPIOF) rcc = RCC_AHB1Periph_GPIOF;
	else if(GPIOx == GPIOH) rcc = RCC_AHB1Periph_GPIOH;
	else if(GPIOx == GPIOI) rcc = RCC_AHB1Periph_GPIOI;
	else if(GPIOx == GPIOG) rcc = RCC_AHB1Periph_GPIOG;
# endif
	return rcc;
}


uint32_t bsp_GetADCRCC(ADC_TypeDef* ADCx)
{
	uint32_t rcc = 0;
	if(ADC1 == ADCx) rcc = RCC_APB2Periph_ADC1;
	else if(ADC2 == ADCx) rcc = RCC_APB2Periph_ADC2;
	else if(ADC3 == ADCx) rcc = RCC_APB2Periph_ADC3;
	return rcc;
}

/*
*********************************************************************************************************
*                                       bsp_GetTIMPinAF   
*
* Description: 当将GPIO配置成定时器模式时，可以通过该函数获取复用定时器
*             
* Arguments  : TIMx:定时器编号
*
* Reutrn     : 引脚复用为定时器的编号
*
* Note(s)    : 
*********************************************************************************************************
*/
# ifdef VECTOR_F4
uint8_t bsp_GetTIMPinAF(TIM_TypeDef *TIMx)
{
	uint8_t pinAF = 0;
	if(TIMx == TIM1)	pinAF = GPIO_AF_TIM1;
	else if(TIMx == TIM2) pinAF = GPIO_AF_TIM2;
	else if(TIMx == TIM3) pinAF = GPIO_AF_TIM3;
	else if(TIMx == TIM4) pinAF = GPIO_AF_TIM4;
	else if(TIMx == TIM5) pinAF = GPIO_AF_TIM5;
	else if(TIMx == TIM8) pinAF = GPIO_AF_TIM8;
	else if(TIMx == TIM9) pinAF = GPIO_AF_TIM9;
	else if(TIMx == TIM10) pinAF = GPIO_AF_TIM10;
	else if(TIMx == TIM11) pinAF = GPIO_AF_TIM11;
	else if(TIMx == TIM12) pinAF = GPIO_AF_TIM12;
	else if(TIMx == TIM13) pinAF = GPIO_AF_TIM13;
	else if(TIMx == TIM14) pinAF = GPIO_AF_TIM14;
	
	return pinAF;
}



/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t bsp_GetSPIPinAF(SPI_TypeDef* SPIx)
{
	uint8_t pinAF = 0;
	if(SPIx == SPI1)	pinAF = GPIO_AF_SPI1;
	else if(SPIx == SPI2) pinAF = GPIO_AF_SPI2;
	else if(SPIx == SPI3) pinAF = GPIO_AF_SPI3;
	else if(SPIx == SPI4) pinAF = GPIO_AF_SPI4;
	else if(SPIx == SPI5) pinAF = GPIO_AF_SPI5;
	else if(SPIx == SPI6) pinAF = GPIO_AF_SPI6;
	
	return pinAF;
}


# endif
/*
*********************************************************************************************************
*                                         bsp_GetPinSource 
*
* Description: 获取GPIO引脚资源
*             
* Arguments  : GPIO_Pin:GPIO引脚编号，只能输入单个引脚
*
* Reutrn     : 相应引脚资源
*
* Note(s)    : 无
*********************************************************************************************************
*/
uint16_t bsp_GetPinSource(uint16_t GPIO_Pin)
{
	uint16_t PinSource = 0;
	switch(GPIO_Pin)
	{
		case GPIO_Pin_0:PinSource = GPIO_PinSource0;break;
		case GPIO_Pin_1:PinSource = GPIO_PinSource1;break;
		case GPIO_Pin_2:PinSource = GPIO_PinSource2;break;
		case GPIO_Pin_3:PinSource = GPIO_PinSource3;break;
		case GPIO_Pin_4:PinSource = GPIO_PinSource4;break;
		case GPIO_Pin_5:PinSource = GPIO_PinSource5;break;
		case GPIO_Pin_6:PinSource = GPIO_PinSource6;break;
		case GPIO_Pin_7:PinSource = GPIO_PinSource7;break;
		case GPIO_Pin_8:PinSource = GPIO_PinSource8;break;
		case GPIO_Pin_9:PinSource = GPIO_PinSource9;break;
		case GPIO_Pin_10:PinSource = GPIO_PinSource10;break;
		case GPIO_Pin_11:PinSource = GPIO_PinSource11;break;
		case GPIO_Pin_12:PinSource = GPIO_PinSource12;break;
		case GPIO_Pin_13:PinSource = GPIO_PinSource13;break;
		case GPIO_Pin_14:PinSource = GPIO_PinSource14;break;
		case GPIO_Pin_15:	PinSource = GPIO_PinSource15;break;		
	}
	return PinSource;
}


/*
*********************************************************************************************************
*                                          bsp_TIMClockCmd
*
* Description: 定时器外设时钟使能函数
*             
* Arguments  : TIMx:定时器编号，为TIM_TypeDef结构体指针
*
* Reutrn     : 无
*
* Note(s)    : 无
*********************************************************************************************************
*/
void bsp_TIMClockCmd(TIM_TypeDef *TIMx)
{
	if(TIMx == TIM2 || TIMx == TIM3) RCC_APB1PeriphClockCmd(bsp_GetTIMRCC(TIMx), ENABLE);
# if defined (STM32F10X_HD)|| defined (STM32F10X_MD) || defined (VECTOR_F4)
	else if(TIMx == TIM4 || TIMx == TIM5 || TIMx == TIM6) RCC_APB1PeriphClockCmd(bsp_GetTIMRCC(TIMx), ENABLE);
# endif
	
# ifdef VECTOR_F4
	else if(TIMx == TIM12 || TIMx == TIM13 || TIMx == TIM14 ) RCC_APB1PeriphClockCmd(bsp_GetTIMRCC(TIMx), ENABLE);
	else if(TIMx == TIM8 || TIMx == TIM9 || TIMx == TIM10 || TIMx == TIM11)
		 RCC_APB2PeriphClockCmd(bsp_GetTIMRCC(TIMx), ENABLE);
# endif
		
}


/*
*********************************************************************************************************
*                                          bsp_GPIOClcokCmd
*
* Description: GPIO端口时钟使能函数
*             
* Arguments  : GPIOx:要使能的GPIO端口
*
* Reutrn     : 无
*
* Note(s)    : 无
*********************************************************************************************************
*/
void bsp_GPIOClcokCmd(GPIO_TypeDef* GPIOx)
{
# ifdef VECTOR_F1
	RCC_APB2PeriphClockCmd(bsp_GetGPIORCC(GPIOx), ENABLE);
# elif defined VECTOR_F4
	RCC_AHB1PeriphClockCmd(bsp_GetGPIORCC(GPIOx), ENABLE);
# endif
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Reutrn:
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_ADCClockCmd(ADC_TypeDef* ADCx)
{
	RCC_APB2PeriphClockCmd(bsp_GetADCRCC(ADCx), ENABLE);
}




/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Reutrn:
*
* Note(s)   : 
*********************************************************************************************************
*/
uint32_t bsp_GetSPIRCC(SPI_TypeDef *SPIx)
{
	uint32_t rcc = 0;
	if(SPIx == SPI1) rcc = RCC_APB2Periph_SPI1;
	else if(SPIx == SPI2) rcc = RCC_APB1Periph_SPI2;
	else if(SPIx == SPI3) rcc = RCC_APB1Periph_SPI3;
# ifdef VECTOR_F4
	else if(SPIx == SPI4) rcc = RCC_APB2Periph_SPI4;
	else if(SPIx == SPI5) rcc = RCC_APB2Periph_SPI5;
	else if(SPIx == SPI6) rcc = RCC_APB2Periph_SPI6;
# endif
	return rcc;
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Reutrn:
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_SPIClockCmd(SPI_TypeDef* SPIx)
{
# ifdef VECTOR_F1
	if(SPIx == SPI1)
		RCC_APB2PeriphClockCmd(bsp_GetSPIRCC(SPIx), ENABLE);
	else if(SPIx == SPI2 || SPIx == SPI3)
		RCC_APB1PeriphClockCmd(bsp_GetSPIRCC(SPIx), ENABLE);
# elif defined VECTOR_F4
	if(SPIx == SPI1 || SPIx == SPI4 || SPIx == SPI5 || SPIx == SPI6)
		RCC_APB2PeriphClockCmd(bsp_GetSPIRCC(SPIx), ENABLE);
	else if(SPIx == SPI2 || SPIx == SPI3)
		RCC_APB1PeriphClockCmd(bsp_GetSPIRCC(SPIx), ENABLE);
# endif
}


//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量		 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}
//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//取低四位
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//设置响应优先级和抢断优先级   	    	  				   
} 
//外部中断配置函数
//只针对GPIOA~I;不包括PVD,RTC,USB_OTG,USB_HS,以太网唤醒等
//参数:
//GPIOx:0~8,代表GPIOA~I
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET=(BITx%4)*4;  
	RCC->APB2ENR|=1<<14;  						//使能SYSCFG时钟  
	SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;	//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR|=1<<BITx;					//开启line BITx上的中断(如果要禁止中断，则反操作即可)
	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;	//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;	//line BITx上事件上升降沿触发
} 	
//GPIO复用设置
//GPIOx:GPIOA~GPIOI.
//BITx:0~15,代表IO引脚编号.
//AFx:0~15,代表AF0~AF15.
//AF0~15设置情况(这里仅是列出常用的,详细的请见407数据手册,56页Table 7):
//AF0:MCO/SWD/SWCLK/RTC   AF1:TIM1/TIM2;            AF2:TIM3~5;               AF3:TIM8~11
//AF4:I2C1~I2C3;          AF5:SPI1/SPI2;            AF6:SPI3;                 AF7:USART1~3;
//AF8:USART4~6;           AF9;CAN1/CAN2/TIM12~14    AF10:USB_OTG/USB_HS       AF11:ETH
//AF12:FSMC/SDIO/OTG/HS   AF13:DCIM                 AF14:                     AF15:EVENTOUT
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
}   
//GPIO通用设置 
//GPIOx:GPIOA~GPIOI.
//BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
//MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
//OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
//OSPEED:0~3;输出速度设置,0,2Mhz;1,25Mhz;2,50Mhz;3,100Mh. 
//PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
//注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=BITx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=MODE<<(pinpos*2);	//设置新的模式 
			if((MODE==0X01)||(MODE==0X02))	//如果是输出模式/复用功能模式
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//设置新的速度值  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
				GPIOx->OTYPER|=OTYPE<<pinpos;		//设置新的输出模式
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//设置新的上下拉
		}
	}
} 


