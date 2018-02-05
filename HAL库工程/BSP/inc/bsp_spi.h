# ifndef __BSP_SPI_H
# define __BSP_SPI_H

# include "bsp.h"
# define USE_DMA	1u

void bsp_spi_Config(SPI_TypeDef *SPIx);

uint8_t bsp_spi_Transmit(SPI_TypeDef *SPIx, uint8_t *rxbuff, uint8_t *txbuff, uint16_t length);
uint8_t bsp_spi_ReadByte(SPI_TypeDef *SPIx);
uint8_t bsp_spi_ReadBuff(SPI_TypeDef *SPIx, uint8_t *rxbuff, uint16_t length);
uint8_t bsp_spi_WriteByte(SPI_TypeDef *SPIx, uint8_t byte);
uint8_t bsp_spi_WriteBuff(SPI_TypeDef *SPIx, uint8_t *txbuff, uint16_t length);
uint8_t bsp_spi_ReadWriteByte(SPI_TypeDef *SPIx, uint8_t byte);
uint8_t bsp_spi_SetSpeed(SPI_TypeDef *SPIx, uint8_t SPI_BaudRatePrescaler);

# if USE_DMA > 0u
void bsp_spi_DMAConfig(void);
void bsp_spi_DMAStart(DMA_Stream_TypeDef *DMA_Streamx, uint16_t length);
void bsp_spi_DMAStop(DMA_Stream_TypeDef *DMA_Streamx);
void DMA1_Stream0_IRQHandler(void);
# endif
# endif

