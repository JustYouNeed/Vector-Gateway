/**
  ******************************************************************************
  * File Name: bsp.c
  * Author: Vector
  * Version: V1.0.0
  * Date: 2018 - 1 - 5
  * Brief: �弶֧�ְ����ṩ����������GPIO��TIM��ʱ��ʹ�ܺ�������RCCʱ��Դ��ȡ
  ******************************************************************************
  * History
  *
  *  1.Date��2018-1-5
  *		 Author: Vector
  *    Modification: �����ļ�����һ�ν�����������
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
* Description: �弶��Դ��ʼ����������ʼ���簴������ʱ�������ڵȳ��ù���
*             
* Arguments  : ��
*
* Reutrn     : ��
*
* Note(s)    : �ú����ĳ�ʼ�����ܿ���bsp.h�������ã�ȥ������Ҫʹ�õĹ���
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
* Description: �����λ�ں�
*             
* Arguments  : ��
*
* Reutrn     ����
*
* Note(s)    : ��
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
* Description: ��ȡ��ʱ������ʱ�ӱ�������
*             
* Arguments  : TIMx:Ҫ��ȡ��ʱ��ʱ�ӵĶ�ʱ����ţ�ΪTIM_TypeDef�ṹ��ָ��
*
* Reutrn     ������ֵΪ��ʱ������ʱ�ӱ���
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
* Description: ��ȡGPIO����ʱ�ӱ���
*             
* Arguments : GPIOx:Ҫ��ȡʱ�ӱ�����GPIO�˿ڣ�ΪGPIO_TypeDef�ṹ��ָ��
*
* Reutrn��
*
* Note(s)   : ����ֵΪ��Ӧ��GPIO�˿�ʱ�ӱ���
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
* Description: ����GPIO���óɶ�ʱ��ģʽʱ������ͨ���ú�����ȡ���ö�ʱ��
*             
* Arguments  : TIMx:��ʱ�����
*
* Reutrn     : ���Ÿ���Ϊ��ʱ���ı��
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
* Description: ��ȡGPIO������Դ
*             
* Arguments  : GPIO_Pin:GPIO���ű�ţ�ֻ�����뵥������
*
* Reutrn     : ��Ӧ������Դ
*
* Note(s)    : ��
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
* Description: ��ʱ������ʱ��ʹ�ܺ���
*             
* Arguments  : TIMx:��ʱ����ţ�ΪTIM_TypeDef�ṹ��ָ��
*
* Reutrn     : ��
*
* Note(s)    : ��
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
* Description: GPIO�˿�ʱ��ʹ�ܺ���
*             
* Arguments  : GPIOx:Ҫʹ�ܵ�GPIO�˿�
*
* Reutrn     : ��
*
* Note(s)    : ��
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


//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����		 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]������
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//������Ӧ���ȼ����������ȼ�   	    	  				   
} 
//�ⲿ�ж����ú���
//ֻ���GPIOA~I;������PVD,RTC,USB_OTG,USB_HS,��̫�����ѵ�
//����:
//GPIOx:0~8,����GPIOA~I
//BITx:��Ҫʹ�ܵ�λ;
//TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET=(BITx%4)*4;  
	RCC->APB2ENR|=1<<14;  						//ʹ��SYSCFGʱ��  
	SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;	//EXTI.BITxӳ�䵽GPIOx.BITx 
	//�Զ�����
	EXTI->IMR|=1<<BITx;					//����line BITx�ϵ��ж�(���Ҫ��ֹ�жϣ��򷴲�������)
	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;	//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;	//line BITx���¼��������ش���
} 	
//GPIO��������
//GPIOx:GPIOA~GPIOI.
//BITx:0~15,����IO���ű��.
//AFx:0~15,����AF0~AF15.
//AF0~15�������(��������г����õ�,��ϸ�����407�����ֲ�,56ҳTable 7):
//AF0:MCO/SWD/SWCLK/RTC   AF1:TIM1/TIM2;            AF2:TIM3~5;               AF3:TIM8~11
//AF4:I2C1~I2C3;          AF5:SPI1/SPI2;            AF6:SPI3;                 AF7:USART1~3;
//AF8:USART4~6;           AF9;CAN1/CAN2/TIM12~14    AF10:USB_OTG/USB_HS       AF11:ETH
//AF12:FSMC/SDIO/OTG/HS   AF13:DCIM                 AF14:                     AF15:EVENTOUT
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
}   
//GPIOͨ������ 
//GPIOx:GPIOA~GPIOI.
//BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
//MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
//OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
//OSPEED:0~3;����ٶ�����,0,2Mhz;1,25Mhz;2,50Mhz;3,100Mh. 
//PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
//ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//һ����λ��� 
		curpin=BITx&pos;//��������Ƿ�Ҫ����
		if(curpin==pos)	//��Ҫ����
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->MODER|=MODE<<(pinpos*2);	//�����µ�ģʽ 
			if((MODE==0X01)||(MODE==0X02))	//��������ģʽ/���ù���ģʽ
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//���ԭ��������
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//�����µ��ٶ�ֵ  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//���ԭ��������
				GPIOx->OTYPER|=OTYPE<<pinpos;		//�����µ����ģʽ
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//�����µ�������
		}
	}
} 


