# ifndef __BSP_WLAN_H
# define __BSP_WLAN_H

# include "bsp.h"
# include "bsp_sdcard.h"


void bsp_wlan_GPIOConfig(void);
void bsp_wlan_NVICConfig(void);

SD_Error bsp_wlan_Config(void);
SD_Error bsp_wlan_PowerCmd(FunctionalState cmd);
SD_Error bsp_wlan_CardInit(void);
SD_Error bsp_wlan_GetCardInfo(SD_CardInfo *CardInfo);
SD_Error bsp_wlan_SetWideBus(uint32_t mode);
SD_Error bsp_wlan_SetDeviceMode(uint32_t mode);
SD_Error bsp_wlan_Select(uint32_t addr);
SD_Error bsp_wlan_SetStatus(uint32_t *CardStatus);
SD_CardState bsp_wlan_GetStatus(void);
SD_Error bsp_wlan_ReadBlock(uint8_t *buff, long long addr, uint16_t blksize);
SD_Error bsp_wlan_ReadBlocks(uint8_t *buff, long long addr, uint16_t blksize, uint32_t nblks);
SD_Error bsp_wlan_WriteBlock(uint8_t *buff, long long addr, uint16_t blksize);
SD_Error bsp_wlan_WriteBlocks(uint8_t *buff, long long addr, uint16_t blksize, uint32_t nblks);
SD_Error bsp_wlan_ProcessIRQSrc(void);
SD_Error bsp_wlan_CmdR0Error(void);
SD_Error bsp_wlan_CmdR1Error(uint8_t cmd);
SD_Error bsp_wlan_CmdR2Error(void);
SD_Error bsp_wlan_CmdR3Error(void);
SD_Error bsp_wlan_CmdR6Error(uint8_t cmd, uint16_t *prca);
SD_Error bsp_wlan_CmdR7Error(void);
SD_Error bsp_wlan_EnWideBus(uint8_t en);
SD_Error bsp_wlan_IsBusy(uint8_t *status);
SD_Error bsp_wlan_FindSCR(uint16_t rca, uint32_t *pscr);


# endif

