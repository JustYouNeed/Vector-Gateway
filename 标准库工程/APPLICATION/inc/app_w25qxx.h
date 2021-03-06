# ifndef __APP_W25QXX_H
# define __APP_W25QXX_H

# include "bsp.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
# define W25Q80 	0XEF13 	
# define W25Q16 	0XEF14
# define W25Q32 	0XEF15
# define W25Q64 	0XEF16
# define W25Q128	0XEF17

extern u16 W25QXX_TYPE;					//定义W25QXX芯片型号

# define W25X_WriteEnable		0x06 
# define W25X_WriteDisable		0x04 
# define W25X_ReadStatusReg		0x05 
# define W25X_WriteStatusReg		0x01 
# define W25X_ReadData			0x03 
# define W25X_FastReadData		0x0B 
# define W25X_FastReadDual		0x3B 
# define W25X_PageProgram		0x02 
# define W25X_BlockErase			0xD8 
# define W25X_SectorErase		0x20 
# define W25X_ChipErase			0xC7 
# define W25X_PowerDown			0xB9 
# define W25X_ReleasePowerDown	0xAB 
# define W25X_DeviceID			0xAB 
# define W25X_ManufactDeviceID	0x90 
# define W25X_JedecDeviceID		0x9F 

# define FLASH_CS				PAout(15)

void 			app_flash_Config(void);
uint16_t 	app_flash_ReadID(void);
uint8_t 	app_flash_ReadSR(void);
void 			app_flash_WriteSR(uint8_t sr);
void 			app_flash_WriteCmd(FunctionalState state);
void 			app_flash_WritePage(uint8_t *buff, uint32_t addr, uint16_t len);
void 			app_flash_WriteNoCheck(uint8_t *buff, uint32_t addr, uint16_t len);
void 			app_flash_Read(uint8_t *buff, uint32_t addr, uint16_t len);
void 			app_flash_Write(uint8_t *buff, uint32_t addr, uint16_t len);
void 			app_flash_EraseChip(void);
void 			app_flash_EraseSector(uint32_t dstAddr);
void 			app_flash_WaitBusy(void);
void 			app_flash_PowerCmd(FunctionalState state);
# endif

