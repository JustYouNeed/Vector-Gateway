# ifndef __BSP_LAN_H
# define __BSP_LAN_H

# include "bsp.h"
# include "bsp_malloc.h"
# include "stm32f4xx_hal_eth.h"

extern ETH_HandleTypeDef ETH_Handler;               //以太网句柄
extern ETH_DMADescTypeDef *DMARxDescTab;			//以太网DMA接收描述符数据结构体指针
extern ETH_DMADescTypeDef *DMATxDescTab;			//以太网DMA发送描述符数据结构体指针 
extern uint8_t *Rx_Buff; 							//以太网底层驱动接收buffers指针 
extern uint8_t *Tx_Buff; 							//以太网底层驱动发送buffers指针
extern ETH_DMADescTypeDef  *DMATxDescToSet;			//DMA发送描述符追踪指针
extern ETH_DMADescTypeDef  *DMARxDescToGet; 		//DMA接收描述符追踪指针 
 
# define LAN_RST		PCout(7)
# define LAN_PHY_ADDR		0x00


uint8_t bsp_lan_Config(void);
uint8_t bsp_lan_GetSpeed(void);
void bsp_lan_WritePHY(uint16_t reg, uint16_t value);
uint8_t bsp_lan_ReadPHY(uint16_t reg);
void ETH_NVICConfig(void);
uint32_t  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc);

//uint8_t ETH_MACDMA_Config(void);
uint8_t ETH_Malloc(void);
void ETH_Free(void);

# endif
