# include "bsp_spi.h"

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
void bsp_spiConfig(SPI_TypeDef *SPIx)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
	bsp_GPIOClcokCmd(GPIOB);
	bsp_SPIClockCmd(SPI3);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_SPI3); //PB3复用为 SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_SPI3); //PB4复用为 SPI1
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_SPI3); //PB5复用为 SPI1

 	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,ENABLE);//复位SPI1
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3,DISABLE);//停止复位SPI1

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPIx, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPIx, ENABLE); //使能SPI外设

//SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF); 

	bsp_spiReadWriteByte(SPIx, 0xff);//启动传输		 
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
uint8_t bsp_spiReadWriteByte(SPI_TypeDef *SPIx, uint8_t byte)
{
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPIx, byte); //通过外设SPIx发送一个byte  数据
		
  while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPIx); //返回通过SPIx最近接收的数据	
 		   
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
void bsp_spiSetSpeed(SPI_TypeDef *SPIx, uint8_t SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPIx,ENABLE);  
}



