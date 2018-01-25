# include "bsp_sdcard.h"

static uint8_t CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static uint32_t CSD_Tab[4],CID_Tab[4],RCA=0;					//SD��CSD,CID�Լ���Ե�ַ(RCA)����
static uint8_t DeviceMode=SD_DMA_MODE;	
static uint8_t StopCondition=0; 								//�Ƿ���ֹͣ�����־λ,DMA����д��ʱ���õ�  
volatile SD_Error TransferError=SD_OK;					//���ݴ�������־,DMA��дʱʹ��	    
volatile uint8_t TransferEnd=0;	
SD_CardInfo SDCardInfo;

void bsp_sd_GPIOConfig(void)
{	
	RCC->AHB1ENR |= 1<<2;  /* ʹ��PORTCʱ�� */
	RCC->AHB1ENR |= 1<<3;  /* ʹ��PORTDʱ�� */
	RCC->AHB1ENR |= 1<<22;  /* ʹ��DMA2ʱ�� */
	RCC->APB2ENR |= 1<<11;  /* ʹ��SDIOʱ�� */
	RCC->APB2RSTR |= 1<<11; /* ��λSDIO */
	
	
	/* ����GPIOC.8Ϊ�������������������ΪSDIO���� */
	GPIOC->MODER &= ~(3 << (8*2));  /* ��λGPIOC.8,SDIO_D0 */
	GPIOC->MODER |= 2 << (8*2);
	GPIOC->OTYPER &= ~(1 << 8);
	GPIOC->OTYPER |= 0 << 8;
	GPIOC->PUPDR &= ~(3 << (8*2));
	GPIOC->PUPDR |= 1 << (8*2);
	GPIOC->OSPEEDR &= ~(3 << (8*2));
	GPIOC->OSPEEDR |= 2 << (8*2);
	GPIOC->AFR[1] &= ~(0x0f << 0);
	GPIOC->AFR[1] |= (uint32_t)12 << 0;
	
	/* ����GPIOC.9Ϊ�������������������ΪSDIO���� */
	GPIOC->MODER &= ~(3 << (9*2));  /* ��λGPIOC.9,SDIO_D1 */
	GPIOC->MODER |= 2 << (9*2);
	GPIOC->OTYPER &= ~(1 << 9);
	GPIOC->OTYPER |= 0 << 9;
	GPIOC->PUPDR &= ~(3 << (9*2));
	GPIOC->PUPDR |= 1 << (9*2);
	GPIOC->OSPEEDR &= ~(3 << (9*2));
	GPIOC->OSPEEDR |= 2 << (9*2);
	GPIOC->AFR[1] &= ~(0x0f << 4);
	GPIOC->AFR[1] |= (uint32_t)12 << 4;
	
	/* ����GPIOC.10Ϊ�������������������ΪSDIO���� */
	GPIOC->MODER &= ~(3 << (10*2));  /* ��λGPIOC.10,SDIO_D2 */
	GPIOC->MODER |= 2 << (10*2);
	GPIOC->OTYPER &= ~(1 << 10);
	GPIOC->OTYPER |= 0 << 10;
	GPIOC->PUPDR &= ~(3 << (10*2));
	GPIOC->PUPDR |= 1 << (10*2);
	GPIOC->OSPEEDR &= ~(3 << (10*2));
	GPIOC->OSPEEDR |= 2 << (10*2);
	GPIOC->AFR[1] &= ~(0x0f << 8);
	GPIOC->AFR[1] |= (uint32_t)12 << 8;
	
	/* ����GPIOC.11Ϊ�������������������ΪSDIO���� */
	GPIOC->MODER &= ~(3 << (11*2));  /* ��λGPIOC.11,SDIO_D3 */
	GPIOC->MODER |= 2 << (11*2);
	GPIOC->OTYPER &= ~(1 << 11);
	GPIOC->OTYPER |= 0 << 11;
	GPIOC->PUPDR &= ~(3 << (11*2));
	GPIOC->PUPDR |= 1 << (11*2);
	GPIOC->OSPEEDR &= ~(3 << (11*2));
	GPIOC->OSPEEDR |= 2 << (11*2);
	GPIOC->AFR[1] &= ~(0x0f << 12);
	GPIOC->AFR[1] |= (uint32_t)12 << 12;
	
	/* ����GPIOC.12Ϊ�������������������ΪSDIO���� */
	GPIOC->MODER &= ~(3 << (12*2));  /* ��λGPIOC.12,SDIO_CLK */
	GPIOC->MODER |= 2 << (12*2);
	GPIOC->OTYPER &= ~(1 << 12);
	GPIOC->OTYPER |= 0 << 12;
	GPIOC->PUPDR &= ~(3 << (12*2));
	GPIOC->PUPDR |= 1 << (12*2);
	GPIOC->OSPEEDR &= ~(3 << (12*2));
	GPIOC->OSPEEDR |= 2 << (12*2);
	GPIOC->AFR[1] &= ~(0x0f << 16);
	GPIOC->AFR[1] |= (uint32_t)12 << 16;
	
	/* ����GPIOD.2Ϊ�������������������ΪSDIO���� */
	GPIOD->MODER &= ~(3 << (2*2));  /* ��λGPIOD.2,SDIO_CMD */
	GPIOD->MODER |= 2 << (2*2);
	GPIOD->OTYPER &= ~(1 << 2);
	GPIOD->OTYPER |= 0 << 2;
	GPIOD->PUPDR &= ~(3 << (2*2));
	GPIOD->PUPDR |= 1 << (2*2);
	GPIOD->OSPEEDR &= ~(3 << (2*2));
	GPIOD->OSPEEDR |= 2 << (2*2);
	GPIOD->AFR[0] &= ~(0x0f << 8);
	GPIOD->AFR[0] |= (uint32_t)12 << 8;
	
	RCC->APB2RSTR &= ~(1 << 11);  /* ������λSDIO */
	
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

/*
*********************************************************************************************************
*                                       bsp_sd_NVICConfig   
*
* Description: ��ʼ��SDIO�ж�
*             
* Arguments  : None.
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_sd_NVICConfig(void)
{
	//HAL_NVIC_SetPriority(SDIO_IRQn, 0, 0);
}


/*
*********************************************************************************************************
*                                          bsp_sd_Config
*
* Description: ��ʼ��SD��
*             
* Arguments  : None.
*
* Reutrn     : 1> SD_OK:��ʼ��SD���ɹ�
*		           2> ����:�������
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_Config(void)
{
	SD_Error err = SD_OK;
	uint8_t clkdiv = 0;
	
	bsp_sd_GPIOConfig(); /*  SDIO���ų�ʼ��  */
	bsp_sd_NVICConfig(); /*  NVIC��ʼ��  */
	
	err = bsp_sd_PowerCmd(ENABLE);	/*  ���ϵ�  */
	
	if(err == SD_OK)  /*  �ϵ�ɹ����ʼ��SD��  */
		err = bsp_sd_CardInit();
	
	if(err == SD_OK)	/*  ��ʼ�����ȡ������Ϣ  */
		err = bsp_sd_GetCardInfo(&SDCardInfo);
	
	if(err == SD_OK)	/*  ѡ��SD��,���ÿ�����Ե�ַ  */
		err = bsp_sd_Select((uint32_t)(SDCardInfo.RCA<<16));
	
	if(err == SD_OK)	/*  ��ʼ������λ��Ϊ1  */
		err = bsp_sd_SetWideBus(1);
	
	/*  SD����������ʱ������߿��Ĺ���ʱ��  */
	if(err == SD_OK || (SDIO_MULTIMEDIA_CARD==CardType))
	{
		if(SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1 || SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
		{
			clkdiv = 0x0 + 2;
		}
		else
			clkdiv = 0x0;
		bsp_sdio_SetClk(clkdiv);
		err = bsp_sd_SetDeviceMode(SD_POLLING_MODE);		
	}
	return err;
}



/*
*********************************************************************************************************
*                                      bsp_sd_PowerCmd    
*
* Description: SD���ϵ纯��
*             
* Arguments  : cmd: 1> ENABLE:�ϵ�
*                   2> DISABLE: �ϵ�
*
* Reutrn     : 1> SD_OK: �ϵ�ɹ�
*              2> ����: �������
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_PowerCmd(FunctionalState cmd)
{
	SD_Error sd_err = SD_OK;
	uint8_t i = 0;
	uint32_t response = 0, cnt = 0, validvol = 0;
	uint32_t SDType = SD_STD_CAPACITY;
	
	if(cmd == ENABLE)
	{
		SDIO->CLKCR = 0;   /*  ���֮ǰ������  */
		SDIO->CLKCR |= 0 << 9;	/*  ��ʡ��ģʽ  */
		SDIO->CLKCR |= 0 << 10;	/*  �ر���·��CK���ݷ�Ƶ�������  */
		SDIO->CLKCR |= 0 << 11;	/*  1λ���ݿ��  */
		SDIO->CLKCR |= 0 << 13; /*  SDIOCLK�����ز���SDIOCK  */
		SDIO->CLKCR |= 0 << 14; /*  �ر�Ӳ��������  */
		
		bsp_sdio_SetClk(0x76);
		SDIO->POWER = 0x03;
		SDIO->CLKCR |= 1 << 8;
		
		
		/*  ���ȷ���CMD0  */
		for(i = 0; i < 74; i++)
		{
			bsp_sdio_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0);
			printf("SDIO->STA:%08x\r\n",SDIO->STA);
			sd_err = bsp_sdio_CmdR0Error();
			if(sd_err == SD_OK) 
				break;
		}
		if(sd_err != SD_OK) return sd_err;
		
		/*  ����CMD8,����Ӧ��ΪV2.0���߸��ٵĿ�,û����ΪMMC��  */
		bsp_sdio_SendCmd(SDIO_SEND_IF_COND, 1, SD_CHECK_PATTERN);
		sd_err = bsp_sdio_CmdR7Error();
		
		if(sd_err == SD_OK)  /*  ����Ӧ,˵��ΪV2.0���߸���Ŀ�  */
		{
			CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
			SDType = SD_HIGH_CAPACITY;
		}
		
		/*  ����CMD55������Ӧ  */
		bsp_sdio_SendCmd(SD_CMD_APP_CMD, 1, 0);
		sd_err = bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
		if(sd_err == SD_OK)
		{
			while((!validvol) && (cnt < SD_MAX_VOLT_TRIAL))
			{
				/*  ����CMD55  */
				bsp_sdio_SendCmd(SD_CMD_APP_CMD, 1, 0);
				sd_err = bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
				if(sd_err != SD_OK) return sd_err;
				
				/*  ����CMD41,����Ӧ  */
				bsp_sdio_SendCmd(SD_CMD_SD_APP_OP_COND, 1, SD_VOLTAGE_WINDOW_MMC | SDType);
				sd_err = bsp_sdio_CmdR3Error();
				if(sd_err != SD_OK) return sd_err;
				
				response = SDIO->RESP1;
				validvol = (((response >> 31) == 1) ? 1 : 0);
				cnt++;
			}/*  end of while  */
			
			if(cnt >= SD_MAX_VOLT_TRIAL) 
			{
				sd_err = SD_INVALID_VOLTRANGE;
				return sd_err;
			}
			
			if(response &= SD_HIGH_CAPACITY)
			{
				CardType = SDIO_HIGH_CAPACITY_SD_CARD;
			}
		}   /*  end of if(sd_err == SD_OK)  */
		else  /*  MMC��  */
		{
			/*  MMC��,����CMD1,SDIO_SEND_OP_COND,����:0X80FF800  */
			while((!validvol) && (cnt < SD_MAX_VOLT_TRIAL))
			{
				/*  ����CMD1  */
				bsp_sdio_SendCmd(SD_CMD_SEND_OP_COND, 1, SD_VOLTAGE_WINDOW_MMC);
				sd_err = bsp_sdio_CmdR3Error();
				if(sd_err != SD_OK) return sd_err;
				
				response = SDIO->RESP1;
				validvol = (((response >> 31) == 1) ? 1 : 0);
				cnt++;
			} /*  end of while((!validvol) && (cnt < SD_MAX_VOLT_TRIAL))  */
			
			if(cnt >= SD_MAX_VOLT_TRIAL) 
			{
				sd_err = SD_INVALID_VOLTRANGE;
				return sd_err;
			}
			CardType = SDIO_MULTIMEDIA_CARD;
		}/*  end of else  */
		return sd_err;
	}  /*  end of if(cmd == ENABLE)  */
	else
	{
		SDIO->POWER &= ~(3 << 0);
		return sd_err;
	}
	
	return SD_OK;  /*  ��������������е�����  */
}

/*
*********************************************************************************************************
*                                     bsp_sd_CardInit     
*
* Description: ��ʼ��SD��,���ÿ��������״̬
*             
* Arguments  : None.
*
* Reutrn     : 1> SD_OK
*              2> ����:�������
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_CardInit(void)
{
	SD_Error sd_err = SD_OK;
	uint16_t rca = 0x01;
	
	if((SDIO->POWER & 0x03) == 0)  /*  SD��δ�ϵ�  */
		return SD_REQUEST_NOT_APPLICABLE;
	
	/*  ����SD��  */
	if(SDIO_SECURE_DIGITAL_IO_CARD != CardType)
	{
		/*  ����CMD2,ȡ��CID,����Ӧ  */
		bsp_sdio_SendCmd(SD_CMD_ALL_SEND_CID, 3, 0);
		sd_err = bsp_sdio_CmdR2Error();
		if(sd_err != SD_OK) return sd_err;
		CID_Tab[0] = SDIO->RESP1;
		CID_Tab[1] = SDIO->RESP2;
		CID_Tab[2] = SDIO->RESP3;
		CID_Tab[3] = SDIO->RESP4;
	}/*  end of if(SDIO_SECURE_DIGITAL_IO_CARD != CardType)  */
	
	/*  �жϿ�������  */
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || 
		 (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) || 
	   (SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType) || 
	   (SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		/*  ����CMD33,����Ӧ  */
		bsp_sdio_SendCmd(SD_CMD_SET_REL_ADDR, 1, 0);
		sd_err = bsp_sdio_CmdR6Error(SD_CMD_SET_REL_ADDR, &rca);
		if(sd_err != SD_OK) return sd_err;
	} /*  end of if �������ж�  */
	
	
	if(SDIO_MULTIMEDIA_CARD==CardType)
	{
		bsp_sdio_SendCmd(SD_CMD_SET_REL_ADDR, 1, (uint32_t)(rca << 16));
		sd_err = bsp_sdio_CmdR2Error();
		if(sd_err != SD_OK) return sd_err;
	} /*  end if(SDIO_MULTIMEDIA_CARD==CardType)  */
	
	if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)
	{
		RCA = rca;
		bsp_sdio_SendCmd(SD_CMD_SEND_CSD, 3, (uint32_t)(rca << 16));
		sd_err = bsp_sdio_CmdR2Error();
		if(sd_err != SD_OK) return sd_err;
		CSD_Tab[0]=SDIO->RESP1;
		CSD_Tab[1]=SDIO->RESP2;
		CSD_Tab[2]=SDIO->RESP3;						
		CSD_Tab[3]=SDIO->RESP4;	
	} /*  end if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType) */
	
	return SD_OK;
}

/*
*********************************************************************************************************
*                                          bsp_sd_GetCardInfo
*
* Description: ��ȡ����Ϣ����
*             
* Arguments  : CardInfo:  SD_CardInfo�ṹ��ָ��
*
* Reutrn     : 1> SD_OK: �ɹ���ȡ��Ϣ
*              2> ����: �������
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_GetCardInfo(SD_CardInfo *CardInfo)
{
	SD_Error sd_err = SD_OK;
	uint8_t tmp = 0;
	
	CardInfo->CardType = (uint8_t )CardType;
	CardInfo->RCA = (uint16_t)RCA;
	tmp = (uint8_t )((CSD_Tab[0] &0XFF000000)>> 24);
	CardInfo->SD_csd.CSDStruct = (tmp & 0xc0) >> 6;
	CardInfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨���
	CardInfo->SD_csd.Reserved1=tmp&0x03;			//2������λ  
	tmp=(uint8_t)((CSD_Tab[0]&0x00FF0000)>>16);			//��1���ֽ�
	CardInfo->SD_csd.TAAC=tmp;				   		//���ݶ�ʱ��1
	tmp=(uint8_t)((CSD_Tab[0]&0x0000FF00)>>8);	  		//��2���ֽ�
	CardInfo->SD_csd.NSAC=tmp;		  				//���ݶ�ʱ��2
	tmp=(uint8_t)(CSD_Tab[0]&0x000000FF);				//��3���ֽ�
	CardInfo->SD_csd.MaxBusClkFrec=tmp;		  		//�����ٶ�	   
	tmp=(uint8_t)((CSD_Tab[1]&0xFF000000)>>24);			//��4���ֽ�
	CardInfo->SD_csd.CardComdClasses=tmp<<4;    	//��ָ�������λ
	tmp=(uint8_t)((CSD_Tab[1]&0x00FF0000)>>16);	 		//��5���ֽ�
	CardInfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//��ָ�������λ
	CardInfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//����ȡ���ݳ���
	tmp=(uint8_t)((CSD_Tab[1]&0x0000FF00)>>8);			//��6���ֽ�
	CardInfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//����ֿ��
	CardInfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//д���λ
	CardInfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//�����λ
	CardInfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	CardInfo->SD_csd.Reserved2=0; 					//����
	
	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1) || 
		 (CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0) || 
	   (SDIO_MULTIMEDIA_CARD==CardType))
	{
		CardInfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12λ)
	 	tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 			//��7���ֽ�	
		CardInfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24);		//��8���ֽ�	
		CardInfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		CardInfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		CardInfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�	
		CardInfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		CardInfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		CardInfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
 		tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8);	  	//��10���ֽ�	
		CardInfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		CardInfo->CardCapacity=(CardInfo->SD_csd.DeviceSize+1);//���㿨����
		CardInfo->CardCapacity*=(1<<(CardInfo->SD_csd.DeviceSizeMul+2));
		CardInfo->CardBlockSize=1<<(CardInfo->SD_csd.RdBlockLen);//���С
		CardInfo->CardCapacity*=CardInfo->CardBlockSize;
	}/*  end if  */
	else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)
	{
		tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 		//��7���ֽ�	
		CardInfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
 		tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24); 	//��8���ֽ�	
 		CardInfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);	//��9���ֽ�	
 		CardInfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8); 	//��10���ֽ�	
 		CardInfo->CardCapacity=(long long)(CardInfo->SD_csd.DeviceSize+1)*512*1024;//���㿨����
		CardInfo->CardBlockSize=512; 			//���С�̶�Ϊ512�ֽ�
	}/*  end else if  */
	
	CardInfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	CardInfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(uint8_t)(CSD_Tab[2]&0x000000FF);			//��11���ֽ�	
	CardInfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	CardInfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(uint8_t)((CSD_Tab[3]&0xFF000000)>>24);		//��12���ֽ�	
	CardInfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	CardInfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	CardInfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	CardInfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(uint8_t)((CSD_Tab[3]&0x00FF0000)>>16);		//��13���ֽ�
	CardInfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	CardInfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	CardInfo->SD_csd.Reserved3=0;
	CardInfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(uint8_t)((CSD_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	CardInfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	CardInfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	CardInfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	CardInfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	CardInfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	CardInfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(uint8_t)(CSD_Tab[3]&0x000000FF);			//��15���ֽ�
	CardInfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	CardInfo->SD_csd.Reserved4=1;		 
	tmp=(uint8_t)((CID_Tab[0]&0xFF000000)>>24);		//��0���ֽ�
	CardInfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(uint8_t)((CID_Tab[0]&0x00FF0000)>>16);		//��1���ֽ�
	CardInfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(uint8_t)((CID_Tab[0]&0x000000FF00)>>8);		//��2���ֽ�
	CardInfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(uint8_t)(CID_Tab[0]&0x000000FF);			//��3���ֽ�	
	CardInfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(uint8_t)((CID_Tab[1]&0xFF000000)>>24); 		//��4���ֽ�
	CardInfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(uint8_t)((CID_Tab[1]&0x00FF0000)>>16);	   	//��5���ֽ�
	CardInfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(uint8_t)((CID_Tab[1]&0x0000FF00)>>8);		//��6���ֽ�
	CardInfo->SD_cid.ProdName1|=tmp;		   
	tmp=(uint8_t)(CID_Tab[1]&0x000000FF);	  		//��7���ֽ�
	CardInfo->SD_cid.ProdName2=tmp;			  
	tmp=(uint8_t)((CID_Tab[2]&0xFF000000)>>24); 		//��8���ֽ�
	CardInfo->SD_cid.ProdRev=tmp;		 
	tmp=(uint8_t)((CID_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
	CardInfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(uint8_t)((CID_Tab[2]&0x0000FF00)>>8); 		//��10���ֽ�
	CardInfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(uint8_t)(CID_Tab[2]&0x000000FF);   			//��11���ֽ�
	CardInfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(uint8_t)((CID_Tab[3]&0xFF000000)>>24); 		//��12���ֽ�
	CardInfo->SD_cid.ProdSN|=tmp;			     
	tmp=(uint8_t)((CID_Tab[3]&0x00FF0000)>>16);	 	//��13���ֽ�
	CardInfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	CardInfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(uint8_t)((CID_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
	CardInfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(uint8_t)(CID_Tab[3]&0x000000FF);			//��15���ֽ�
	CardInfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	CardInfo->SD_cid.Reserved2=1;	 
	
	return sd_err;
}

/*
*********************************************************************************************************
*                                          bsp_sd_SetWideBus
*
* Description: ����SD������λ��
*             
* Arguments  : mode = 1 һλ����λ��, mode = 2 4λ����λ��
*
* Reutrn     : 1> SD_OK: ���óɹ�
*              2> ����: �������
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_SetWideBus(uint32_t mode)
{
	SD_Error sd_err = SD_OK;
	
	/*  MMC����֧��  */
	if(SDIO_MULTIMEDIA_CARD==CardType)
	{
		return SD_UNSUPPORTED_FEATURE;
	}
	else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || 
		      (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) || 
	        (SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		/*  ��֧��8λģʽ  */
		if(mode >= 2) 
		{
			return SD_UNSUPPORTED_FEATURE;
		} 
		else
		{
			sd_err = bsp_sd_EnWideBus(mode);
			if(sd_err == SD_OK)
			{
				SDIO->CLKCR&=~(3<<11);		//���֮ǰ��λ������    
				SDIO->CLKCR|=(uint16_t)mode<<11;//1λ/4λ���߿�� 
				SDIO->CLKCR|=0<<14;			//������Ӳ��������
			}
		} /*  end else  */
	}/*  end else if  */
	return sd_err;
}


/*
*********************************************************************************************************
*                                          bsp_sd_SetDeviceMode
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
SD_Error bsp_sd_SetDeviceMode(uint32_t mode)
{
	SD_Error sd_err = SD_OK;
 	if((mode==SD_DMA_MODE)||(mode==SD_POLLING_MODE))DeviceMode=mode;
	else sd_err=SD_INVALID_PARAMETER;
	return sd_err;	
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
SD_Error bsp_sd_Select(uint32_t addr)
{
	bsp_sdio_SendCmd(SD_CMD_SEL_DESEL_CARD,1,addr);	//����CMD7,ѡ��,����Ӧ	 	   
	return bsp_sdio_CmdR1Error(SD_CMD_SEL_DESEL_CARD);
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
SD_Error bsp_sd_SetStatus(uint32_t *CardStatus)
{
	SD_Error errorstatus = SD_OK;
	if(CardStatus==NULL)
	{
		errorstatus=SD_INVALID_PARAMETER;
		return errorstatus;
	}
 	bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,RCA<<16);	//����CMD13,����Ӧ		 
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SEND_STATUS);	//��ѯ��Ӧ״̬ 
	if(errorstatus!=SD_OK)return errorstatus;
	*CardStatus=SDIO->RESP1;//��ȡ��Ӧֵ
	return errorstatus;
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
SD_CardState bsp_sd_GetStatus(void)
{
	uint32_t resp1=0;
	if(bsp_sd_SetStatus(&resp1)!=SD_OK)return SD_CARD_ERROR;
	else return (SD_CardState)((resp1>>9) & 0x0F);
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
SD_Error bsp_sd_ReadBlock(uint8_t *buff, long long addr, uint16_t blksize)
{
	SD_Error errorstatus=SD_OK;
	uint8_t power;
	uint32_t count=0,*tempbuff=(uint32_t*)buff;//ת��Ϊuint32_tָ�� 
	uint32_t timeout=SDIO_DATATIMEOUT;   
	
	if(NULL==buff)  /*  ���ղ�������  */
		return SD_INVALID_PARAMETER; 

	SDIO->DCTRL=0x0;	//���ݿ��ƼĴ�������(��DMA)   
	
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}   
	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)
		return SD_LOCK_UNLOCK_FAILED;//������
	
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    	   
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus != SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	  
	
	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,blksize,power,1);	//blksize,����������	  
	bsp_sdio_SendCmd(SD_CMD_READ_SINGLE_BLOCK,1,addr);		//����CMD17+��addr��ַ����ȡ����,����Ӧ 	   
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_READ_SINGLE_BLOCK);//�ȴ�R1��Ӧ   
	
	if(errorstatus!=SD_OK)return errorstatus;   		//��Ӧ����	 
	
 	if(DeviceMode==SD_POLLING_MODE)						//��ѯģʽ,��ѯ����	 
	{
 		DISABLE_INT();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
		{
			if(SDIO->STA&(1<<15))						//����������,��ʾ���ٴ���8����
			{
				for(count=0;count<8;count++)			//ѭ����ȡ����
				{
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff+=8;	 
				timeout=0X7FFFFF; 	//���������ʱ��
			}else 	//����ʱ
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
		{										   
	 		SDIO->ICR|=1<<3; 		//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
		{
	 		SDIO->ICR|=1<<1; 		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<5)) 	//����fifo�������
		{
	 		SDIO->ICR|=1<<5; 		//������־
			return SD_RX_OVERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
		{
	 		SDIO->ICR|=1<<9; 		//������־
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO->STA&(1<<21))	//FIFO����,�����ڿ�������
		{
			*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
			tempbuff++;
		}
		ENABLE_INT();//�������ж�
		SDIO->ICR=0X5FF;	 		//������б��
	}else if(DeviceMode==SD_DMA_MODE)
	{
 		TransferError=SD_OK;
		StopCondition=0;			//�����,����Ҫ����ֹͣ����ָ��
		TransferEnd=0;				//�����������λ�����жϷ�����1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж� 
	 	SDIO->DCTRL|=1<<3;		 	//SDIO DMAʹ�� 
		bsp_sd_DMAConfig((uint32_t*)buff,blksize,0); 
 		while(((DMA2->LISR&(1<<27))==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//�ȴ�������� 
		if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
		if(TransferError!=SD_OK)errorstatus=TransferError;  
    }   
 	return errorstatus; 
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
__align(4) uint32_t *tempbuff;
SD_Error bsp_sd_ReadBlocks(uint8_t *buff, long long addr, uint16_t blksize, uint32_t nblks)
{
	SD_Error errorstatus=SD_OK;
	uint8_t power;
	uint32_t count=0;
	uint32_t timeout=SDIO_DATATIMEOUT;  
	tempbuff=(uint32_t*)buff;//ת��Ϊuint32_tָ��
	
	SDIO->DCTRL=0x0;		//���ݿ��ƼĴ�������(��DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}  
  bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	  
	if(nblks>1)											//����  
	{									    
 	  	if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;//�ж��Ƿ񳬹������ճ���
		bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,nblks*blksize,power,1);//nblks*blksize,512���С,����������	  
	  	bsp_sdio_SendCmd(SD_CMD_READ_MULT_BLOCK,1,addr);	//����CMD18+��addr��ַ����ȡ����,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_READ_MULT_BLOCK);//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	  
 		if(DeviceMode==SD_POLLING_MODE)
		{
			DISABLE_INT();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
			{
				if(SDIO->STA&(1<<15))						//����������,��ʾ���ٴ���8����
				{
					for(count=0;count<8;count++)			//ѭ����ȡ����
					{
						*(tempbuff+count)=SDIO->FIFO;
					}
					tempbuff+=8;	 
					timeout=0X7FFFFF; 	//���������ʱ��
				}else 	//����ʱ
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}  
			if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
			{										   
		 		SDIO->ICR|=1<<3; 		//������־
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
			{
		 		SDIO->ICR|=1<<1; 		//������־
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<5)) 	//����fifo�������
			{
		 		SDIO->ICR|=1<<5; 		//������־
				return SD_RX_OVERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
			{
		 		SDIO->ICR|=1<<9; 		//������־
				return SD_START_BIT_ERR;		 
			}   
			while(SDIO->STA&(1<<21))	//FIFO����,�����ڿ�������
			{
				*tempbuff=SDIO->FIFO;	//ѭ����ȡ����
				tempbuff++;
			}
	 		if(SDIO->STA&(1<<8))		//���ս���
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					bsp_sdio_SendCmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12+�������� 	   
					errorstatus=bsp_sdio_CmdR1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
 			}
			ENABLE_INT();//�������ж�
	 		SDIO->ICR=0X5FF;	 		//������б�� 
 		}else if(DeviceMode==SD_DMA_MODE)
		{
	   		TransferError=SD_OK;
			StopCondition=1;			//����,��Ҫ����ֹͣ����ָ�� 
			TransferEnd=0;				//�����������λ�����жϷ�����1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж� 
		 	SDIO->DCTRL|=1<<3;		 						//SDIO DMAʹ�� 
			bsp_sd_DMAConfig((uint32_t*)buff,nblks*blksize,0); 
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ�������� 
			if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
			while((TransferEnd==0)&&(TransferError==SD_OK)); 
			if(TransferError!=SD_OK)errorstatus=TransferError;  	 
		}		 
  	}
	return errorstatus;
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
SD_Error bsp_sd_WriteBlock(uint8_t *buff, long long addr, uint16_t blksize)
{
	SD_Error errorstatus = SD_OK;
	uint8_t  power=0,cardstate=0;
	uint32_t timeout=0,bytestransferred=0;
	uint32_t cardstatus=0,count=0,restwords=0;
	uint32_t	tlen=blksize;						//�ܳ���(�ֽ�)
	uint32_t*tempbuff=(uint32_t*)buff;		
	
 	if(buff==NULL)return SD_INVALID_PARAMETER;//��������   
  	SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)   
  	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	 
  bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,(uint32_t)RCA<<16);	//����CMD13,��ѯ����״̬,����Ӧ 	   
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SEND_STATUS);		//�ȴ�R1��Ӧ   		   
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;													  
	timeout=SD_DATATIMEOUT;
   	while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//���READY_FOR_DATAλ�Ƿ���λ
	{
		timeout--;
		bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,(uint32_t)RCA<<16);//����CMD13,��ѯ����״̬,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SEND_STATUS);	//�ȴ�R1��Ӧ   		   
		if(errorstatus!=SD_OK)return errorstatus;				    
		cardstatus=SDIO->RESP1;													  
	}
	if(timeout==0)return SD_ERROR;
   	bsp_sdio_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK,1,addr);	//����CMD24,д����ָ��,����Ӧ 	   
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_WRITE_SINGLE_BLOCK);//�ȴ�R1��Ӧ   		   
	if(errorstatus!=SD_OK)return errorstatus;   	  
	StopCondition=0;									//����д,����Ҫ����ֹͣ����ָ�� 
 	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,blksize,power,0);	//blksize, ����������	  
	timeout=SDIO_DATATIMEOUT;
	if (DeviceMode == SD_POLLING_MODE)
	{
		DISABLE_INT();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//���ݿ鷢�ͳɹ�/����/CRC/��ʱ/��ʼλ����
		{
			if(SDIO->STA&(1<<14))							//���������,��ʾ���ٴ���8����
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
				{
					restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
					
					for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
					{
						SDIO->FIFO=*tempbuff;
					}
				}else
				{
					for(count=0;count<8;count++)
					{
						SDIO->FIFO=*(tempbuff+count);
					}
					tempbuff+=8;
					bytestransferred+=32;
				}
				timeout=0X3FFFFFFF;	//д�������ʱ��
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
		{										   
	 		SDIO->ICR|=1<<3; 		//������־
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
		{
	 		SDIO->ICR|=1<<1; 		//������־
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<4)) 	//����fifo�������
		{
	 		SDIO->ICR|=1<<4; 		//������־
			return SD_TX_UNDERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
		{
	 		SDIO->ICR|=1<<9; 		//������־
			return SD_START_BIT_ERR;		 
		}   
		ENABLE_INT();//�������ж�
		SDIO->ICR=0X5FF;	 		//������б��	  
	}else if(DeviceMode==SD_DMA_MODE)
	{
   		TransferError=SD_OK;
		StopCondition=0;			//����д,����Ҫ����ֹͣ����ָ�� 
		TransferEnd=0;				//�����������λ�����жϷ�����1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
		bsp_sd_DMAConfig((uint32_t*)buff,blksize,1);				//SDIO DMA����
 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.  
 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ�������� 
		if(timeout==0)
		{
  			bsp_sd_Config();	 					//���³�ʼ��SD��,���Խ��д������������
			return SD_DATA_TIMEOUT;			//��ʱ	 
 		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ	 
  		if(TransferError!=SD_OK)return TransferError;
 	}  
 	SDIO->ICR=0X5FF;	 		//������б��
 	errorstatus=bsp_sd_IsBusy(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=bsp_sd_IsBusy(&cardstate);
	}   
	return errorstatus;
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
SD_Error bsp_sd_WriteBlocks(uint8_t *buff, long long addr, uint16_t blksize, uint32_t nblks)
{
		SD_Error errorstatus = SD_OK;
	uint8_t  power = 0, cardstate = 0;
	uint32_t timeout=0,bytestransferred=0;
	uint32_t count = 0, restwords = 0;
	uint32_t tlen=nblks*blksize;				//�ܳ���(�ֽ�)
	uint32_t *tempbuff = (uint32_t*)buff;  
  	if(buff==NULL)return SD_INVALID_PARAMETER; //��������  
  	SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)   
  	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//���DPSM״̬������
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//����CMD16+�������ݳ���Ϊblksize,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ   
		if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����	 
	}else return SD_INVALID_PARAMETER;	 
	if(nblks>1)
	{					  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
			//�������
	 	   	bsp_sdio_SendCmd(SD_CMD_APP_CMD,1,(uint32_t)RCA<<16);	//����ACMD55,����Ӧ 	   
			errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);		//�ȴ�R1��Ӧ   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
	 	   	bsp_sdio_SendCmd(SD_CMD_SET_BLOCK_COUNT,1,nblks);	//����CMD23,���ÿ�����,����Ӧ 	   
			errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCK_COUNT);//�ȴ�R1��Ӧ   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
		} 
		bsp_sdio_SendCmd(SD_CMD_WRITE_MULT_BLOCK,1,addr);		//����CMD25,���дָ��,����Ӧ 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_WRITE_MULT_BLOCK);	//�ȴ�R1��Ӧ   		   
		if(errorstatus!=SD_OK)return errorstatus;
 	 	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,nblks*blksize,power,0);//blksize, ����������	
		if(DeviceMode==SD_POLLING_MODE)
	    {
			timeout=SDIO_DATATIMEOUT;
			DISABLE_INT();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//����/CRC/���ݽ���/��ʱ/��ʼλ����
			{
				if(SDIO->STA&(1<<14))							//���������,��ʾ���ٴ���8��(32�ֽ�)
				{	  
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
					{
						restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
						{
							SDIO->FIFO=*tempbuff;
						}
					}else 										//���������,���Է�������8��(32�ֽ�)����
					{
						for(count=0;count<SD_HALFFIFO;count++)
						{
							SDIO->FIFO=*(tempbuff+count);
						}
						tempbuff+=SD_HALFFIFO;
						bytestransferred+=SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;	//д�������ʱ��
				}else
				{
					if(timeout==0)return SD_DATA_TIMEOUT; 
					timeout--;
				}
			} 
			if(SDIO->STA&(1<<3))		//���ݳ�ʱ����
			{										   
		 		SDIO->ICR|=1<<3; 		//������־
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//���ݿ�CRC����
			{
		 		SDIO->ICR|=1<<1; 		//������־
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<4)) 	//����fifo�������
			{
		 		SDIO->ICR|=1<<4; 		//������־
				return SD_TX_UNDERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//������ʼλ����
			{
		 		SDIO->ICR|=1<<9; 		//������־
				return SD_START_BIT_ERR;		 
			}   										   
			if(SDIO->STA&(1<<8))		//���ͽ���
			{															 
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					bsp_sdio_SendCmd(SD_CMD_STOP_TRANSMISSION,1,0);		//����CMD12+�������� 	   
					errorstatus=bsp_sdio_CmdR1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
			}
			ENABLE_INT();//�������ж�
	 		SDIO->ICR=0X5FF;	 		//������б�� 
	    }else if(DeviceMode==SD_DMA_MODE)
		{
	   		TransferError=SD_OK;
			StopCondition=1;			//���д,��Ҫ����ֹͣ����ָ�� 
			TransferEnd=0;				//�����������λ�����жϷ�����1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
			bsp_sd_DMAConfig((uint32_t*)buff,nblks*blksize,1);		//SDIO DMA����
	 	 	SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��. 
			timeout=SDIO_DATATIMEOUT;
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ�������� 
			if(timeout==0)	 								//��ʱ
			{									  
				bsp_sd_Config();	 					//���³�ʼ��SD��,���Խ��д������������
	 			return SD_DATA_TIMEOUT;			//��ʱ	 
	 		}
			timeout=SDIO_DATATIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
	 		if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ	 
	 		if(TransferError!=SD_OK)return TransferError;	 
		}
  	}
 	SDIO->ICR=0X5FF;	 		//������б��
 	errorstatus=bsp_sd_IsBusy(&cardstate);
 	while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
	{
		errorstatus=bsp_sd_IsBusy(&cardstate);
	}   
	return errorstatus;	   
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
SD_Error bsp_sd_ProcessIRQSrc(void)
{
	if(SDIO->STA & (1 << 8))//��������ж�
	{	 
		if(StopCondition == 1)
		{
			bsp_sdio_SendCmd(SD_CMD_STOP_TRANSMISSION, 1, 0);		//����CMD12,�������� 	   
			TransferError = bsp_sdio_CmdR1Error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;	
		
 		SDIO->ICR|=1<<8;//�������жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
 		TransferEnd = 1;
		
		return(TransferError);
	}/*  end if(SDIO->STA & (1 << 8))  */
	
	
 	if(SDIO->STA&(1<<1))//����CRC����
	{
		SDIO->ICR |= 1 << 1;//����жϱ��
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
		TransferError = SD_DATA_CRC_FAIL;
		return(SD_DATA_CRC_FAIL);
	}/*  end if(SDIO->STA&(1<<1))  */
	
	
 	if(SDIO->STA&(1<<3))//���ݳ�ʱ����
	{
		SDIO->ICR|=1<<3;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
		TransferError = SD_DATA_TIMEOUT;
		return(SD_DATA_TIMEOUT);
	}
	if(SDIO->STA&(1<<5))//FIFO�������
	{
		SDIO->ICR|=1<<5;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
		TransferError = SD_RX_OVERRUN;
		return(SD_RX_OVERRUN);
	}
	if(SDIO->STA&(1<<4))//FIFO�������
	{
		SDIO->ICR|=1<<4;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
		TransferError = SD_TX_UNDERRUN;
		return(SD_TX_UNDERRUN);
	}
	if(SDIO->STA&(1<<9))//��ʼλ����
	{
		SDIO->ICR|=1<<9;//����жϱ��
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
		TransferError = SD_START_BIT_ERR;
		return(SD_START_BIT_ERR);
	}
	return(SD_OK);
}


/*
*********************************************************************************************************
*                                 bsp_sdio_CmdR0Error         
*
* Description: CMD0����Ӧ,����CMD0��˵,������Ӧ,ֻ�������Ѿ�����
*             
* Arguments  : None.
*
* Reutrn     : 1> SD_OK:����ִ�гɹ�
*							 2> SD_CMD_RSP_TIMEOUT:������Ӧ��ʱ
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sdio_CmdR0Error(void)
{
	SD_Error sd_err = SD_OK;
	uint32_t timeout = SDIO_CMD0TIMEOUT;
	
	while(timeout--)
	{
		if((SDIO->STA & (1 << 7))) /*  λ7�����ѷ���,����Ҫ��Ӧ  */
			break;
	}
	
	/*  ������Ӧ��ʱ  */
	if(timeout == 0) return SD_CMD_RSP_TIMEOUT;
	SDIO->ICR = 0X5FF;  /*  ����ж�����Ĵ���  */
	return sd_err;
}


/*
*********************************************************************************************************
*                                 bsp_sdio_CmdR1Error         
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
SD_Error bsp_sdio_CmdR1Error(uint8_t cmd)
{
	uint32_t status; 
	while(1)
	{
		status = SDIO->STA;
		if(status & ((1 << 0) | (1 << 2) | (1 << 6)))
			break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
	} 
	if(status & (1 << 2))					//��Ӧ��ʱ
	{																				    
 		SDIO->ICR = 1 << 2;					//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	
 	if(status & (1 << 0))					//CRC����
	{																				    
 		SDIO->ICR = 1 << 0;					//�����־
		return SD_CMD_CRC_FAIL;
	}		
	if(SDIO->RESPCMD != cmd)
		return SD_ILLEGAL_CMD;//���ƥ�� 
	SDIO->ICR=0X5FF;	 				//������
	return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS);//���ؿ���Ӧ
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
SD_Error bsp_sdio_CmdR2Error(void)
{
		SD_Error errorstatus=SD_OK;
	uint32_t status;
	uint32_t timeout=SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
  	if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT; 
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return errorstatus;
	}	 
	if(status&1<<0)						//CRC����
	{								   
		errorstatus=SD_CMD_CRC_FAIL;
		SDIO->ICR|=1<<0;				//�����Ӧ��־
 	}
	SDIO->ICR=0X5FF;	 				//������
 	return errorstatus;	
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
SD_Error bsp_sdio_CmdR3Error(void)
{
		uint32_t status;						 
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
 	if(status&(1<<2))					//��Ӧ��ʱ
	{											 
		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	 
   	SDIO->ICR=0X5FF;	 				//������
 	return SD_OK;		
}

SD_Error bsp_sdio_CmdR4Error(void)
{
	uint32_t status = 0;
	
	while(1)
	{
		status = SDIO->STA;
		if(status & ((1 << 0) | (1 << 2) | (1 << 6))) break;
	}
	if(status & (1 << 2))
	{
		SDIO->ICR |= 1 << 2;
		return SD_CMD_RSP_TIMEOUT;
	}
	SDIO->ICR=0X5FF;	 				//������
	return SD_OK;
	
//	while(SDIO->STA & SDIO_STA_CTIMEOUT)
//	{
//		SDIO->ICR = SDIO_ICR_CTIMEOUTC;
//		SDIO->CMD = SDIO->CMD;
//	//	printf("Timeout! Resend CMD%d\n", SDIO->CMD & SDIO_CMD_CMDINDEX);
//		while(SDIO->STA & SDIO_STA_CMDACT);
//	}
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
SD_Error bsp_sdio_CmdR6Error(uint8_t cmd, uint16_t *prca)
{
	SD_Error errorstatus=SD_OK;
	uint32_t status;					    
	uint32_t rspr1;
 	while(1)
	{
		status=SDIO->STA;
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
	if(status&(1<<2))					//��Ӧ��ʱ
	{																				    
 		SDIO->ICR|=1<<2;				//���������Ӧ��ʱ��־
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	if(status&1<<0)						//CRC����
	{								   
		SDIO->ICR|=1<<0;				//�����Ӧ��־
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)				//�ж��Ƿ���Ӧcmd����
	{
 		return SD_ILLEGAL_CMD; 		
	}	    
	SDIO->ICR=0X5FF;	 				//������б��
	rspr1=SDIO->RESP1;					//�õ���Ӧ 	 
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(uint16_t)(rspr1>>16);			//����16λ�õ�,rca
		return errorstatus;
	}
   	if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)return SD_GENERAL_UNKNOWN_ERROR;
   	if(rspr1&SD_R6_ILLEGAL_CMD)return SD_ILLEGAL_CMD;
   	if(rspr1&SD_R6_COM_CRC_FAILED)return SD_COM_CRC_FAILED;
	return errorstatus;
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
SD_Error bsp_sdio_CmdR7Error(void)
{
	SD_Error sd_err = SD_OK;
	uint32_t status;
	uint32_t timeout = SDIO_CMD0TIMEOUT;
 	while(timeout--)
	{
		status = SDIO->STA;
		if(status&((1 << 0) | (1 << 2) | (1 << 6)))
			break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)	
	}
 	if((timeout == 0) || (status & (1 << 2)))	//��Ӧ��ʱ
	{																				    
		sd_err = SD_CMD_RSP_TIMEOUT;	//��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ
		SDIO->ICR |= 1 << 2;				//���������Ӧ��ʱ��־
		return sd_err;
	}	 
	if(status & (1 << 6))						//�ɹ����յ���Ӧ
	{								   
		sd_err = SD_OK;
		SDIO->ICR |= 1 << 6;				//�����Ӧ��־
 	}
	return sd_err;
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
SD_Error bsp_sd_EnWideBus(uint8_t en)
{
	SD_Error errorstatus = SD_OK;
 	uint32_t scr[2]={0,0};
	uint8_t arg=0X00;
	
	arg = (en) ? 0X02 : 0x00;
	
 	if(SDIO->RESP1&SD_CARD_LOCKED)
		return SD_LOCK_UNLOCK_FAILED;//SD������LOCKED״̬		    
	
 	errorstatus = bsp_sd_FindSCR(RCA,scr);						//�õ�SCR�Ĵ�������
	
 	if(errorstatus!=SD_OK)
		return errorstatus;
	
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//֧�ֿ�����
	{
	 	bsp_sdio_SendCmd(SD_CMD_APP_CMD,1,(uint32_t)RCA<<16);	//����CMD55+RCA,����Ӧ											  
	 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
	 	if(errorstatus!=SD_OK)return errorstatus; 
	 	bsp_sdio_SendCmd(SD_CMD_APP_SD_SET_BUSWIDTH,1,arg);//����ACMD6,����Ӧ,����:10,4λ;00,1λ.											  
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				//��֧�ֿ��������� 
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
SD_Error bsp_sd_IsBusy(uint8_t *status)
{
	__IO uint32_t respR1 = 0, statu = 0; 
	bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,(uint32_t)RCA<<16);		//����CMD13 	   
	statu=SDIO->STA;
	while(!(statu&((1<<0)|(1<<6)|(1<<2))))statu=SDIO->STA;//�ȴ��������
   	if(statu&(1<<0))			//CRC���ʧ��
	{
		SDIO->ICR|=1<<0;		//���������
		return SD_CMD_CRC_FAIL;
	}
   	if(statu&(1<<2))			//���ʱ 
	{
		SDIO->ICR|=1<<2;		//���������
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO->ICR=0X5FF;	 		//������б��
	respR1=SDIO->RESP1;
	*status=(uint8_t)((respR1>>9)&0x0000000F);
	return SD_OK;
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
SD_Error bsp_sd_FindSCR(uint16_t rca, uint32_t *pscr)
{
	uint32_t index = 0; 
	SD_Error errorstatus = SD_OK;
	uint32_t tempscr[2]={0,0};  
	
 	bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,8);			//����CMD16,����Ӧ,����Block SizeΪ8�ֽ�											  
 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);
 	if(errorstatus!=SD_OK)return errorstatus;	    
	
	bsp_sdio_SendCmd(SD_CMD_APP_CMD,1,(uint32_t)rca<<16);	//����CMD55,����Ӧ 									  
 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	
	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,8,3,1);		//8���ֽڳ���,blockΪ8�ֽ�,SD����SDIO.
	bsp_sdio_SendCmd(SD_CMD_SD_APP_SEND_SCR,1,0);		//����ACMD51,����Ӧ,����Ϊ0											  
 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;							 
  
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{ 
		if(SDIO->STA&(1<<21))//����FIFO���ݿ���
		{
			*(tempscr+index)=SDIO->FIFO;	//��ȡFIFO����
			index++;
			if(index>=2)break;
		}
	}
 	if(SDIO->STA&(1<<3))		//�������ݳ�ʱ
	{										 
 		SDIO->ICR|=1<<3;		//������
		return SD_DATA_TIMEOUT;
	}
	else if(SDIO->STA&(1<<1))	//�ѷ���/���յ����ݿ�CRCУ�����
	{
 		SDIO->ICR|=1<<1;		//������
		return SD_DATA_CRC_FAIL;   
	}
	else if(SDIO->STA&(1<<5))	//����FIFO���
	{
 		SDIO->ICR|=1<<5;		//������
		return SD_RX_OVERRUN;   	   
	}
	else if(SDIO->STA&(1<<9))	//��ʼλ������
	{
 		SDIO->ICR|=1<<9;		//������
		return SD_START_BIT_ERR;    
	}
   	SDIO->ICR=0X5FF;	 		//������	 
	//������˳��8λΪ��λ������.   	
	*(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
	*(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
 	return errorstatus;
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
uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
	uint8_t count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
}



/*
*********************************************************************************************************
*                                          bsp_sd_ReadDisk
*
* Description: SD�������ݺ���
*             
* Arguments  : 
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
__align(4) uint8_t SDIO_DATA_BUFFER[512];	
uint8_t bsp_sd_ReadDisk(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	SD_Error sta = SD_OK;
	long long lsector = sector; /*  long long�Ϳ���֧�ִ���2G�Ŀ�  */
	uint8_t n;
	if(CardType != SDIO_STD_CAPACITY_SD_CARD_V1_1) 
		lsector<<=9;
	
	if((uint32_t)buff%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
		 	sta=bsp_sd_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector�Ķ�����
			memcpy(buff,SDIO_DATA_BUFFER,512);
			buff+=512;
		} 
	}else
	{
		if(cnt==1)sta=bsp_sd_ReadBlock(buff,lsector,512);    	//����sector�Ķ�����
		else sta=bsp_sd_ReadBlocks(buff,lsector,512,cnt);//���sector  
	}
	return (sta == SD_OK) ? 0 : (uint8_t)sta;
}

/*
*********************************************************************************************************
*                                         bsp_sd_WriteDisk 
*
* Description: дSD��
*             
* Arguments  : 1> buff:���ݻ�����
*							 2> sector:����
*              3> cnt:����
*
* Reutrn     : 0:����д��ɹ�
*							 ����:ʧ��
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t bsp_sd_WriteDisk(uint8_t *buff, uint32_t sector, uint32_t cnt)
{
	SD_Error sta=SD_OK;
	uint8_t n;
	long long lsector=sector;
	if(CardType!=SDIO_STD_CAPACITY_SD_CARD_V1_1)lsector<<=9;
	if((uint32_t)buff%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
			memcpy(SDIO_DATA_BUFFER,buff,512);
		 	sta=bsp_sd_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector��д����
			buff+=512;
		} 
	}else
	{
		if(cnt==1)sta=bsp_sd_WriteBlock(buff,lsector,512);    	//����sector��д����
		else sta=bsp_sd_WriteBlocks(buff,lsector,512,cnt);	//���sector  
	}
	
	return (sta == SD_OK) ? 0 : (uint8_t)sta;
}

/*
*********************************************************************************************************
*                                    bsp_sdio_SetClk      
*
* Description: SDIOʱ�ӳ�ʼ������
*             
* Arguments  : 1> clkdiv:ʱ�ӷ�Ƶϵ��
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_sdio_SetClk(uint8_t clkdiv)
{
	uint32_t tmpreg = 0;
	
	tmpreg = SDIO->CLKCR;  /*  λ31:15����,���뱣�ָ�λֵ  */
	
	tmpreg &= 0XFFFFFF00;  /*  �����8λ,ʱ�ӷ�Ƶϵ��λ  */
	tmpreg |= (clkdiv & 0XFF) << 0; /*  λ7:0ʱ�ӷ�Ƶϵ��  */
	
	SDIO->CLKCR = tmpreg;  /*  д��Ĵ���  */
}

/*
*********************************************************************************************************
*                                       bsp_sdio_SendCmd   
*
* Description: SDIO���������
*             
* Arguments  : 1> index:��������,��6λ��Ч
*              2> waitrsp:�ڴ�����Ӧ��00/10,����Ӧ, 01:����Ӧ, 11:����Ӧ
*              3> arg:�������
*
* Reutrn     : ��
*
* Note(s)    : ��
*********************************************************************************************************
*/
void bsp_sdio_SendCmd(uint8_t index, uint8_t waitrsp, uint32_t arg)
{
	uint32_t tmpreg = 0;
	
	SDIO->ARG = arg;  /*  ������дCMD�Ĵ���֮ǰARG�Ĵ���  */
	
	tmpreg = SDIO->CMD;  /*  SDIO->CMD�Ĵ�����λ31:15Ϊ����λ,���ɸı䣬�����Ҫ�ȶ��üĴ���  */	
	tmpreg &= 0XFFFFF800; /*  �üĴ���ֻ�е�15λ��Ч��������  */
	tmpreg |= (index & 0x3f); /*  λ5:0,��������  */
	tmpreg |= ((waitrsp & 0x03) << 6); /*  λ6:7,�ȴ���Ӧλ  */
	tmpreg |= (0 << 8); /*  ����Ҫ�ȴ�  */
	tmpreg |= (1 << 10);	/*  ʹ������·��״̬��  */
	
	SDIO->CMD = tmpreg; /*  д������  */
}


/*
*********************************************************************************************************
*                                       bsp_sdio_SendData_Cfg   
*
* Description: SDIO���Ϳ��ƺ��������÷��͵ĳ�ʱʱ�䡢���ݿ��С�����ݳ��ȵ�
*             
* Arguments  : 1> timeout:���ͳ�ʱʱ��
*              2> len:�������ݳ���
*              3> blksize:���ݿ��С
*						   4> dir:���䷽��0:������������1:����������
*
* Reutrn     : �޷���ֵ
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_sdio_SendData_Cfg(uint32_t timeout, uint32_t len, uint8_t blksize, uint8_t dir)
{
	uint32_t tmpreg;
	
	SDIO->DTIMER = timeout;
	SDIO->DLEN = len & 0X1FFFFFF;  /*  ��25λ��Ч  */
	
	tmpreg = SDIO->DCTRL; /*  ��������λ  */
	tmpreg &= 0XFFFFFF08; /*  ���֮ǰ������  */
	tmpreg |= blksize << 4; 
	tmpreg |= 0 << 2;
	tmpreg |= (dir & 0x01) << 1;
	tmpreg |= 1 << 0;
	SDIO->DCTRL = tmpreg;
}


/*
*********************************************************************************************************
*                                          bsp_sd_DMAConfig
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
void bsp_sd_DMAConfig(uint32_t *buff, uint32_t bufsize, uint8_t dir)
{
	uint32_t tmpreg=0;//��������
	while(DMA2_Stream3->CR&0X01);	//�ȴ�DMA������ 
	DMA2->LIFCR|=0X3D<<22;			//���֮ǰ��stream3�ϵ������жϱ�־
	
	
	DMA2_Stream3->PAR=(uint32_t)&SDIO->FIFO;	//DMA2 �����ַ
	DMA2_Stream3->M0AR=(uint32_t)buff; 	//DMA2,�洢��0��ַ;	 
	DMA2_Stream3->NDTR=0; 			//DMA2,����������0,���������� 
	tmpreg|=dir<<6;		//���ݴ��䷽�����
	tmpreg|=0<<8;		//��ѭ��ģʽ(��ʹ����ͨģʽ)
	tmpreg|=0<<9;		//���������ģʽ
	tmpreg|=1<<10;		//�洢������ģʽ
	tmpreg|=2<<11;		//�������ݳ���:32λ
	tmpreg|=2<<13;		//�洢�����ݳ���:32λ
	tmpreg|=3<<16;		//������ȼ�
	tmpreg|=1<<21;		//����ͻ��4�δ���
	tmpreg|=1<<23;		//�洢��ͻ��4�δ���
	tmpreg|=(uint32_t)4<<25;	//ͨ��ѡ��
	DMA2_Stream3->CR=tmpreg; 
	
	tmpreg=DMA2_Stream3->FCR;
	tmpreg&=0XFFFFFFF8;	//���DMDIS��FTH
	tmpreg|=1<<2;		//FIFOʹ��
	tmpreg|=3<<0;		//ȫFIFO
	DMA2_Stream3->FCR=tmpreg;
	DMA2_Stream3->CR|=1<<5;	//���������� 
	DMA2_Stream3->CR|=1<<0;	//����DMA���� 
}

void SDIO_IRQHandler(void) 
{											
 	bsp_sd_ProcessIRQSrc();//��������SDIO����ж�
}	


