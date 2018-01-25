# include "bsp_lan.h"
# include "lwip_comm.h"
# include "app_sys.h"

ETH_HandleTypeDef ETH_Handler; 

ETH_DMADescTypeDef *DMARxDescTab;	//以太网DMA接收描述符数据结构体指针
ETH_DMADescTypeDef *DMATxDescTab;	//以太网DMA发送描述符数据结构体指针 
uint8_t *Rx_Buff; 					//以太网底层驱动接收buffers指针 
uint8_t *Tx_Buff; 					//以太网底层驱动发送buffers指针
  
extern lwip_dev lwip_info;

	/*网络引脚设置 RMII接口
	  ETH_MDIO -------------------------> PA2
	  ETH_MDC --------------------------> PC1
	  ETH_RMII_REF_CLK------------------> PA1
	  ETH_RMII_CRS_DV ------------------> PA7
	  ETH_RMII_RXD0 --------------------> PC4
	  ETH_RMII_RXD1 --------------------> PC5
	  ETH_RMII_TX_EN -------------------> PB11
	  ETH_RMII_TXD0 --------------------> PB12
	  ETH_RMII_TXD1 --------------------> PB13
	  ETH_RESET-------------------------> PC7*/
		
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
void ETH_NVICConfig(void)
{
	HAL_NVIC_SetPriority(ETH_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(ETH_IRQn);
}



void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable Peripheral clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_ETH_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	__ETH_CLK_ENABLE() ;

	/* Peripheral interrupt init */
	ETH_NVICConfig();
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
    /* Peripheral clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();
  
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(ETH_IRQn);

  }
}

void bsp_lan_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
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
uint8_t bsp_lan_Config(void)
{
	uint8_t MAC[6];
	uint32_t phyreg = 0x00;
	HAL_StatusTypeDef eth_err;
	
	bsp_lan_GPIOConfig();
	
	DISABLE_INT();
	LAN_RST = 0;
	bsp_tim_DelayMs(100);
	LAN_RST = 1;
	ENABLE_INT();
	
	MAC[0] = lwip_info.mac[0];
	MAC[1] = lwip_info.mac[1];
	MAC[2] = lwip_info.mac[2];
	MAC[3] = lwip_info.mac[3];
	MAC[4] = lwip_info.mac[4];
	MAC[5] = lwip_info.mac[5];
	
	ETH_Handler.Instance = ETH;
	ETH_Handler.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
//	ETH_Handler.Init.Speed = ETH_SPEED_100M;
//	ETH_Handler.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
	ETH_Handler.Init.PhyAddress = LAN_PHY_ADDR;
	ETH_Handler.Init.MACAddr = &MAC[0];
	ETH_Handler.Init.RxMode = ETH_RXINTERRUPT_MODE;
	ETH_Handler.Init.ChecksumMode = ETH_CHECKSUM_BY_SOFTWARE;
	ETH_Handler.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
	
	HAL_ETH_DeInit(&ETH_Handler);	 /* 初始化之前先复位 */
	
	/* 开启发送完成中断和异常错误总中断 */
	__HAL_ETH_DMA_ENABLE_IT(&ETH_Handler, ETH_DMA_IT_NIS | ETH_DMA_IT_T | ETH_DMA_IT_AIS | ETH_DMA_IT_FBE);	

	/* 正常初始化返回HAL_OK，如果没插网线则返回超时 */
	eth_err = HAL_ETH_Init(&ETH_Handler);

	/* 构造DMA发送和接收描述符成为链表方式 */
	HAL_ETH_DMATxDescListInit(&ETH_Handler, DMATxDescTab, (uint8_t *)Tx_Buff, ETH_TXBUFNB);
	HAL_ETH_DMARxDescListInit(&ETH_Handler,  DMARxDescTab, (uint8_t *)Rx_Buff, ETH_RXBUFNB);	
	
	/* 启动ETH */
	HAL_ETH_Start(&ETH_Handler);
	
	/* 检查是否连接了网线*/
	HAL_ETH_ReadPHYRegister(&ETH_Handler, PHY_BSR, &phyreg);
	Sys_Info.LinkStatus = (phyreg & PHY_LINKED_STATUS);

	return eth_err;	
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
void bsp_lan_WritePHY(uint16_t reg, uint16_t value)
{
	uint32_t regval = value;
	HAL_ETH_ReadPHYRegister(&ETH_Handler, reg, &regval);
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
uint8_t bsp_lan_ReadPHY(uint16_t reg)
{
	uint32_t regval;
	HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&regval);
	return regval;
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
uint8_t bsp_lan_GetSpeed(void)
{
	return ((bsp_lan_ReadPHY(31) & 0x1c) >> 2);
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
uint8_t ETH_Malloc(void)
{
	DMARxDescTab = bsp_mem_Malloc(SRAMIN, ETH_RXBUFNB * sizeof(ETH_DMADescTypeDef));
	DMATxDescTab = bsp_mem_Malloc(SRAMIN, ETH_TXBUFNB * sizeof(ETH_DMADescTypeDef));
	
	Rx_Buff = bsp_mem_Malloc(SRAMIN, ETH_RX_BUF_SIZE * ETH_RXBUFNB);
	Tx_Buff = bsp_mem_Malloc(SRAMIN, ETH_TX_BUF_SIZE * ETH_TXBUFNB);
	
	if(!DMARxDescTab || !DMATxDescTab || !Rx_Buff || !Tx_Buff)
	{
		ETH_Free();
		return 1;
	}
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
void ETH_Free(void)
{
	bsp_mem_Free(SRAMIN, DMARxDescTab);
	bsp_mem_Free(SRAMIN, DMATxDescTab);
	bsp_mem_Free(SRAMIN, Rx_Buff);
	bsp_mem_Free(SRAMIN, Tx_Buff);
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
void ETH_IRQHandler(void)
{
    OSIntEnter(); 
    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))   
    {
			lwip_pkt_handle();//处理以太网数据，即将数据提交给LWIP
    }
    //清除中断标志位
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R); 
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS); 
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
uint32_t  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    uint32_t frameLength = 0;
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET)) 
    {
        frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return frameLength;
}

