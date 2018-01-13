# include "bsp_sdcard.h"


SD_HandleTypeDef        SDCARD_Handler;     //SD�����
HAL_SD_CardInfoTypeDef  SDCARD_Info;         //SD����Ϣ�ṹ��
DMA_HandleTypeDef SDTxDMAHandler,SDRxDMAHandler;    //SD��DMA���ͺͽ��վ��

__align(4) uint8_t SDIO_DATA_BUFFER[512];


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
uint8_t bsp_sd_Config(void)
{
	uint8_t SD_Err;
	
	SDCARD_Handler.Instance = SDIO;
	SDCARD_Handler.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	SDCARD_Handler.Init.BusWide = SDIO_BUS_WIDE_1B;
	SDCARD_Handler.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	SDCARD_Handler.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV;
	SDCARD_Handler.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	SDCARD_Handler.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	SD_Err = HAL_SD_Init(&SDCARD_Handler);
	
	if(SD_Err != HAL_OK) return SD_Err;
	
	HAL_SD_GetCardInfo(&SDCARD_Handler, &SDCARD_Info);
	
	SD_Err = HAL_SD_ConfigWideBusOperation(&SDCARD_Handler, SDIO_BUS_WIDE_4B);
	return SD_Err;
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
void HAL_SD_MspInit(SD_HandleTypeDef* hsd)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	/* Peripheral clock enable */
	__HAL_RCC_SDIO_CLK_ENABLE();

	/**SDIO GPIO Configuration    
	PC8     ------> SDIO_D0
	PC9     ------> SDIO_D1
	PC10     ------> SDIO_D2
	PC11     ------> SDIO_D3
	PC12     ------> SDIO_CK
	PD2     ------> SDIO_CMD 
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
												|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	
#if (SD_DMA_MODE==1)                        //ʹ��DMAģʽ
    HAL_NVIC_SetPriority(SDMMC1_IRQn,2,0);  //����SDMMC1�жϣ���ռ���ȼ�2�������ȼ�0
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);        //ʹ��SDMMC1�ж�
    
    //���÷���DMA
    SDRxDMAHandler.Instance=DMA2_Stream3;
    SDRxDMAHandler.Init.Channel=DMA_CHANNEL_4;
    SDRxDMAHandler.Init.Direction=DMA_PERIPH_TO_MEMORY;
    SDRxDMAHandler.Init.PeriphInc=DMA_PINC_DISABLE;
    SDRxDMAHandler.Init.MemInc=DMA_MINC_ENABLE;
    SDRxDMAHandler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
    SDRxDMAHandler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
    SDRxDMAHandler.Init.Mode=DMA_PFCTRL;
    SDRxDMAHandler.Init.Priority=DMA_PRIORITY_VERY_HIGH;
    SDRxDMAHandler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
    SDRxDMAHandler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    SDRxDMAHandler.Init.MemBurst=DMA_MBURST_INC4;
    SDRxDMAHandler.Init.PeriphBurst=DMA_PBURST_INC4;

    __HAL_LINKDMA(hsd, hdmarx, SDRxDMAHandler); //������DMA��SD���ķ���DMA��������
    HAL_DMA_DeInit(&SDRxDMAHandler);
    HAL_DMA_Init(&SDRxDMAHandler);              //��ʼ������DMA
    
    //���ý���DMA 
    SDTxDMAHandler.Instance=DMA2_Stream6;
    SDTxDMAHandler.Init.Channel=DMA_CHANNEL_4;
    SDTxDMAHandler.Init.Direction=DMA_MEMORY_TO_PERIPH;
    SDTxDMAHandler.Init.PeriphInc=DMA_PINC_DISABLE;
    SDTxDMAHandler.Init.MemInc=DMA_MINC_ENABLE;
    SDTxDMAHandler.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;
    SDTxDMAHandler.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;
    SDTxDMAHandler.Init.Mode=DMA_PFCTRL;
    SDTxDMAHandler.Init.Priority=DMA_PRIORITY_VERY_HIGH;
    SDTxDMAHandler.Init.FIFOMode=DMA_FIFOMODE_ENABLE;
    SDTxDMAHandler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;
    SDTxDMAHandler.Init.MemBurst=DMA_MBURST_INC4;
    SDTxDMAHandler.Init.PeriphBurst=DMA_PBURST_INC4;
    
    __HAL_LINKDMA(hsd, hdmatx, SDTxDMAHandler);//������DMA��SD���ķ���DMA��������
    HAL_DMA_DeInit(&SDTxDMAHandler);
    HAL_DMA_Init(&SDTxDMAHandler);              //��ʼ������DMA 
  

    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 3, 0);  //����DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 3, 0);  //����DMA�ж����ȼ�
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);
#endif
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
void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd)
{

  if(hsd->Instance==SDIO)
  {

    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();
  
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  }

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
uint8_t bsp_sd_GetInfo(HAL_SD_CardInfoTypeDef *cardinfo)
{
	return HAL_SD_GetCardInfo(&SDCARD_Handler, cardinfo);
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
uint8_t bsp_sd_GetState(void)
{
	return ((HAL_SD_GetCardState(&SDCARD_Handler)==HAL_SD_CARD_TRANSFER )?SD_TRANSFER_OK:SD_TRANSFER_BUSY);
}

# if SD_DMA_MODE == 1
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
uint8_t bsp_sd_ReadBlocks_DMA(uint32_t *buff, uint16_t sector, uint32_t blocksize, uint32_t cnt)
{
	uint8_t err=0;
	uint32_t timeout=SD_TIMEOUT;
	err=HAL_SD_ReadBlocks_DMA(&SDCARD_Handler,(uint8_t*)buff,sector,cnt);//ͨ��DMA��ȡSD��n������

	if(err==0)
	{
		//�ȴ�SD������
		while(bsp_sd_GetState()!=SD_TRANSFER_OK)
		{
			if(timeout-- == 0)
			{	
				err=SD_TRANSFER_BUSY;
			}
		} 
	}
	return err;
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
uint8_t bsp_sd_WriteBlocks_DMA(uint32_t *buff, uint16_t sector, uint32_t blocksize, uint32_t cnt)
{
	uint8_t err=0; 
	uint32_t timeout=SD_TIMEOUT;
	err=HAL_SD_WriteBlocks_DMA(&SDCARD_Handler,(uint8_t*)buff,sector,cnt);//ͨ��DMAдSD��n������

	if(err==0)
	{
		//�ȴ�SD��д��
		while(bsp_sd_GetState()!=SD_TRANSFER_OK)
		{
			if(timeout-- == 0)
			{	
				err=SD_TRANSFER_BUSY;
			}
		} 
	}
	return err;
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
uint8_t bsp_sd_ReadDisk(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	uint8_t sta = HAL_OK;
	long long lsector = sector;
	
	sta = bsp_sd_ReadBlocks_DMA((uint32_t*)buff, lsector, 512, cnt);
	return sta;
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
uint8_t bsp_sd_WriteDisk(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	uint8_t sta=HAL_OK;
	long long lsector=sector;
	
	sta=bsp_sd_WriteBlocks_DMA((uint32_t*)buff,lsector,512,cnt);//���sector��д����

	return sta;
}

//SDMMC1�жϷ�����
void SDMMC1_IRQHandler(void)
{
    HAL_SD_IRQHandler(&SDCARD_Handler);
}

//DMA2������6�жϷ�����
void DMA2_Stream6_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmatx);
}

//DMA2������3�жϷ�����
void DMA2_Stream3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(SDCARD_Handler.hdmarx);
}
# else

uint8_t bsp_sd_ReadDisk(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	uint8_t sta=HAL_OK;
	uint32_t timeout=SD_TIMEOUT;
	long long lsector=sector;
	DISABLE_INT();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
	sta=HAL_SD_ReadBlocks(&SDCARD_Handler, (uint8_t*)buff,lsector,cnt,SD_TIMEOUT);//���sector�Ķ�����

	//�ȴ�SD������
	while(bsp_sd_GetState()!=SD_TRANSFER_OK)
	{
		if(timeout-- == 0)
		{	
			sta=SD_TRANSFER_BUSY;
		}
	}
	ENABLE_INT();//�������ж�
	return sta;
}

uint8_t bsp_sd_WriteDisk(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	uint8_t sta=HAL_OK;
	uint32_t timeout=SD_TIMEOUT;
	long long lsector=sector;
	DISABLE_INT();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
	sta=HAL_SD_WriteBlocks(&SDCARD_Handler,(uint8_t*)buff,lsector,cnt,SD_TIMEOUT);//���sector��д����

	//�ȴ�SD��д��
	while(bsp_sd_GetState()!=SD_TRANSFER_OK)
	{
		if(timeout-- == 0)
		{	
			sta=SD_TRANSFER_BUSY;
		}
	}    
	ENABLE_INT();//�������ж�
	return sta;
}

# endif





