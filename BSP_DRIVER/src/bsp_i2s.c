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
const u16 I2S_PSC_TBL[][5]=
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
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_Config(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
	I2S_InitTypeDef I2S_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//使能SPI2时钟
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE); //复位SPI2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//结束复位
  
	I2S_InitStructure.I2S_Mode=I2S_Mode;//IIS模式
	I2S_InitStructure.I2S_Standard=I2S_Standard;//IIS标准
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;//IIS数据长度
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//主时钟输出禁止
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;//IIS频率设置
	I2S_InitStructure.I2S_CPOL=I2S_Clock_Polarity;//空闲状态时钟电平
	I2S_Init(SPI2,&I2S_InitStructure);//初始化IIS

 
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA请求使能.
  I2S_Cmd(SPI2,ENABLE);//SPI2 I2S EN使能.	
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_ExtConfig(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
		I2S_InitTypeDef I2S2ext_InitStructure;
	
	I2S2ext_InitStructure.I2S_Mode=I2S_Mode^(1<<8);//IIS模式
	I2S2ext_InitStructure.I2S_Standard=I2S_Standard;//IIS标准
	I2S2ext_InitStructure.I2S_DataFormat=I2S_DataFormat;//IIS数据长度
	I2S2ext_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;//主时钟输出禁止
	I2S2ext_InitStructure.I2S_AudioFreq=I2S_AudioFreq_Default;//IIS频率设置
	I2S2ext_InitStructure.I2S_CPOL=I2S_Clock_Polarity;//空闲状态时钟电平
	
	I2S_FullDuplexConfig(I2S2ext,&I2S2ext_InitStructure);//初始化I2S2ext配置
	
	SPI_I2S_DMACmd(I2S2ext,SPI_I2S_DMAReq_Rx,ENABLE);//I2S2ext RX DMA请求使能.
 
	I2S_Cmd(I2S2ext,ENABLE);		//I2S2ext I2S EN使能.
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t bsp_i2s_SampleRateSet(uint32_t samplerate)
{
		u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//缩小10倍   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//看看改采样率是否可以支持
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC_PLLI2SCmd(DISABLE);//先关闭PLLI2S
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//搜遍了也找不到
	RCC_PLLI2SConfig((u32)I2S_PSC_TBL[i][1],(u32)I2S_PSC_TBL[i][2]);//设置I2SxCLK的频率(x=2)  设置PLLI2SN PLLI2SR
 
	RCC->CR|=1<<26;					//开启I2S时钟
	while((RCC->CR&1<<27)==0);		//等待I2S时钟开启成功. 
	tempreg=I2S_PSC_TBL[i][3]<<0;	//设置I2SDIV
	tempreg|=I2S_PSC_TBL[i][4]<<8;	//设置ODD位
	tempreg|=1<<9;					//使能MCKOE位,输出MCK
	SPI2->I2SPR=tempreg;			//设置I2SPR寄存器 
	return 0;
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_TX_DMAConfig(uint8_t* buf0,uint8_t *buf1,uint16_t num)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
	
	DMA_DeInit(DMA1_Stream4);
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}//等待DMA1_Stream4可配置 
		
	DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_FEIF4|DMA_IT_DMEIF4|DMA_IT_TEIF4|DMA_IT_HTIF4|DMA_IT_TCIF4);//清空DMA1_Stream4上所有中断标志

  /* 配置 DMA Stream */

  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  //通道0 SPI2_TX通道 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;//外设地址为:(u32)&SPI2->DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;//存储器到外设模式
  DMA_InitStructure.DMA_BufferSize = num;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度：16位 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;//高优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //不使用FIFO模式        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA1_Stream4, &DMA_InitStructure);//初始化DMA Stream
	
	DMA_DoubleBufferModeConfig(DMA1_Stream4,(u32)buf1,DMA_Memory_0);//双缓冲模式配置
 
  DMA_DoubleBufferModeCmd(DMA1_Stream4,ENABLE);//双缓冲模式开启
 
  DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);//开启传输完成中断
 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_RX_DMAConfig(uint8_t* buf0,uint8_t *buf1,uint16_t num)
{
		NVIC_InitTypeDef   NVIC_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1时钟使能 
	
	DMA_DeInit(DMA1_Stream3);
	while (DMA_GetCmdStatus(DMA1_Stream3) != DISABLE){}//等待DMA1_Stream3可配置 
		
	DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_FEIF3|DMA_IT_DMEIF3|DMA_IT_TEIF3|DMA_IT_HTIF3|DMA_IT_TCIF3);//清空DMA1_Stream3上所有中断标志

  /* 配置 DMA Stream */

  DMA_InitStructure.DMA_Channel = DMA_Channel_3;  //通道3 I2S2ext_RX通道 
  DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2S2ext->DR;//外设地址为:(u32)&I2S2ext->DR>DR
  DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;//DMA 存储器0地址
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;//外设到存储器模式
  DMA_InitStructure.DMA_BufferSize = num;//数据传输量 
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设非增量模式
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//存储器增量模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据长度:16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据长度：16位 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;// 使用循环模式 
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;//中等优先级
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; //不使用FIFO模式        
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;//外设突发单次传输
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;//存储器突发单次传输
  DMA_Init(DMA1_Stream3, &DMA_InitStructure);//初始化DMA Stream
	
	DMA_DoubleBufferModeConfig(DMA1_Stream3,(u32)buf1,DMA_Memory_0);//双缓冲模式配置
 
  DMA_DoubleBufferModeCmd(DMA1_Stream3,ENABLE);//双缓冲模式开启
 
  DMA_ITConfig(DMA1_Stream3,DMA_IT_TC,ENABLE);//开启传输完成中断
  
		
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0x00;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;//子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
}


//I2S DMA回调函数指针
void (*i2s_tx_callback)(void);	//TX回调函数
void (*i2s_rx_callback)(void);	//RX回调函数



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void DMA1_Stream4_IRQHandler(void)
{    
  OSIntEnter();    
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4)==SET)////DMA1_Stream4,传输完成标志
	{ 
		DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
      	i2s_tx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}   
  OSIntExit(); 	
} 


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void DMA1_Stream3_IRQHandler(void)
{ 
	OSIntEnter();        
	if(DMA_GetITStatus(DMA1_Stream3,DMA_IT_TCIF3)==SET)	//DMA1_Stream3,传输完成标志
	{ 
		DMA_ClearITPendingBit(DMA1_Stream3,DMA_IT_TCIF3);	//清除传输完成中断
      	i2s_rx_callback();	//执行回调函数,读取数据等操作在这里面处理  
	}  		
 OSIntExit();	
} 


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_PlayStart(void)
{
	DMA_Cmd(DMA1_Stream4,ENABLE);//开启DMA TX传输,开始播放 	
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_PlayStop(void)
{
	DMA_Cmd(DMA1_Stream4,DISABLE);//关闭DMA,结束播放
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_RecStart(void)
{
	DMA_Cmd(DMA1_Stream3,ENABLE);//开启DMA TX传输,开始录音	
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2s_RecStop(void)
{
	DMA_Cmd(DMA1_Stream3,DISABLE);//关闭DMA,结束录音
}
