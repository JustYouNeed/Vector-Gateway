/**
  *******************************************************************************************************
  * File Name: bsp.c
  * Author: Vector
  * Version: V1.0.0
  * Date: 2018 - 1 - 5
  * Brief: 板级支持包，提供基本外设如GPIO、TIM的时钟使能函数，及RCC时钟源获取
  *******************************************************************************************************
  * History
  *
  *  1.Date：2018-1-5
  *		 Author: Vector
  *    Modification: 建立文件，第一次建立基本函数
  *  2.Data: 2018-1-30
  *    Author: Vector
  *    Modification: 修改文件,删除原本函数,改为操作寄存器方式,以保证在HAL库
  *                  以及标准库之间的通用性.
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
* Description: 软件复位内核
*             
* Arguments  : 无
*
* Reutrn     ：无
*
* Note(s)    : 无
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
* Description: 设置向量表偏移地址
*             
* Arguments  : 1> NVIC_VectorTable: 向量表基地址
*              2> Offset: 偏移量
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_nvic_SetVectorTable(uint32_t NVIC_VectorTable, uint32_t Offset)
{ 	   	  
	SCB->VTOR=NVIC_VectorTable|(Offset&(uint32_t)0xFFFFFE00);//设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}


	  
/*
*********************************************************************************************************
*                                       bsp_nvic_PriorityGroupConfig
*
* Description: 设置NVIC分组
*             
* Arguments  : 1> NVIC_Group: NVIC分组, 0~4
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_nvic_PriorityGroupConfig(uint8_t NVIC_Group)	 
{ 
	uint32_t temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}


/*
*********************************************************************************************************
*                                       bsp_nvic_Config   
*
* Description: 设置NVIC 
*             
* Arguments  : 1> NVIC_PreemptionPriority:抢占优先级
*              2> NVIC_SubPriority       :响应优先级
*              3> NVIC_Channel           :中断编号
*              4> NVIC_Group             :中断分组 0~4
*
* Reutrn     : None.
*
* Note(s)    : 注意优先级不能超过设定的组的范围!否则会有意想不到的错误
*							 组划分:
*						   组0:0位抢占优先级,4位响应优先级
*						   组1:1位抢占优先级,3位响应优先级
*						   组2:2位抢占优先级,2位响应优先级
*						   组3:3位抢占优先级,1位响应优先级
*						   组4:4位抢占优先级,0位响应优先级
*
*********************************************************************************************************
*/
void bsp_nvic_Config(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_SubPriority,uint8_t NVIC_Channel,uint8_t NVIC_Group)
{ 
	uint32_t temp;	  
	bsp_nvic_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//取低四位
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//设置响应优先级和抢断优先级   	    	  				   
} 


/*
*********************************************************************************************************
*                                     bsp_nvic_EXTISet     
*
* Description: 外部中断配置函数,只针对GPIOA~I;不包括PVD,RTC,USB_OTG,USB_HS,以太网唤醒等
*             
* Arguments  : 1> GPIOx    : 0~8,代表GPIOA~I
*              2> GPIO_Pinx: 需要使能的位
*              3> TRIM     : 触发模式,1,下升沿;2,上降沿;3，任意电平触发
*
* Reutrn     : None.
*
* Note(s)    : 该函数一次只能配置1个IO口,多个IO口,需多次调用,该函数会自动开启对应中断,以及屏蔽线
*********************************************************************************************************
*/
void bsp_nvic_EXTISet(uint8_t GPIOx, uint8_t GPIO_Pinx, uint8_t TRIM)
{ 
	uint8_t EXTOFFSET=(GPIO_Pinx%4)*4;  
	RCC->APB2ENR|=1<<14;  						//使能SYSCFG时钟  
	SYSCFG->EXTICR[GPIO_Pinx/4]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	SYSCFG->EXTICR[GPIO_Pinx/4]|=GPIOx<<EXTOFFSET;	//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR|=1<<GPIO_Pinx;					//开启line BITx上的中断(如果要禁止中断，则反操作即可)
	if(TRIM&0x01)EXTI->FTSR|=1<<GPIO_Pinx;	//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR|=1<<GPIO_Pinx;	//line BITx上事件上升降沿触发
} 	


/*
*********************************************************************************************************
*                                    bsp_gpio_AFSet      
*
* Description: GPIO复用设置
*             
* Arguments  : 1> GPIOx    : GPIOA~GPIOI.
*              2> GPIO_Pinx: 0~15,代表IO引脚编号.
*              3> GPIO_AFx : 0~15,代表AF0~AF15.
*
* Reutrn     : None.
*
* Note(s)    : AF0~15设置情况(这里仅是列出常用的,详细的请见407数据手册,56页Table 7):
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
* Description: GPIO通用设置,配置GPIO引脚模式
*             
* Arguments  : 1> GPIOx: GPIOA~GPIOI.
*              2> GPIO_Pinx: 0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.
*                            比如0X0101,代表同时设置Px0和Px8.
*              3> GPIO_Mode: 0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
*              4> GPIO_OType: 0/1;输出类型选择,0,推挽输出;1,开漏输出.
*              5> GPIO_Speed: 0~3;输出速度设置,0,2Mhz;1,25Mhz;2,50Mhz;3,100Mh.
*              6> GPIO_PuPd: 0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
*
* Reutrn     : None.
*
* Note(s)    : 注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
*********************************************************************************************************
*/
void bsp_gpio_Config(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pinx, uint32_t GPIO_Mode, uint32_t GPIO_OType, uint32_t GPIO_Speed, uint32_t GPIO_PuPd)
{  
	uint32_t pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=GPIO_Pinx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=GPIO_Mode<<(pinpos*2);	//设置新的模式 
			if((GPIO_Mode==0X01)||(GPIO_Mode==0X02))	//如果是输出模式/复用功能模式
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
				GPIOx->OSPEEDR|=(GPIO_Speed<<(pinpos*2));//设置新的速度值  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
				GPIOx->OTYPER|=GPIO_OType<<pinpos;		//设置新的输出模式
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=GPIO_PuPd<<(pinpos*2);	//设置新的上下拉
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
* Description: 系统进入休眠模式
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
	SCB->SCR|=1<<2;		//使能SLEEPDEEP位 (SYS->CTRL)	   
	RCC->APB1ENR|=1<<28;//使能电源时钟 
	PWR->CSR|=1<<8;     //设置WKUP用于唤醒
	PWR->CR|=1<<2;      //清除Wake-up 标志
	PWR->CR|=1<<1;      //PDDS置位   	
	WFI_SET();			//执行WFI指令,进入待机模式		 
}	     


/*
*********************************************************************************************************
*                                     bsp_clk_Set     
*
* Description: 时钟设置函数
*              
* Arguments  : 1> PLLn: 主PLL倍频系数(PLL倍频),取值范围:64~432.
*              2> PLLm: 主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
*              3> PLLp: 系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
*              4> PLLq: USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
*
* Reutrn     : 1> 0: 成功
*              2> 1: 失败
*
* Note(s)    : Fvco=Fs*(PLLn/PLLm); VCO频率
*              Fsys=Fvco/PLLp=Fs*(PLLn/(PLLm*PLLp)); 系统时钟频率
*              Fusb=Fvco/PLLq=Fs*(PLLn/(PLLm*PLLq)); PLL输入时钟频率,可以是HSI,HSE等. 
*              外部晶振为8M的时候,推荐值:PLLn=336,PLLm=8,PLLp=2,PLLq=7.
*							 得到:Fvco=8*(336/8)=336Mhz
*										Fsys=336/2=168Mhz
*										Fusb=336/7=48Mhz
*
*********************************************************************************************************
*/
uint8_t bsp_clk_Set(uint32_t PLLn, uint32_t PLLm, uint32_t PLLp, uint32_t PLLq)
{ 
	uint16_t retry=0;
	uint8_t status=0;
	RCC->CR|=1<<16;				//HSE 开启 
	while(((RCC->CR&(1<<17))==0)&&(retry<0X1FFF))retry++;//等待HSE RDY
	if(retry==0X1FFF)status=1;	//HSE无法就绪
	else   
	{
		RCC->APB1ENR|=1<<28;	//电源接口时钟使能
		PWR->CR|=3<<14; 		//高性能模式,时钟可到168Mhz
		RCC->CFGR|=(0<<4)|(5<<10)|(4<<13);//HCLK 不分频;APB1 4分频;APB2 2分频. 
		RCC->CR&=~(1<<24);	//关闭主PLL
		RCC->PLLCFGR=PLLm|(PLLn<<6)|(((PLLp>>1)-1)<<16)|(PLLq<<24)|(1<<22);//配置主PLL,PLL时钟源来自HSE
		RCC->CR|=1<<24;			//打开主PLL
		while((RCC->CR&(1<<25))==0);//等待PLL准备好 
		FLASH->ACR|=1<<8;		//指令预取使能.
		FLASH->ACR|=1<<9;		//指令cache使能.
		FLASH->ACR|=1<<10;		//数据cache使能.
		FLASH->ACR|=5<<0;		//5个CPU等待周期. 
		RCC->CFGR&=~(3<<0);		//清零
		RCC->CFGR|=2<<0;		//选择主PLL作为系统时钟	 
		while((RCC->CFGR&(3<<2))!=(2<<2));//等待主PLL作为系统时钟成功. 
	} 
	return status;
}  


/*
*********************************************************************************************************
*                               bsp_clk_Config           
*
* Description: 系统时钟初始化函数
*             
* Arguments  : 1> PLLn: 主PLL倍频系数(PLL倍频),取值范围:64~432.
*              2> PLLm: 主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
*              3> PLLp: 系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
*              4> PLLq: USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void bsp_clk_Config(uint32_t PLLn, uint32_t PLLm, uint32_t PLLp, uint32_t PLLq)
{  
	RCC->CR|=0x00000001;		//设置HISON,开启内部高速RC振荡
	RCC->CFGR=0x00000000;		//CFGR清零 
	RCC->CR&=0xFEF6FFFF;		//HSEON,CSSON,PLLON清零 
	RCC->PLLCFGR=0x24003010;	//PLLCFGR恢复复位值 
	RCC->CR&=~(1<<18);			//HSEBYP清零,外部晶振不旁路
	RCC->CIR=0x00000000;		//禁止RCC时钟中断 
	bsp_clk_Set(PLLn,PLLm,PLLp,PLLq);//设置时钟 
	//配置向量表				  
#ifdef  VECT_TAB_RAM
	bsp_nvic_SetVectorTable(1<<29,0x0);
#else   
	bsp_nvic_SetVectorTable(0,0x0);
#endif 
}		    





