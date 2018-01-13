# ifndef __BSP_SDCARD_H
# define __BSP_SDCARD_H

# include "bsp.h"


#define SD_TIMEOUT 			((uint32_t)100000000)  	//超时时间
#define SD_TRANSFER_OK     	((uint8_t)0x00)
#define SD_TRANSFER_BUSY   	((uint8_t)0x01)

#define SD_DMA_MODE    		1						//1：DMA模式，0：查询模式   

extern SD_HandleTypeDef        SDCARD_Handler;     	//SD卡句柄
extern HAL_SD_CardInfoTypeDef  SDCARD_Info;         	//SD卡信息结构体



uint8_t bsp_sd_Config(void);
uint8_t bsp_sd_GetInfo(HAL_SD_CardInfoTypeDef *cardinfo);
uint8_t bsp_sd_GetState(void);
uint8_t bsp_sd_ReadDisk(uint8_t *buff, uint32_t sector, uint32_t cnt);
uint8_t bsp_sd_WriteDisk(uint8_t *buff, uint32_t sector, uint32_t cnt);
//uint8_t bsp_sd_ReadBlocks_DMA(uint32_t *buff, uint16_t sector, uint32_t blocksize, uint32_t cnt);
//uint8_t bsp_sd_WriteBlocks_DMA(uint32_t *buff, uint16_t sector, uint32_t blocksize, uint32_t cnt);
# endif


