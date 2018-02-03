/**
  *******************************************************************************************************
  * File Name: bsp.c
  * Author: Vector
  * Version: V1.0.0
  * Date: 2018 - 1 - 5
  * Brief: �弶֧�ְ����ṩ����������GPIO��TIM��ʱ��ʹ�ܺ�������RCCʱ��Դ��ȡ
  *******************************************************************************************************
  * History
  *
  *  1.Date��2018-1-5
  *		 Author: Vector
  *    Modification: �����ļ�����һ�ν�����������
  *  2.Data: 2018-1-30
  *    Author: Vector
  *    Modification: �޸��ļ�,ɾ��ԭ������,��Ϊ�����Ĵ�����ʽ,�Ա�֤��HAL��
  *                  �Լ���׼��֮���ͨ����.
  *
  *******************************************************************************************************
  */

/*
  *******************************************************************************************************
  *                              INCLUDE FILES
  *******************************************************************************************************
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
# if OS_SUPPORT < 1u
	bsp_tim_CreateTimer(1,20,TIMER_MODE_AUTO);
	bsp_tim_SetTimerCB(1,bsp_key_Scan);
# endif
	
# if USE_KEY > 0u
	bsp_key_Config();
# endif
	
# if USE_LED > 0u
	bsp_led_Config();		        
# endif
	
//# if USE_USART > 0u
//	bsp_usart_Config();
//# endif
	
# if USE_MALLOC > 0u
		bsp_mem_Config();
# endif


}


/*
*********************************************************************************************************
*                                       bsp_sys_Reset   
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
void bsp_sys_Reset(void)
{
	SCB->AIRCR =0X05FA0000|(uint32_t)0x04;	
}

	
/*
*********************************************************************************************************
*                                      bsp_nvic_SetVectorTable    
*
* Description: ����������ƫ�Ƶ�ַ
*             
* Arguments  : 1> NVIC_VectorTable: ���������ַ
*              2> Offset: ƫ����
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_nvic_SetVectorTable(uint32_t NVIC_VectorTable, uint32_t Offset)
{ 	   	  
	SCB->VTOR=NVIC_VectorTable|(Offset&(uint32_t)0xFFFFFE00);//����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]������
}


	  
/*
*********************************************************************************************************
*                                       bsp_nvic_PriorityGroupConfig
*
* Description: ����NVIC����
*             
* Arguments  : 1> NVIC_Group: NVIC����, 0~4
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_nvic_PriorityGroupConfig(uint8_t NVIC_Group)	 
{ 
	uint32_t temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}


/*
*********************************************************************************************************
*                                       bsp_nvic_Config   
*
* Description: ����NVIC 
*             
* Arguments  : 1> NVIC_PreemptionPriority:��ռ���ȼ�
*              2> NVIC_SubPriority       :��Ӧ���ȼ�
*              3> NVIC_Channel           :�жϱ��
*              4> NVIC_Group             :�жϷ��� 0~4
*
* Reutrn     : None.
*
* Note(s)    : ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
*							 �黮��:
*						   ��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
*						   ��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
*						   ��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
*						   ��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
*						   ��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
*
*********************************************************************************************************
*/
void bsp_nvic_Config(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_SubPriority,uint8_t NVIC_Channel,uint8_t NVIC_Group)
{ 
	uint32_t temp;	  
	bsp_nvic_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//������Ӧ���ȼ����������ȼ�   	    	  				   
} 


/*
*********************************************************************************************************
*                                     bsp_nvic_EXTISet     
*
* Description: �ⲿ�ж����ú���,ֻ���GPIOA~I;������PVD,RTC,USB_OTG,USB_HS,��̫�����ѵ�
*             
* Arguments  : 1> GPIOx    : 0~8,����GPIOA~I
*              2> GPIO_Pinx: ��Ҫʹ�ܵ�λ
*              3> TRIM     : ����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
*
* Reutrn     : None.
*
* Note(s)    : �ú���һ��ֻ������1��IO��,���IO��,���ε���,�ú������Զ�������Ӧ�ж�,�Լ�������
*********************************************************************************************************
*/
void bsp_nvic_EXTISet(uint8_t GPIOx, uint8_t GPIO_Pinx, uint8_t TRIM)
{ 
	uint8_t EXTOFFSET=(GPIO_Pinx%4)*4;  
	RCC->APB2ENR|=1<<14;  						//ʹ��SYSCFGʱ��  
	SYSCFG->EXTICR[GPIO_Pinx/4]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	SYSCFG->EXTICR[GPIO_Pinx/4]|=GPIOx<<EXTOFFSET;	//EXTI.BITxӳ�䵽GPIOx.BITx 
	//�Զ�����
	EXTI->IMR|=1<<GPIO_Pinx;					//����line BITx�ϵ��ж�(���Ҫ��ֹ�жϣ��򷴲�������)
	if(TRIM&0x01)EXTI->FTSR|=1<<GPIO_Pinx;	//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR|=1<<GPIO_Pinx;	//line BITx���¼��������ش���
} 	


/*
*********************************************************************************************************
*                                    bsp_gpio_AFSet      
*
* Description: GPIO��������
*             
* Arguments  : 1> GPIOx    : GPIOA~GPIOI.
*              2> GPIO_Pinx: 0~15,����IO���ű��.
*              3> GPIO_AFx : 0~15,����AF0~AF15.
*
* Reutrn     : None.
*
* Note(s)    : AF0~15�������(��������г����õ�,��ϸ�����407�����ֲ�,56ҳTable 7):
*              AF0:MCO/SWD/SWCLK/RTC   AF1:TIM1/TIM2;            AF2:TIM3~5;               AF3:TIM8~11
*              AF4:I2C1~I2C3;          AF5:SPI1/SPI2;            AF6:SPI3;                 AF7:USART1~3
*              AF8:USART4~6;           AF9;CAN1/CAN2/TIM12~14    AF10:USB_OTG/USB_HS       AF11:ETH
*              AF12:FSMC/SDIO/OTG/HS   AF13:DCIM                 AF14:                     AF15:EVENTOUT
*
*********************************************************************************************************
*/
void bsp_gpio_AFSet(GPIO_TypeDef *GPIOx, uint8_t GPIO_Pinx, uint8_t GPIO_AFx)
{  
	GPIOx->AFR[GPIO_Pinx >> 3 ]&= ~(0X0F << ((GPIO_Pinx & 0X07) * 4));
	GPIOx->AFR[GPIO_Pinx >> 3 ] |= (uint32_t)GPIO_AFx << ((GPIO_Pinx & 0X07) * 4);
}   


/*
*********************************************************************************************************
*                                    bsp_gpio_Config      
*
* Description: GPIOͨ������,����GPIO����ģʽ
*             
* Arguments  : 1> GPIOx: GPIOA~GPIOI.
*              2> GPIO_Pinx: 0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.
*                            ����0X0101,����ͬʱ����Px0��Px8.
*              3> GPIO_Mode: 0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
*              4> GPIO_OType: 0/1;�������ѡ��,0,�������;1,��©���.
*              5> GPIO_Speed: 0~3;����ٶ�����,0,2Mhz;1,25Mhz;2,50Mhz;3,100Mh.
*              6> GPIO_PuPd: 0~3:����������,0,����������;1,����;2,����;3,����.
*
* Reutrn     : None.
*
* Note(s)    : ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
*********************************************************************************************************
*/
void bsp_gpio_Config(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pinx, uint32_t GPIO_Mode, uint32_t GPIO_OType, uint32_t GPIO_Speed, uint32_t GPIO_PuPd)
{  
	uint32_t pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//һ����λ��� 
		curpin=GPIO_Pinx&pos;//��������Ƿ�Ҫ����
		if(curpin==pos)	//��Ҫ����
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->MODER|=GPIO_Mode<<(pinpos*2);	//�����µ�ģʽ 
			if((GPIO_Mode==0X01)||(GPIO_Mode==0X02))	//��������ģʽ/���ù���ģʽ
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//���ԭ��������
				GPIOx->OSPEEDR|=(GPIO_Speed<<(pinpos*2));//�����µ��ٶ�ֵ  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//���ԭ��������
				GPIOx->OTYPER|=GPIO_OType<<pinpos;		//�����µ����ģʽ
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->PUPDR|=GPIO_PuPd<<(pinpos*2);	//�����µ�������
		}
	}
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
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
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
void INTX_DISABLE(void)
{
	__ASM volatile("cpsid i");
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
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
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
__asm void MSR_MSP(uint32_t addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
/*
*********************************************************************************************************
*                                        bsp_sys_Standby  
*
* Description: ϵͳ��������ģʽ
*             
* Arguments  : None.
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/ 
void bsp_sys_Standby(void)
{ 
	SCB->SCR|=1<<2;		//ʹ��SLEEPDEEPλ (SYS->CTRL)	   
	RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ�� 
	PWR->CSR|=1<<8;     //����WKUP���ڻ���
	PWR->CR|=1<<2;      //���Wake-up ��־
	PWR->CR|=1<<1;      //PDDS��λ   	
	WFI_SET();			//ִ��WFIָ��,�������ģʽ		 
}	     


/*
*********************************************************************************************************
*                                     bsp_clk_Set     
*
* Description: ʱ�����ú���
*              
* Arguments  : 1> PLLn: ��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:64~432.
*              2> PLLm: ��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
*              3> PLLp: ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
*              4> PLLq: USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.
*
* Reutrn     : 1> 0: �ɹ�
*              2> 1: ʧ��
*
* Note(s)    : Fvco=Fs*(PLLn/PLLm); VCOƵ��
*              Fsys=Fvco/PLLp=Fs*(PLLn/(PLLm*PLLp)); ϵͳʱ��Ƶ��
*              Fusb=Fvco/PLLq=Fs*(PLLn/(PLLm*PLLq)); PLL����ʱ��Ƶ��,������HSI,HSE��. 
*              �ⲿ����Ϊ8M��ʱ��,�Ƽ�ֵ:PLLn=336,PLLm=8,PLLp=2,PLLq=7.
*							 �õ�:Fvco=8*(336/8)=336Mhz
*										Fsys=336/2=168Mhz
*										Fusb=336/7=48Mhz
*
*********************************************************************************************************
*/
uint8_t bsp_clk_Set(uint32_t PLLn, uint32_t PLLm, uint32_t PLLp, uint32_t PLLq)
{ 
	uint16_t retry=0;
	uint8_t status=0;
	RCC->CR|=1<<16;				//HSE ���� 
	while(((RCC->CR&(1<<17))==0)&&(retry<0X1FFF))retry++;//�ȴ�HSE RDY
	if(retry==0X1FFF)status=1;	//HSE�޷�����
	else   
	{
		RCC->APB1ENR|=1<<28;	//��Դ�ӿ�ʱ��ʹ��
		PWR->CR|=3<<14; 		//������ģʽ,ʱ�ӿɵ�168Mhz
		RCC->CFGR|=(0<<4)|(5<<10)|(4<<13);//HCLK ����Ƶ;APB1 4��Ƶ;APB2 2��Ƶ. 
		RCC->CR&=~(1<<24);	//�ر���PLL
		RCC->PLLCFGR=PLLm|(PLLn<<6)|(((PLLp>>1)-1)<<16)|(PLLq<<24)|(1<<22);//������PLL,PLLʱ��Դ����HSE
		RCC->CR|=1<<24;			//����PLL
		while((RCC->CR&(1<<25))==0);//�ȴ�PLL׼���� 
		FLASH->ACR|=1<<8;		//ָ��Ԥȡʹ��.
		FLASH->ACR|=1<<9;		//ָ��cacheʹ��.
		FLASH->ACR|=1<<10;		//����cacheʹ��.
		FLASH->ACR|=5<<0;		//5��CPU�ȴ�����. 
		RCC->CFGR&=~(3<<0);		//����
		RCC->CFGR|=2<<0;		//ѡ����PLL��Ϊϵͳʱ��	 
		while((RCC->CFGR&(3<<2))!=(2<<2));//�ȴ���PLL��Ϊϵͳʱ�ӳɹ�. 
	} 
	return status;
}  


/*
*********************************************************************************************************
*                               bsp_clk_Config           
*
* Description: ϵͳʱ�ӳ�ʼ������
*             
* Arguments  : 1> PLLn: ��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:64~432.
*              2> PLLm: ��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
*              3> PLLp: ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
*              4> PLLq: USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void bsp_clk_Config(uint32_t PLLn, uint32_t PLLm, uint32_t PLLp, uint32_t PLLq)
{  
	RCC->CR|=0x00000001;		//����HISON,�����ڲ�����RC��
	RCC->CFGR=0x00000000;		//CFGR���� 
	RCC->CR&=0xFEF6FFFF;		//HSEON,CSSON,PLLON���� 
	RCC->PLLCFGR=0x24003010;	//PLLCFGR�ָ���λֵ 
	RCC->CR&=~(1<<18);			//HSEBYP����,�ⲿ������·
	RCC->CIR=0x00000000;		//��ֹRCCʱ���ж� 
	bsp_clk_Set(PLLn,PLLm,PLLp,PLLq);//����ʱ�� 
	//����������				  
#ifdef  VECT_TAB_RAM
	bsp_nvic_SetVectorTable(1<<29,0x0);
#else   
	bsp_nvic_SetVectorTable(0,0x0);
#endif 
}		    





