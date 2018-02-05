# include "bsp_spi.h"
# include "bsp_malloc.h"

SPI_HandleTypeDef SPI_Handle;

# define SPI3_DR_ADDR	(uint32_t )0x40003c0c
extern uint8_t *FLASH_RX_BUFF;
extern uint8_t *FLASH_TX_BUFF;


void bsp_spi_GPIOConfig(void)
{
	
}

void bsp_spi_ParaConfig(void)
{
	
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
void bsp_spi_Config(SPI_TypeDef *SPIx)
{	
	SPI_Handle.Instance = SPI3;
	SPI_Handle.Init.Mode = SPI_MODE_MASTER;
	SPI_Handle.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
	SPI_Handle.Init.CLKPhase = SPI_PHASE_2EDGE;
	SPI_Handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
	SPI_Handle.Init.NSS = SPI_NSS_SOFT;
	SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_Handle.Init.CRCPolynomial = 7;
	HAL_SPI_Init(&SPI_Handle);
	
	
	# if USE_DMA > 0u
	SPI3->CR2 |= 1 << 1;
	SPI3->CR2 |= 1 << 0;
	bsp_spi_DMAConfig();
	# endif
	SPI3->CR1 |= SPI_CR1_SPE;   /* Enable SPI */
	bsp_spi_ReadWriteByte(SPIx, 0xff);//启动传输		 
}

# if USE_DMA > 0u
void bsp_spi_DMAConfig(void)
{
	RCC->AHB1ENR |= 1 << 21;  /*  开启DMA1的时钟  */
	
	while(DMA1_Stream2->CR & 0x01);  /*  等待DMA可配置  */
	
	/*  配置SPI3,DMA RX  */
	DMA1_Stream2->PAR = SPI3_DR_ADDR;
	DMA1_Stream2->M0AR = (uint32_t)FLASH_RX_BUFF;
	DMA1_Stream2->NDTR = 4096;
	
	DMA1_Stream2->CR = (uint32_t)0;
	DMA1_Stream2->CR |= ((uint8_t)0x00 << 25);  /*  数据流2,通道0  */
	DMA1_Stream2->CR |= ((uint8_t)0x00 << 21);  /*  单次传输  */
	DMA1_Stream2->CR |= 0 << 19;								/*  寻址指针为DMA_SxM0AR  */
	DMA1_Stream2->CR |= ((uint8_t )0x02 << 16);	/*  中等优先级  */
	DMA1_Stream2->CR |= (uint8_t )0x00 << 13;		/*  存储器数据长度8位  */
	DMA1_Stream2->CR |= (uint8_t )0x00 << 11;		/*  外设数据长度8位  */
	DMA1_Stream2->CR |= 1 << 10;	/*  存储器增量模式  */
	DMA1_Stream2->CR |= 0 << 9;		/*  外设非增量模式  */
	DMA1_Stream2->CR |= 0 << 8;		/*  非循环模式  */
	DMA1_Stream2->CR |= (uint8_t)0x00 << 6;		/*  外设到存储器  */
//	DMA1_Stream2->CR |= 1 << 4;  /*  开启传输完成中断  */

	
	/*  配置SPI3,DMA TX  */
	while(DMA1_Stream5->CR & 0x01);
	DMA1_Stream5->PAR = SPI3_DR_ADDR;
	DMA1_Stream5->M0AR = (uint32_t)FLASH_TX_BUFF;
	DMA1_Stream5->NDTR = 4096;
	
	DMA1_Stream5->CR = (uint32_t)0;
	DMA1_Stream5->CR |= ((uint8_t)0x00 << 25);  /*  数据流4,通道0  */
	DMA1_Stream5->CR |= ((uint8_t)0x00 << 21);  /*  单次传输  */
	DMA1_Stream5->CR |= 0 << 19;								/*  寻址指针为DMA_SxM0AR  */
	DMA1_Stream5->CR |= ((uint8_t )0x02 << 16);	/*  中等优先级  */
	DMA1_Stream5->CR |= (uint8_t )0x00 << 13;		/*  存储器数据长度8位  */
	DMA1_Stream5->CR |= (uint8_t )0x00 << 11;		/*  外设数据长度8位  */
	DMA1_Stream5->CR |= 1 << 10;	/*  存储器增量模式  */
	DMA1_Stream5->CR |= 0 << 9;		/*  外设非增量模式  */
	DMA1_Stream5->CR |= 0 << 8;		/*  非循环模式  */
	DMA1_Stream5->CR |= (uint8_t)0x01 << 6;		/*  存储器到外设  */
//	DMA1_Stream5->CR |= 1 << 4;  /*  开启传输完成中断  */
	
//	bsp_nvic_Config(1,0,DMA1_Stream2_IRQn, 4);
//	bsp_nvic_Config(1,3,DMA1_Stream5_IRQn, 4);
//	DMA1->HIFCR = 0;
//	DMA1->LIFCR = 0;
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
void bsp_spi_DMAStart(DMA_Stream_TypeDef *DMA_Streamx, uint16_t length)
{
	DMA_Streamx->CR &= ~(1 << 0);
	while(DMA_Streamx->CR & 0x01);
	DMA_Streamx->NDTR = length;
	DMA_Streamx->CR |= 1 << 0;		/*  开启DMA传输  */
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
void bsp_spi_DMAStop(DMA_Stream_TypeDef *DMA_Streamx)
{
	DMA_Streamx->CR &= ~(1 << 0);
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
void DMA1_Stream2_IRQHandler(void)
{
	OSIntEnter();
	
	if((DMA1->LISR >> 21) & 0x01)
	{
		os_printf("INT\r\n");
		DMA1->LIFCR |= 1 << 21;		/*  清除中断标志位  */
		DMA1_Stream2->CR &= ~(1 << 0);		/*  关闭数据流  */
	}
	OSIntExit();
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
void DMA1_Stream5_IRQHandler(void)
{
	OSIntEnter();
	
	if((DMA1->HISR >> 11) & 0x01)
	{
		os_printf("INT4\r\n");
		DMA1->HIFCR |= 1 << 11;		/*  清除中断标志位  */
		DMA1_Stream5->CR &= ~(1 << 0);		/*  关闭数据流  */
	}
	OSIntExit();
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
uint8_t bsp_spi_ReadByte(SPI_TypeDef *SPIx)
{
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
uint8_t bsp_spi_ReadBuff(SPI_TypeDef *SPIx, uint8_t *rxbuff, uint16_t length)
{
	uint32_t temp[1];
	bsp_spi_DMAStop(DMA1_Stream2);
	bsp_spi_DMAStop(DMA1_Stream5);
	
	DMA1_Stream2->M0AR = (uint32_t)rxbuff;
	DMA1_Stream2->NDTR = length;
	
	DMA1_Stream5->M0AR = (uint32_t)temp;
	SPIx->DR;
	
	while((SPIx->SR & SPI_FLAG_TXE) == RESET);
	
	DMA1_Stream2->CR |= 1 << 0;
	DMA1_Stream5->CR |= 1 << 0;
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
uint8_t bsp_spi_WriteByte(SPI_TypeDef *SPIx, uint8_t byte)
{
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
uint8_t bsp_spi_WriteBuff(SPI_TypeDef *SPIx, uint8_t *txbuff, uint16_t length)
{
	uint32_t temp[1];
	bsp_spi_DMAStop(DMA1_Stream2);
	bsp_spi_DMAStop(DMA1_Stream5);
	
	DMA1_Stream5->M0AR = (uint32_t)txbuff;
	DMA1_Stream5->NDTR = length;
	
	DMA1_Stream2->M0AR = (uint32_t)temp;
	DMA1_Stream2->NDTR = length;
	SPIx->DR;
	
	while((SPIx->SR & SPI_FLAG_TXE) == RESET);
	
	DMA1_Stream2->CR |= 1 << 0;
	DMA1_Stream5->CR |= 1 << 0;
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
uint8_t bsp_spi_Transmit(SPI_TypeDef *SPIx, uint8_t *rxbuff, uint8_t *txbuff, uint16_t length)
{
	bsp_spi_DMAStop(DMA1_Stream5);   /*  关闭发送DMA数据流  */
	bsp_spi_DMAStop(DMA1_Stream2);   /*  关闭接收DMA数据流  */
	
	DMA1_Stream5->NDTR = length;		/*  设置发送数据长度  */
	DMA1_Stream2->NDTR = length;    /*  设置接收数据长度  */
	
	DMA1_Stream5->CR |= 1 << 10;		/*  发送地址自增  */
	DMA1_Stream2->CR |= 1 << 10;		/*  接收地址自增  */
	
	DMA1_Stream5->M0AR = (uint32_t)txbuff;
	DMA1_Stream2->M0AR = (uint32_t)rxbuff;
	
	SPIx->DR;
	
	while((SPIx->SR & SPI_FLAG_TXE) == RESET);
//	DMA1_Stream2->CR |= 1 << 4;
//	DMA1_Stream5->CR |= 1 << 4;
	
	DMA1_Stream2->CR |= 1 << 0;
	DMA1_Stream5->CR |= 1 << 0;
	
	while((uint8_t)((DMA1->HISR >> 11) & 0x01) != 0x01);
	while((uint8_t)((DMA1->LISR >> 21) & 0x01) != 0x01);
	
	DMA1->HIFCR |= 1 << 11;		/*  清除中断标志位  */
	DMA1_Stream5->CR &= ~(1 << 0);		/*  关闭数据流  */
	
	DMA1->LIFCR |= 1 << 21;		/*  清除中断标志位  */
	DMA1_Stream2->CR &= ~(1 << 0);		/*  关闭数据流  */
	
	return 0;
}
# else
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
uint8_t bsp_spi_ReadByte(SPI_TypeDef *SPIx)
{
	uint16_t timeout = 0;
	
	while(((SPIx->SR & SPI_FLAG_TXE) == RESET) && (timeout++ < 50000));
	if(timeout >= 50000) return 0xff;
	
	SPIx->DR = 0XFF;
	
	timeout = 0;
	while(((SPIx->SR & SPI_FLAG_RXNE) == RESET) && (timeout++ < 50000));
	if(timeout >= 50000) return 0xff;
	
	return SPIx->DR;
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
uint8_t bsp_spi_ReadBuff(SPI_TypeDef *SPIx, uint8_t *rxbuff, uint16_t length)
{
	uint16_t i = 0;
	
	for(i = 0; i < length; i++)
	{
		rxbuff[i] = bsp_spi_ReadByte(SPI3);
	}
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
uint8_t bsp_spi_WriteByte(SPI_TypeDef *SPIx, uint8_t byte)
{
	uint16_t timeout = 0;

	while(((SPIx->SR & SPI_FLAG_TXE) == RESET) && (timeout++ < 50000));
	if(timeout >= 50000) return 0xff;

	SPIx->DR = byte;

	timeout = 0;
	while(((SPIx->SR & SPI_FLAG_RXNE) == RESET) && (timeout++ < 50000));
	if(timeout >= 50000) return 0xff;
	
	return SPIx->DR;
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
uint8_t bsp_spi_WriteBuff(SPI_TypeDef *SPIx, uint8_t *txbuff, uint16_t length)
{
	uint16_t i = 0;
	
	for(i = 0; i < length; i++)
	{
		bsp_spi_WriteByte(SPI3, txbuff[i]);
	}
	return 0;
}
# endif


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
uint8_t bsp_spi_ReadWriteByte(SPI_TypeDef *SPIx, uint8_t byte)
{
	while((SPIx->SR & SPI_FLAG_TXE) == RESET);
	
	SPIx->DR = byte;
	
	while((SPIx->SR & SPI_FLAG_RXNE) == RESET);
	
	return SPIx->DR;
	   
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
uint8_t bsp_spi_SetSpeed(SPI_TypeDef *SPIx, uint8_t SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPIx->CR1 |= SPI_CR1_SPE;   /* Enable SPI */
	
	return 0;
}



