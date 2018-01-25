# include "bsp_wlan.h"

void bsp_showShortResponse()
{
	printf("SDIO->STA:%08x\r\n",SDIO->STA);
	printf("Command response recevied:CMD %d, RESP_%08x\r\n", SDIO->RESPCMD, SDIO->RESP1);
}

void bsp_wlan_GPIOConfig(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	SD_Error errorstatus=SD_OK;	 
  u8 clkdiv=0;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_DMA2, ENABLE);//ʹ��GPIOC,GPIOD DMA2ʱ��
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIOʱ��ʹ��
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIO��λ
	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12; 	//PC8,9,10,11,12���ù������	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100M
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);// PC8,9,10,11,12���ù������

	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
  GPIO_Init(GPIOD, &GPIO_InitStructure);//PD2���ù������
	
	 //���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO); //PC8,AF12
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);	
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);//SDIO������λ
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//	RCC->AHB1ENR |= 1<<2;  /* ʹ��PORTCʱ�� */
//	RCC->AHB1ENR |= 1<<3;  /* ʹ��PORTDʱ�� */
//	RCC->AHB1ENR |= 1<<22;  /* ʹ��DMA2ʱ�� */
//	RCC->APB2ENR |= 1<<11;  /* ʹ��SDIOʱ�� */
//	RCC->APB2RSTR |= 1<<11; /* ��λSDIO */
//	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	PDout(3) = 0;
	PBout(8) = 0;
//	PBout(6) = 1;
//	
//	/* ����GPIOC.8Ϊ�������������������ΪSDIO���� */
//	GPIOC->MODER &= ~(3 << (8*2));  /* ��λGPIOC.8,SDIO_D0 */
//	GPIOC->MODER |= 2 << (8*2);
//	GPIOC->OTYPER &= ~(1 << 8);
//	GPIOC->OTYPER |= 0 << 8;
//	GPIOC->PUPDR &= ~(3 << (8*2));
//	GPIOC->PUPDR |= 1 << (8*2);
//	GPIOC->OSPEEDR &= ~(3 << (8*2));
//	GPIOC->OSPEEDR |= 2 << (8*2);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	
//	GPIOC->AFR[1] &= ~(0x0f << 0);
//	GPIOC->AFR[1] |= (uint32_t)12 << 0;
//	
//	/* ����GPIOC.9Ϊ�������������������ΪSDIO���� */
//	GPIOC->MODER &= ~(3 << (9*2));  /* ��λGPIOC.9,SDIO_D1 */
//	GPIOC->MODER |= 2 << (9*2);
//	GPIOC->OTYPER &= ~(1 << 9);
//	GPIOC->OTYPER |= 0 << 9;
//	GPIOC->PUPDR &= ~(3 << (9*2));
//	GPIOC->PUPDR |= 1 << (9*2);
//	GPIOC->OSPEEDR &= ~(3 << (9*2));
//	GPIOC->OSPEEDR |= 2 << (9*2);
//	GPIOC->AFR[1] &= ~(0x0f << 4);
//	GPIOC->AFR[1] |= (uint32_t)12 << 4;
//	
//	/* ����GPIOC.10Ϊ�������������������ΪSDIO���� */
//	GPIOC->MODER &= ~(3 << (10*2));  /* ��λGPIOC.10,SDIO_D2 */
//	GPIOC->MODER |= 2 << (10*2);
//	GPIOC->OTYPER &= ~(1 << 10);
//	GPIOC->OTYPER |= 0 << 10;
//	GPIOC->PUPDR &= ~(3 << (10*2));
//	GPIOC->PUPDR |= 1 << (10*2);
//	GPIOC->OSPEEDR &= ~(3 << (10*2));
//	GPIOC->OSPEEDR |= 2 << (10*2);
//	GPIOC->AFR[1] &= ~(0x0f << 8);
//	GPIOC->AFR[1] |= (uint32_t)12 << 8;
//	
//	/* ����GPIOC.11Ϊ�������������������ΪSDIO���� */
//	GPIOC->MODER &= ~(3 << (11*2));  /* ��λGPIOC.11,SDIO_D3 */
//	GPIOC->MODER |= 2 << (11*2);
//	GPIOC->OTYPER &= ~(1 << 11);
//	GPIOC->OTYPER |= 0 << 11;
//	GPIOC->PUPDR &= ~(3 << (11*2));
//	GPIOC->PUPDR |= 1 << (11*2);
//	GPIOC->OSPEEDR &= ~(3 << (11*2));
//	GPIOC->OSPEEDR |= 2 << (11*2);
//	GPIOC->AFR[1] &= ~(0x0f << 12);
//	GPIOC->AFR[1] |= (uint32_t)12 << 12;
//	
//	/* ����GPIOC.12Ϊ�������������������ΪSDIO���� */
//	GPIOC->MODER &= ~(3 << (12*2));  /* ��λGPIOC.12,SDIO_CLK */
//	GPIOC->MODER |= 2 << (12*2);
//	GPIOC->OTYPER &= ~(1 << 12);
//	GPIOC->OTYPER |= 0 << 12;
//	GPIOC->PUPDR &= ~(3 << (12*2));
//	GPIOC->PUPDR |= 1 << (12*2);
//	GPIOC->OSPEEDR &= ~(3 << (12*2));
//	GPIOC->OSPEEDR |= 2 << (12*2);
//	GPIOC->AFR[1] &= ~(0x0f << 16);
//	GPIOC->AFR[1] |= (uint32_t)12 << 16;
//	
//	/* ����GPIOD.2Ϊ�������������������ΪSDIO���� */
//	GPIOD->MODER &= ~(3 << (2*2));  /* ��λGPIOD.2,SDIO_CMD */
//	GPIOD->MODER |= 2 << (2*2);
//	GPIOD->OTYPER &= ~(1 << 2);
//	GPIOD->OTYPER |= 0 << 2;
//	GPIOD->PUPDR &= ~(3 << (2*2));
//	GPIOD->PUPDR |= 1 << (2*2);
//	GPIOD->OSPEEDR &= ~(3 << (2*2));
//	GPIOD->OSPEEDR |= 2 << (2*2);
//	
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	
//	GPIOD->AFR[0] &= ~(0x0f << 8);
//	GPIOD->AFR[0] |= (uint32_t)12 << 8;
//	
//	RCC->APB2RSTR &= ~(1 << 11);  /* ������λSDIO */
	
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;	
}


void bsp_wlan_NVICConfig(void)
{
	MY_NVIC_Init(0,0,SDIO_IRQn,2);
}


SD_Error bsp_wlan_Config(void)
{
	SD_Error err = SD_OK;
//	uint8_t clkdiv = 0;
	
	bsp_wlan_GPIOConfig(); /*  SDIO���ų�ʼ��  */
	bsp_wlan_NVICConfig(); /*  NVIC��ʼ��  */
	
	err = bsp_wlan_PowerCmd(ENABLE);	/*  ���ϵ�  */
	
//	if(err == SD_OK)  /*  �ϵ�ɹ����ʼ��SD��  */
//		err = bsp_wlan_CardInit();
//	
//	if(err == SD_OK)	/*  ��ʼ�����ȡ������Ϣ  */
//		err = bsp_wlan_GetCardInfo(&SDCardInfo);
//	
//	if(err == SD_OK)	/*  ѡ��SD��,���ÿ�����Ե�ַ  */
//		err = bsp_wlan_Select((uint32_t)(SDCardInfo.RCA<<16));
//	
//	if(err == SD_OK)	/*  ��ʼ������λ��Ϊ1  */
//		err = bsp_wlan_SetWideBus(1);
//	
//	/*  SD����������ʱ������߿��Ĺ���ʱ��  */
//	if(err == SD_OK || (SDIO_MULTIMEDIA_CARD==CardType))
//	{
//		if(SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1 || SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
//		{
//			clkdiv = 0x0 + 2;
//		}
//		else
//			clkdiv = 0x0;
//		bsp_sdio_SetClk(clkdiv);
//		err = bsp_wlan_SetDeviceMode(SD_POLLING_MODE);		
//	}
	return err;
}


SD_Error bsp_wlan_PowerCmd(FunctionalState cmd)
{
		SD_Error sd_err = SD_OK;
	uint8_t i = 0;
//	uint32_t response = 0, cnt = 0, validvol = 0;
//	uint32_t SDType = SD_STD_CAPACITY;
	
	if(cmd == ENABLE)
	{
		SDIO->CLKCR = 0;   /*  ���֮ǰ������  */
		SDIO->CLKCR |= 0 << 9;	/*  ��ʡ��ģʽ  */
		SDIO->CLKCR |= 0 << 10;	/*  �ر���·��CK���ݷ�Ƶ�������  */
		SDIO->CLKCR |= 0 << 11;	/*  1λ���ݿ��  */
		SDIO->CLKCR |= 0 << 13; /*  SDIOCLK�����ز���SDIOCK  */
		SDIO->CLKCR |= 0 << 14; /*  �ر�Ӳ��������  */
		
		bsp_sdio_SetClk(0xaa);
		SDIO->POWER = 0x03;
		SDIO->CLKCR |= 1 << 8;
		
//		SDIO->POWER = SDIO_POWER_PWRCTRL;
//		SDIO->CLKCR = SDIO_CLKCR_CLKEN | 178;
		bsp_tim_DelayMs(10);
		
		/*  SDIO�ȷ���CMD5,����Ӧ  */
		SDIO->ARG = 0;
		SDIO->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_WAITRESP_0 | 5;
		while(SDIO->STA & SDIO_STA_CMDACT);
//		for(i = 0; i < 74; i++)
//		{
//			bsp_sdio_SendCmd(SD_CMD_SDIO_SEN_OP_COND, 0, 0);
//			sd_err = bsp_sdio_CmdR4Error();
//			
//			if(sd_err == SD_OK) break;
//		}		
//		sd_err = bsp_sdio_CmdR4Error();
    while (SDIO->STA & SDIO_STA_CTIMEOUT)  
    {  
        SDIO->ICR = SDIO_ICR_CTIMEOUTC; // ????  
        SDIO->CMD = SDIO->CMD; // ??  
        printf("Timeout! Resend CMD%d\r\n", SDIO->CMD & SDIO_CMD_CMDINDEX);  
        while (SDIO->STA & SDIO_STA_CMDACT);  
    }
		if(SDIO->STA & SDIO_STA_CMDREND)
		{
			SDIO->ICR = SDIO_ICR_CMDRENDC;
			bsp_showShortResponse();
		}
		
		SDIO->ARG = 0x300000;
		SDIO->CMD = SDIO->CMD;
		while (SDIO->STA & SDIO_STA_CMDACT);  
if (SDIO->STA & SDIO_STA_CMDREND)  
{  
    SDIO->ICR = SDIO_ICR_CMDRENDC;  
    bsp_showShortResponse();  
    if (SDIO->RESP1 & (1<<31))  
    {  
        // Card is ready to operate after initialization  
        printf("Number of I/O Functions: %d\n", (SDIO->RESP1 >> 28) & 7);  
        printf("Memory Present: %d\n", (SDIO->RESP1 & (1<<27)) != 0);  
    }  
}
//		if(sd_err == SD_OK)
		
	}  /*  end of if(cmd == ENABLE)  */
	else
	{
		SDIO->POWER &= ~(3 << 0);
		return sd_err;
	}
	
	return SD_OK;  /*  ��������������е�����  */
}


