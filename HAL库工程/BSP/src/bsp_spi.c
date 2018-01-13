# include "bsp_spi.h"

SPI_HandleTypeDef SPI_Handle;

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
	SPIx->CR1 |= SPI_CR1_SPE;   /* Enable SPI */
	
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
void bsp_spiSetSpeed(SPI_TypeDef *SPIx, uint8_t SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPIx->CR1&=0XFFC7;
	SPIx->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPIx->CR1 |= SPI_CR1_SPE;   /* Enable SPI */
}



