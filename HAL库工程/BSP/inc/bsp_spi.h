# ifndef __BSP_SPI_H
# define __BSP_SPI_H

# include "bsp.h"


void bsp_spiConfig(SPI_TypeDef *SPIx);
uint8_t bsp_spiReadWriteByte(SPI_TypeDef *SPIx, uint8_t byte);
void bsp_spiSetSpeed(SPI_TypeDef *SPIx, uint8_t SPI_BaudRatePrescaler);
# endif

