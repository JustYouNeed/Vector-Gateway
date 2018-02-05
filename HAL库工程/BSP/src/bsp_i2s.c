# include "bsp_i2s.h"
# include "includes.h"



//采样率计算公式:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//一般HSE=8Mhz 
//pllm:在Sys_Clock_Set设置的时候确定，一般是8
//PLLI2SN:一般是192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S分频系数表@pllm=8,HSE=8Mhz,即vco输入频率为1Mhz
//表格式:采样率/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const uint16_t I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz采样率
	{1102,429,4,19,0},		//11.025Khz采样率 
	{1600,213,2,13,0},		//16Khz采样率
	{2205,429,4, 9,1},		//22.05Khz采样率
	{3200,213,2, 6,1},		//32Khz采样率
	{4410,271,2, 6,0},		//44.1Khz采样率
	{4800,258,3, 3,1},		//48Khz采样率
	{8820,316,2, 3,1},		//88.2Khz采样率
	{9600,344,2, 3,1},  	//96Khz采样率
	{17640,361,2,2,0},  	//176.4Khz采样率 
	{19200,393,2,2,0},  	//192Khz采样率
};  


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
void bsp_i2s_Config(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
	RCC->APB1ENR |= 1 << 14;  /*  使能SPI2时钟  */
	
	RCC->APB1RSTR |= 1 << 14;		/*  复位SPI2  */
	RCC->APB1RSTR &= ~(1 << 14);		/*  结束复位  */
	
	SPI2->I2SCFGR = 0;
	SPI2->I2SPR = 0x02;		/*  分频器设置默认值  */
	SPI2->I2SCFGR |= 1 << 11;		/*  选择为I2S模式  */
	SPI2->I2SCFGR |= (uint16_t)I2S_Mode << 8;		/*  设置I2S工作模式  */
	SPI1->I2SCFGR |= I2S_Standard << 4;		/*  I2S标准设置  */
	SPI2->I2SCFGR |= I2S_Clock_Polarity;		/*  空闲时钟电平设置  */
	
	if(I2S_DataFormat)		/*  如果不是标准的16位长度  */
	{
		SPI2->I2SCFGR |= 1 << 0;		/*  通道长度设置为32位  */
		I2S_DataFormat -= 1;
	}
	else
		SPI2->I2SCFGR |= 0 << 0;	/*  通道长度设置为16位  */
	
	SPI2->I2SCFGR |= I2S_DataFormat << 1;	/*  I2S标准设置  */
	SPI2->CR2 |= 1 << 1;		/*  使能SPI2发送DMA  */
	SPI2->I2SCFGR |= 1 << 10;		/*  SPI2 I2S使能  */
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
void bsp_i2s_ExtConfig(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
	I2S2ext->I2SCFGR=0;				//全部设置为0
	I2S2ext->I2SPR=0X02;			//分频寄存器为默认值
	I2S2ext->I2SCFGR|=1<<11;		//选择:I2S模式
	I2S2ext->I2SCFGR|=(uint16_t)I2S_Mode<<8;	//I2S工作模式设置
	I2S2ext->I2SCFGR|=I2S_Standard<<4;		//I2S标准设置 
	I2S2ext->I2SCFGR|=I2S_Clock_Polarity<<3;		//空闲时钟电平设置
	if(I2S_DataFormat)						//非标准16位长度
	{
		I2S2ext->I2SCFGR|=1<<0;		//Channel长度为32位 
		I2S_DataFormat-=1;		
	}else I2S2ext->I2SCFGR|=0<<0;	//Channel长度为16位  
	I2S2ext->I2SCFGR|=I2S_DataFormat<<1;	//I2S标准设置 
	I2S2ext->CR2|=1<<0;				//I2S2ext RX DMA请求使能.
	I2S2ext->I2SCFGR|=1<<10;		//I2S2ext I2S EN使能.	
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
uint8_t bsp_i2s_SampleRateSet(uint32_t samplerate)
{
	uint8_t i=0; 
	uint32_t tempreg=0;
	samplerate/=10;//缩小10倍   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC->CR &= ~(1 << 26);  /*  配置之前需要关闭I2S  */
	
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	
	tempreg |= (uint32_t)I2S_PSC_TBL[i][1] << 6;  /*  设置PLLI2SN  */
	tempreg |= (uint32_t)I2S_PSC_TBL[i][2] << 28;  /*  设置PLLI2SR  */
	RCC->PLLI2SCFGR = tempreg;   /*  设置I2S2CLK的时钟频率  */
	RCC->CR |= 1 << 26;					//开启I2S时钟
	while((RCC->CR & 1 << 27) == 0);		//等待I2S时钟开启成功. 
	tempreg = I2S_PSC_TBL[i][3] << 0;	//设置I2SDIV
	tempreg |= I2S_PSC_TBL[i][4] << 8;	//设置ODD位
	tempreg |= 1 << 9;					//使能MCKOE位,输出MCK
	SPI2->I2SPR = tempreg;			//设置I2SPR寄存器 
	return 0;
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
void bsp_i2s_TX_DMAConfig(uint8_t* buf0,uint8_t *buf1,uint16_t num)
{
	RCC->AHB1ENR |= 1 << 21;				/*  使能DMA  */
	
	while(DMA1_Stream4->CR & 0x01);		/*  等待DMA Stream可配置  */
	
	DMA1->HIFCR |= 0x3d << (6 * 0);			/*  清空通道4上所有的中断标志  */
	DMA1_Stream4->FCR = 0x00000021;		/*  设置为默认值  */
	DMA1_Stream4->PAR = (uint32_t)&SPI2->DR;		/*  设置外设地址为SPI2->DR  */
	DMA1_Stream4->M0AR = (uint32_t)buf0;		/*  内存地址1  */
	DMA1_Stream4->M1AR = (uint32_t)buf1;		/*  内存地址2  */
	DMA1_Stream4->NDTR = num;			/*  设置数据长度  */
	
	DMA1_Stream4->CR = 0;		/*  复位  */
	DMA1_Stream4->CR |= 1 << 6;		/*  模式为存储器到外设  */
	DMA1_Stream4->CR |= 1 << 8;		/*  循环模式  */
	DMA1_Stream4->CR |= 0 << 9;		/*  外设非增量模式  */
	DMA1_Stream4->CR |= 1 << 10;	/*  存储器增量模式  */
	DMA1_Stream4->CR |= 1 << 11;	/*  外设数据长度为16位  */
	DMA1_Stream4->CR |= 1 << 13;	/*  存储器长度16位  */
	DMA1_Stream4->CR |= 2 << 16;	/*  高优先级  */
	DMA1_Stream4->CR |= 1 << 18;	/*  双缓冲模式  */
	DMA1_Stream4->CR |= 0 << 21;	/*  外设突发单次传输  */
	DMA1_Stream4->CR |= 0 << 23;	/*  存储器突发单次传输  */
	DMA1_Stream4->CR |= 0 << 25;	/*  选择通道0 SPI2_TX通道  */
	DMA1_Stream4->FCR &= ~(1 << 2);		/*  不使用FIFO模式 */
	DMA1_Stream4->FCR &= ~(3 << 0);		/*  无FIFO设置  */
	DMA1_Stream4->CR |= 1 << 4;		/*  开启传输完成中断  */
	
	bsp_nvic_Config(0, 0, DMA1_Stream4_IRQn, 2);
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
void bsp_i2s_PlayStart(void)
{
	DMA1_Stream4->CR |= 1 << 0;	/*  开启DMA传输  */
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
void bsp_i2s_PlayStop(void)
{
	DMA1_Stream4->CR &= ~(1 << 0); 	/*  关闭DMA传输  */
}


void (*i2s_tx_callback)(void);	
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
//void DMA1_Stream4_IRQHandler(void)
//{    
//OSIntEnter();    
//if(DMA1->HISR & (1 << 5))   /*  DMA1_Stream4,传输完成标志  */
//{ 
//	DMA1->HIFCR |= 1 << 5;
//			i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
//}   
//OSIntExit(); 	
//} 

