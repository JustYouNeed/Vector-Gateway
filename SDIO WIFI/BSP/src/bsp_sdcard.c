# include "bsp_sdcard.h"

static uint8_t CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;
static uint32_t CSD_Tab[4],CID_Tab[4],RCA=0;					//SD卡CSD,CID以及相对地址(RCA)数据
static uint8_t DeviceMode=SD_DMA_MODE;	
static uint8_t StopCondition=0; 								//是否发送停止传输标志位,DMA多块读写的时候用到  
volatile SD_Error TransferError=SD_OK;					//数据传输错误标志,DMA读写时使用	    
volatile uint8_t TransferEnd=0;	
SD_CardInfo SDCardInfo;

void bsp_sd_GPIOConfig(void)
{	
	RCC->AHB1ENR |= 1<<2;  /* 使能PORTC时钟 */
	RCC->AHB1ENR |= 1<<3;  /* 使能PORTD时钟 */
	RCC->AHB1ENR |= 1<<22;  /* 使能DMA2时钟 */
	RCC->APB2ENR |= 1<<11;  /* 使能SDIO时钟 */
	RCC->APB2RSTR |= 1<<11; /* 复位SDIO */
	
	
	/* 设置GPIOC.8为复用推挽输出，并复用为SDIO功能 */
	GPIOC->MODER &= ~(3 << (8*2));  /* 复位GPIOC.8,SDIO_D0 */
	GPIOC->MODER |= 2 << (8*2);
	GPIOC->OTYPER &= ~(1 << 8);
	GPIOC->OTYPER |= 0 << 8;
	GPIOC->PUPDR &= ~(3 << (8*2));
	GPIOC->PUPDR |= 1 << (8*2);
	GPIOC->OSPEEDR &= ~(3 << (8*2));
	GPIOC->OSPEEDR |= 2 << (8*2);
	GPIOC->AFR[1] &= ~(0x0f << 0);
	GPIOC->AFR[1] |= (uint32_t)12 << 0;
	
	/* 设置GPIOC.9为复用推挽输出，并复用为SDIO功能 */
	GPIOC->MODER &= ~(3 << (9*2));  /* 复位GPIOC.9,SDIO_D1 */
	GPIOC->MODER |= 2 << (9*2);
	GPIOC->OTYPER &= ~(1 << 9);
	GPIOC->OTYPER |= 0 << 9;
	GPIOC->PUPDR &= ~(3 << (9*2));
	GPIOC->PUPDR |= 1 << (9*2);
	GPIOC->OSPEEDR &= ~(3 << (9*2));
	GPIOC->OSPEEDR |= 2 << (9*2);
	GPIOC->AFR[1] &= ~(0x0f << 4);
	GPIOC->AFR[1] |= (uint32_t)12 << 4;
	
	/* 设置GPIOC.10为复用推挽输出，并复用为SDIO功能 */
	GPIOC->MODER &= ~(3 << (10*2));  /* 复位GPIOC.10,SDIO_D2 */
	GPIOC->MODER |= 2 << (10*2);
	GPIOC->OTYPER &= ~(1 << 10);
	GPIOC->OTYPER |= 0 << 10;
	GPIOC->PUPDR &= ~(3 << (10*2));
	GPIOC->PUPDR |= 1 << (10*2);
	GPIOC->OSPEEDR &= ~(3 << (10*2));
	GPIOC->OSPEEDR |= 2 << (10*2);
	GPIOC->AFR[1] &= ~(0x0f << 8);
	GPIOC->AFR[1] |= (uint32_t)12 << 8;
	
	/* 设置GPIOC.11为复用推挽输出，并复用为SDIO功能 */
	GPIOC->MODER &= ~(3 << (11*2));  /* 复位GPIOC.11,SDIO_D3 */
	GPIOC->MODER |= 2 << (11*2);
	GPIOC->OTYPER &= ~(1 << 11);
	GPIOC->OTYPER |= 0 << 11;
	GPIOC->PUPDR &= ~(3 << (11*2));
	GPIOC->PUPDR |= 1 << (11*2);
	GPIOC->OSPEEDR &= ~(3 << (11*2));
	GPIOC->OSPEEDR |= 2 << (11*2);
	GPIOC->AFR[1] &= ~(0x0f << 12);
	GPIOC->AFR[1] |= (uint32_t)12 << 12;
	
	/* 设置GPIOC.12为复用推挽输出，并复用为SDIO功能 */
	GPIOC->MODER &= ~(3 << (12*2));  /* 复位GPIOC.12,SDIO_CLK */
	GPIOC->MODER |= 2 << (12*2);
	GPIOC->OTYPER &= ~(1 << 12);
	GPIOC->OTYPER |= 0 << 12;
	GPIOC->PUPDR &= ~(3 << (12*2));
	GPIOC->PUPDR |= 1 << (12*2);
	GPIOC->OSPEEDR &= ~(3 << (12*2));
	GPIOC->OSPEEDR |= 2 << (12*2);
	GPIOC->AFR[1] &= ~(0x0f << 16);
	GPIOC->AFR[1] |= (uint32_t)12 << 16;
	
	/* 设置GPIOD.2为复用推挽输出，并复用为SDIO功能 */
	GPIOD->MODER &= ~(3 << (2*2));  /* 复位GPIOD.2,SDIO_CMD */
	GPIOD->MODER |= 2 << (2*2);
	GPIOD->OTYPER &= ~(1 << 2);
	GPIOD->OTYPER |= 0 << 2;
	GPIOD->PUPDR &= ~(3 << (2*2));
	GPIOD->PUPDR |= 1 << (2*2);
	GPIOD->OSPEEDR &= ~(3 << (2*2));
	GPIOD->OSPEEDR |= 2 << (2*2);
	GPIOD->AFR[0] &= ~(0x0f << 8);
	GPIOD->AFR[0] |= (uint32_t)12 << 8;
	
	RCC->APB2RSTR &= ~(1 << 11);  /* 结束复位SDIO */
	
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
* Description: 初始化SDIO中断
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
* Description: 初始化SD卡
*             
* Arguments  : None.
*
* Reutrn     : 1> SD_OK:初始化SD卡成功
*		           2> 其他:错误代码
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_Config(void)
{
	SD_Error err = SD_OK;
	uint8_t clkdiv = 0;
	
	bsp_sd_GPIOConfig(); /*  SDIO引脚初始化  */
	bsp_sd_NVICConfig(); /*  NVIC初始化  */
	
	err = bsp_sd_PowerCmd(ENABLE);	/*  卡上电  */
	
	if(err == SD_OK)  /*  上电成功后初始化SD卡  */
		err = bsp_sd_CardInit();
	
	if(err == SD_OK)	/*  初始化后读取卡的信息  */
		err = bsp_sd_GetCardInfo(&SDCardInfo);
	
	if(err == SD_OK)	/*  选中SD卡,设置卡的相对地址  */
		err = bsp_sd_Select((uint32_t)(SDCardInfo.RCA<<16));
	
	if(err == SD_OK)	/*  初始化设置位宽为1  */
		err = bsp_sd_SetWideBus(1);
	
	/*  SD卡正常工作时可以提高卡的工作时钟  */
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
* Description: SD卡上电函数
*             
* Arguments  : cmd: 1> ENABLE:上电
*                   2> DISABLE: 断电
*
* Reutrn     : 1> SD_OK: 上电成功
*              2> 其他: 错误代码
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
		SDIO->CLKCR = 0;   /*  清空之前的设置  */
		SDIO->CLKCR |= 0 << 9;	/*  非省电模式  */
		SDIO->CLKCR |= 0 << 10;	/*  关闭旁路，CK根据分频设置输出  */
		SDIO->CLKCR |= 0 << 11;	/*  1位数据宽度  */
		SDIO->CLKCR |= 0 << 13; /*  SDIOCLK上升沿产生SDIOCK  */
		SDIO->CLKCR |= 0 << 14; /*  关闭硬件流控制  */
		
		bsp_sdio_SetClk(0x76);
		SDIO->POWER = 0x03;
		SDIO->CLKCR |= 1 << 8;
		
		
		/*  首先发送CMD0  */
		for(i = 0; i < 74; i++)
		{
			bsp_sdio_SendCmd(SD_CMD_GO_IDLE_STATE, 0, 0);
			printf("SDIO->STA:%08x\r\n",SDIO->STA);
			sd_err = bsp_sdio_CmdR0Error();
			if(sd_err == SD_OK) 
				break;
		}
		if(sd_err != SD_OK) return sd_err;
		
		/*  发送CMD8,有响应则为V2.0或者更迟的卡,没有则为MMC卡  */
		bsp_sdio_SendCmd(SDIO_SEND_IF_COND, 1, SD_CHECK_PATTERN);
		sd_err = bsp_sdio_CmdR7Error();
		
		if(sd_err == SD_OK)  /*  有响应,说明为V2.0或者更后的卡  */
		{
			CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;
			SDType = SD_HIGH_CAPACITY;
		}
		
		/*  发送CMD55，短响应  */
		bsp_sdio_SendCmd(SD_CMD_APP_CMD, 1, 0);
		sd_err = bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
		if(sd_err == SD_OK)
		{
			while((!validvol) && (cnt < SD_MAX_VOLT_TRIAL))
			{
				/*  发送CMD55  */
				bsp_sdio_SendCmd(SD_CMD_APP_CMD, 1, 0);
				sd_err = bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
				if(sd_err != SD_OK) return sd_err;
				
				/*  发送CMD41,短响应  */
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
		else  /*  MMC卡  */
		{
			/*  MMC卡,发送CMD1,SDIO_SEND_OP_COND,参数:0X80FF800  */
			while((!validvol) && (cnt < SD_MAX_VOLT_TRIAL))
			{
				/*  发送CMD1  */
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
	
	return SD_OK;  /*  正常情况不会运行到这里  */
}

/*
*********************************************************************************************************
*                                     bsp_sd_CardInit     
*
* Description: 初始化SD卡,并让卡进入就绪状态
*             
* Arguments  : None.
*
* Reutrn     : 1> SD_OK
*              2> 其他:错误代码
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_CardInit(void)
{
	SD_Error sd_err = SD_OK;
	uint16_t rca = 0x01;
	
	if((SDIO->POWER & 0x03) == 0)  /*  SD卡未上电  */
		return SD_REQUEST_NOT_APPLICABLE;
	
	/*  不是SD卡  */
	if(SDIO_SECURE_DIGITAL_IO_CARD != CardType)
	{
		/*  发送CMD2,取得CID,长响应  */
		bsp_sdio_SendCmd(SD_CMD_ALL_SEND_CID, 3, 0);
		sd_err = bsp_sdio_CmdR2Error();
		if(sd_err != SD_OK) return sd_err;
		CID_Tab[0] = SDIO->RESP1;
		CID_Tab[1] = SDIO->RESP2;
		CID_Tab[2] = SDIO->RESP3;
		CID_Tab[3] = SDIO->RESP4;
	}/*  end of if(SDIO_SECURE_DIGITAL_IO_CARD != CardType)  */
	
	/*  判断卡的类型  */
	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || 
		 (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) || 
	   (SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType) || 
	   (SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		/*  发送CMD33,短响应  */
		bsp_sdio_SendCmd(SD_CMD_SET_REL_ADDR, 1, 0);
		sd_err = bsp_sdio_CmdR6Error(SD_CMD_SET_REL_ADDR, &rca);
		if(sd_err != SD_OK) return sd_err;
	} /*  end of if 卡类型判断  */
	
	
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
* Description: 获取卡信息函数
*             
* Arguments  : CardInfo:  SD_CardInfo结构体指针
*
* Reutrn     : 1> SD_OK: 成功获取信息
*              2> 其他: 错误代码
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
	CardInfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0协议还没定义这部分(为保留),应该是后续协议定义的
	CardInfo->SD_csd.Reserved1=tmp&0x03;			//2个保留位  
	tmp=(uint8_t)((CSD_Tab[0]&0x00FF0000)>>16);			//第1个字节
	CardInfo->SD_csd.TAAC=tmp;				   		//数据读时间1
	tmp=(uint8_t)((CSD_Tab[0]&0x0000FF00)>>8);	  		//第2个字节
	CardInfo->SD_csd.NSAC=tmp;		  				//数据读时间2
	tmp=(uint8_t)(CSD_Tab[0]&0x000000FF);				//第3个字节
	CardInfo->SD_csd.MaxBusClkFrec=tmp;		  		//传输速度	   
	tmp=(uint8_t)((CSD_Tab[1]&0xFF000000)>>24);			//第4个字节
	CardInfo->SD_csd.CardComdClasses=tmp<<4;    	//卡指令类高四位
	tmp=(uint8_t)((CSD_Tab[1]&0x00FF0000)>>16);	 		//第5个字节
	CardInfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//卡指令类低四位
	CardInfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//最大读取数据长度
	tmp=(uint8_t)((CSD_Tab[1]&0x0000FF00)>>8);			//第6个字节
	CardInfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//允许分块读
	CardInfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//写块错位
	CardInfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//读块错位
	CardInfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
	CardInfo->SD_csd.Reserved2=0; 					//保留
	
	if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1) || 
		 (CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0) || 
	   (SDIO_MULTIMEDIA_CARD==CardType))
	{
		CardInfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12位)
	 	tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 			//第7个字节	
		CardInfo->SD_csd.DeviceSize|=(tmp)<<2;
 		tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24);		//第8个字节	
		CardInfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
 		CardInfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
		CardInfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
 		tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);		//第9个字节	
		CardInfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
		CardInfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
		CardInfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
 		tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8);	  	//第10个字节	
		CardInfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
 		CardInfo->CardCapacity=(CardInfo->SD_csd.DeviceSize+1);//计算卡容量
		CardInfo->CardCapacity*=(1<<(CardInfo->SD_csd.DeviceSizeMul+2));
		CardInfo->CardBlockSize=1<<(CardInfo->SD_csd.RdBlockLen);//块大小
		CardInfo->CardCapacity*=CardInfo->CardBlockSize;
	}/*  end if  */
	else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)
	{
		tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 		//第7个字节	
		CardInfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
 		tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24); 	//第8个字节	
 		CardInfo->SD_csd.DeviceSize|=(tmp<<8);
 		tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);	//第9个字节	
 		CardInfo->SD_csd.DeviceSize|=(tmp);
 		tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8); 	//第10个字节	
 		CardInfo->CardCapacity=(long long)(CardInfo->SD_csd.DeviceSize+1)*512*1024;//计算卡容量
		CardInfo->CardBlockSize=512; 			//块大小固定为512字节
	}/*  end else if  */
	
	CardInfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	CardInfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(uint8_t)(CSD_Tab[2]&0x000000FF);			//第11个字节	
	CardInfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	CardInfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
 	tmp=(uint8_t)((CSD_Tab[3]&0xFF000000)>>24);		//第12个字节	
	CardInfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	CardInfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	CardInfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	CardInfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(uint8_t)((CSD_Tab[3]&0x00FF0000)>>16);		//第13个字节
	CardInfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	CardInfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	CardInfo->SD_csd.Reserved3=0;
	CardInfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(uint8_t)((CSD_Tab[3]&0x0000FF00)>>8);		//第14个字节
	CardInfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	CardInfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	CardInfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	CardInfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	CardInfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	CardInfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(uint8_t)(CSD_Tab[3]&0x000000FF);			//第15个字节
	CardInfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	CardInfo->SD_csd.Reserved4=1;		 
	tmp=(uint8_t)((CID_Tab[0]&0xFF000000)>>24);		//第0个字节
	CardInfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(uint8_t)((CID_Tab[0]&0x00FF0000)>>16);		//第1个字节
	CardInfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(uint8_t)((CID_Tab[0]&0x000000FF00)>>8);		//第2个字节
	CardInfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(uint8_t)(CID_Tab[0]&0x000000FF);			//第3个字节	
	CardInfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(uint8_t)((CID_Tab[1]&0xFF000000)>>24); 		//第4个字节
	CardInfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(uint8_t)((CID_Tab[1]&0x00FF0000)>>16);	   	//第5个字节
	CardInfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(uint8_t)((CID_Tab[1]&0x0000FF00)>>8);		//第6个字节
	CardInfo->SD_cid.ProdName1|=tmp;		   
	tmp=(uint8_t)(CID_Tab[1]&0x000000FF);	  		//第7个字节
	CardInfo->SD_cid.ProdName2=tmp;			  
	tmp=(uint8_t)((CID_Tab[2]&0xFF000000)>>24); 		//第8个字节
	CardInfo->SD_cid.ProdRev=tmp;		 
	tmp=(uint8_t)((CID_Tab[2]&0x00FF0000)>>16);		//第9个字节
	CardInfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(uint8_t)((CID_Tab[2]&0x0000FF00)>>8); 		//第10个字节
	CardInfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(uint8_t)(CID_Tab[2]&0x000000FF);   			//第11个字节
	CardInfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(uint8_t)((CID_Tab[3]&0xFF000000)>>24); 		//第12个字节
	CardInfo->SD_cid.ProdSN|=tmp;			     
	tmp=(uint8_t)((CID_Tab[3]&0x00FF0000)>>16);	 	//第13个字节
	CardInfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	CardInfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(uint8_t)((CID_Tab[3]&0x0000FF00)>>8);		//第14个字节
	CardInfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(uint8_t)(CID_Tab[3]&0x000000FF);			//第15个字节
	CardInfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	CardInfo->SD_cid.Reserved2=1;	 
	
	return sd_err;
}

/*
*********************************************************************************************************
*                                          bsp_sd_SetWideBus
*
* Description: 设置SD卡数据位宽
*             
* Arguments  : mode = 1 一位数据位宽, mode = 2 4位数据位宽
*
* Reutrn     : 1> SD_OK: 设置成功
*              2> 其他: 错误代码
*
* Note(s)    : None.
*********************************************************************************************************
*/
SD_Error bsp_sd_SetWideBus(uint32_t mode)
{
	SD_Error sd_err = SD_OK;
	
	/*  MMC卡不支持  */
	if(SDIO_MULTIMEDIA_CARD==CardType)
	{
		return SD_UNSUPPORTED_FEATURE;
	}
	else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType) || 
		      (SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType) || 
	        (SDIO_HIGH_CAPACITY_SD_CARD==CardType))
	{
		/*  不支持8位模式  */
		if(mode >= 2) 
		{
			return SD_UNSUPPORTED_FEATURE;
		} 
		else
		{
			sd_err = bsp_sd_EnWideBus(mode);
			if(sd_err == SD_OK)
			{
				SDIO->CLKCR&=~(3<<11);		//清除之前的位宽设置    
				SDIO->CLKCR|=(uint16_t)mode<<11;//1位/4位总线宽度 
				SDIO->CLKCR|=0<<14;			//不开启硬件流控制
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
	bsp_sdio_SendCmd(SD_CMD_SEL_DESEL_CARD,1,addr);	//发送CMD7,选择卡,短响应	 	   
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
 	bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,RCA<<16);	//发送CMD13,短响应		 
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SEND_STATUS);	//查询响应状态 
	if(errorstatus!=SD_OK)return errorstatus;
	*CardStatus=SDIO->RESP1;//读取响应值
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
	uint32_t count=0,*tempbuff=(uint32_t*)buff;//转换为uint32_t指针 
	uint32_t timeout=SDIO_DATATIMEOUT;   
	
	if(NULL==buff)  /*  接收参数错误  */
		return SD_INVALID_PARAMETER; 

	SDIO->DCTRL=0x0;	//数据控制寄存器清零(关DMA)   
	
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
	{
		blksize=512;
		addr>>=9;
	}   
	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)
		return SD_LOCK_UNLOCK_FAILED;//卡锁了
	
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    	   
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus != SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	  
	
	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,blksize,power,1);	//blksize,卡到控制器	  
	bsp_sdio_SendCmd(SD_CMD_READ_SINGLE_BLOCK,1,addr);		//发送CMD17+从addr地址出读取数据,短响应 	   
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_READ_SINGLE_BLOCK);//等待R1响应   
	
	if(errorstatus!=SD_OK)return errorstatus;   		//响应错误	 
	
 	if(DeviceMode==SD_POLLING_MODE)						//查询模式,轮询数据	 
	{
 		DISABLE_INT();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//无上溢/CRC/超时/完成(标志)/起始位错误
		{
			if(SDIO->STA&(1<<15))						//接收区半满,表示至少存了8个字
			{
				for(count=0;count<8;count++)			//循环读取数据
				{
					*(tempbuff+count)=SDIO->FIFO;
				}
				tempbuff+=8;	 
				timeout=0X7FFFFF; 	//读数据溢出时间
			}else 	//处理超时
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//数据超时错误
		{										   
	 		SDIO->ICR|=1<<3; 		//清错误标志
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
		{
	 		SDIO->ICR|=1<<1; 		//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<5)) 	//接收fifo上溢错误
		{
	 		SDIO->ICR|=1<<5; 		//清错误标志
			return SD_RX_OVERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
		{
	 		SDIO->ICR|=1<<9; 		//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		while(SDIO->STA&(1<<21))	//FIFO里面,还存在可用数据
		{
			*tempbuff=SDIO->FIFO;	//循环读取数据
			tempbuff++;
		}
		ENABLE_INT();//开启总中断
		SDIO->ICR=0X5FF;	 		//清除所有标记
	}else if(DeviceMode==SD_DMA_MODE)
	{
 		TransferError=SD_OK;
		StopCondition=0;			//单块读,不需要发送停止传输指令
		TransferEnd=0;				//传输结束标置位，在中断服务置1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//配置需要的中断 
	 	SDIO->DCTRL|=1<<3;		 	//SDIO DMA使能 
		bsp_sd_DMAConfig((uint32_t*)buff,blksize,0); 
 		while(((DMA2->LISR&(1<<27))==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//等待传输完成 
		if(timeout==0)return SD_DATA_TIMEOUT;//超时
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
	tempbuff=(uint32_t*)buff;//转换为uint32_t指针
	
	SDIO->DCTRL=0x0;		//数据控制寄存器清零(关DMA)   
	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
	{
		blksize=512;
		addr>>=9;
	}  
  bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	  
	if(nblks>1)											//多块读  
	{									    
 	  	if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;//判断是否超过最大接收长度
		bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,nblks*blksize,power,1);//nblks*blksize,512块大小,卡到控制器	  
	  	bsp_sdio_SendCmd(SD_CMD_READ_MULT_BLOCK,1,addr);	//发送CMD18+从addr地址出读取数据,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_READ_MULT_BLOCK);//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	  
 		if(DeviceMode==SD_POLLING_MODE)
		{
			DISABLE_INT();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//无上溢/CRC/超时/完成(标志)/起始位错误
			{
				if(SDIO->STA&(1<<15))						//接收区半满,表示至少存了8个字
				{
					for(count=0;count<8;count++)			//循环读取数据
					{
						*(tempbuff+count)=SDIO->FIFO;
					}
					tempbuff+=8;	 
					timeout=0X7FFFFF; 	//读数据溢出时间
				}else 	//处理超时
				{
					if(timeout==0)return SD_DATA_TIMEOUT;
					timeout--;
				}
			}  
			if(SDIO->STA&(1<<3))		//数据超时错误
			{										   
		 		SDIO->ICR|=1<<3; 		//清错误标志
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
			{
		 		SDIO->ICR|=1<<1; 		//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<5)) 	//接收fifo上溢错误
			{
		 		SDIO->ICR|=1<<5; 		//清错误标志
				return SD_RX_OVERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
			{
		 		SDIO->ICR|=1<<9; 		//清错误标志
				return SD_START_BIT_ERR;		 
			}   
			while(SDIO->STA&(1<<21))	//FIFO里面,还存在可用数据
			{
				*tempbuff=SDIO->FIFO;	//循环读取数据
				tempbuff++;
			}
	 		if(SDIO->STA&(1<<8))		//接收结束
			{
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					bsp_sdio_SendCmd(SD_CMD_STOP_TRANSMISSION,1,0);		//发送CMD12+结束传输 	   
					errorstatus=bsp_sdio_CmdR1Error(SD_CMD_STOP_TRANSMISSION);//等待R1响应   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
 			}
			ENABLE_INT();//开启总中断
	 		SDIO->ICR=0X5FF;	 		//清除所有标记 
 		}else if(DeviceMode==SD_DMA_MODE)
		{
	   		TransferError=SD_OK;
			StopCondition=1;			//多块读,需要发送停止传输指令 
			TransferEnd=0;				//传输结束标置位，在中断服务置1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//配置需要的中断 
		 	SDIO->DCTRL|=1<<3;		 						//SDIO DMA使能 
			bsp_sd_DMAConfig((uint32_t*)buff,nblks*blksize,0); 
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//等待传输完成 
			if(timeout==0)return SD_DATA_TIMEOUT;//超时
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
	uint32_t	tlen=blksize;						//总长度(字节)
	uint32_t*tempbuff=(uint32_t*)buff;		
	
 	if(buff==NULL)return SD_INVALID_PARAMETER;//参数错误   
  	SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)   
  	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//大容量卡
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	 
  bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,(uint32_t)RCA<<16);	//发送CMD13,查询卡的状态,短响应 	   
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SEND_STATUS);		//等待R1响应   		   
	if(errorstatus!=SD_OK)return errorstatus;
	cardstatus=SDIO->RESP1;													  
	timeout=SD_DATATIMEOUT;
   	while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//检查READY_FOR_DATA位是否置位
	{
		timeout--;
		bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,(uint32_t)RCA<<16);//发送CMD13,查询卡的状态,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SEND_STATUS);	//等待R1响应   		   
		if(errorstatus!=SD_OK)return errorstatus;				    
		cardstatus=SDIO->RESP1;													  
	}
	if(timeout==0)return SD_ERROR;
   	bsp_sdio_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK,1,addr);	//发送CMD24,写单块指令,短响应 	   
	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_WRITE_SINGLE_BLOCK);//等待R1响应   		   
	if(errorstatus!=SD_OK)return errorstatus;   	  
	StopCondition=0;									//单块写,不需要发送停止传输指令 
 	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,blksize,power,0);	//blksize, 控制器到卡	  
	timeout=SDIO_DATATIMEOUT;
	if (DeviceMode == SD_POLLING_MODE)
	{
		DISABLE_INT();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
		while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//数据块发送成功/下溢/CRC/超时/起始位错误
		{
			if(SDIO->STA&(1<<14))							//发送区半空,表示至少存了8个字
			{
				if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//不够32字节了
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
				timeout=0X3FFFFFFF;	//写数据溢出时间
			}else
			{
				if(timeout==0)return SD_DATA_TIMEOUT;
				timeout--;
			}
		} 
		if(SDIO->STA&(1<<3))		//数据超时错误
		{										   
	 		SDIO->ICR|=1<<3; 		//清错误标志
			return SD_DATA_TIMEOUT;
	 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
		{
	 		SDIO->ICR|=1<<1; 		//清错误标志
			return SD_DATA_CRC_FAIL;		   
		}else if(SDIO->STA&(1<<4)) 	//接收fifo下溢错误
		{
	 		SDIO->ICR|=1<<4; 		//清错误标志
			return SD_TX_UNDERRUN;		 
		}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
		{
	 		SDIO->ICR|=1<<9; 		//清错误标志
			return SD_START_BIT_ERR;		 
		}   
		ENABLE_INT();//开启总中断
		SDIO->ICR=0X5FF;	 		//清除所有标记	  
	}else if(DeviceMode==SD_DMA_MODE)
	{
   		TransferError=SD_OK;
		StopCondition=0;			//单块写,不需要发送停止传输指令 
		TransferEnd=0;				//传输结束标置位，在中断服务置1
		SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//配置产生数据接收完成中断
		bsp_sd_DMAConfig((uint32_t*)buff,blksize,1);				//SDIO DMA配置
 	 	SDIO->DCTRL|=1<<3;								//SDIO DMA使能.  
 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//等待传输完成 
		if(timeout==0)
		{
  			bsp_sd_Config();	 					//重新初始化SD卡,可以解决写入死机的问题
			return SD_DATA_TIMEOUT;			//超时	 
 		}
		timeout=SDIO_DATATIMEOUT;
		while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
 		if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
  		if(TransferError!=SD_OK)return TransferError;
 	}  
 	SDIO->ICR=0X5FF;	 		//清除所有标记
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
	uint32_t tlen=nblks*blksize;				//总长度(字节)
	uint32_t *tempbuff = (uint32_t*)buff;  
  	if(buff==NULL)return SD_INVALID_PARAMETER; //参数错误  
  	SDIO->DCTRL=0x0;							//数据控制寄存器清零(关DMA)   
  	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,0,0,0);	//清除DPSM状态机配置
	if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//卡锁了
 	if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//大容量卡
	{
		blksize=512;
		addr>>=9;
	}    
	if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
	{
		power=convert_from_bytes_to_power_of_two(blksize);	    
		bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,blksize);	//发送CMD16+设置数据长度为blksize,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);	//等待R1响应   
		if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 
	}else return SD_INVALID_PARAMETER;	 
	if(nblks>1)
	{					  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
     	if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    	{
			//提高性能
	 	   	bsp_sdio_SendCmd(SD_CMD_APP_CMD,1,(uint32_t)RCA<<16);	//发送ACMD55,短响应 	   
			errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);		//等待R1响应   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
	 	   	bsp_sdio_SendCmd(SD_CMD_SET_BLOCK_COUNT,1,nblks);	//发送CMD23,设置块数量,短响应 	   
			errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCK_COUNT);//等待R1响应   		   
			if(errorstatus!=SD_OK)return errorstatus;				    
		} 
		bsp_sdio_SendCmd(SD_CMD_WRITE_MULT_BLOCK,1,addr);		//发送CMD25,多块写指令,短响应 	   
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_WRITE_MULT_BLOCK);	//等待R1响应   		   
		if(errorstatus!=SD_OK)return errorstatus;
 	 	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,nblks*blksize,power,0);//blksize, 控制器到卡	
		if(DeviceMode==SD_POLLING_MODE)
	    {
			timeout=SDIO_DATATIMEOUT;
			DISABLE_INT();//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
			while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//下溢/CRC/数据结束/超时/起始位错误
			{
				if(SDIO->STA&(1<<14))							//发送区半空,表示至少存了8字(32字节)
				{	  
					if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//不够32字节了
					{
						restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
						for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
						{
							SDIO->FIFO=*tempbuff;
						}
					}else 										//发送区半空,可以发送至少8字(32字节)数据
					{
						for(count=0;count<SD_HALFFIFO;count++)
						{
							SDIO->FIFO=*(tempbuff+count);
						}
						tempbuff+=SD_HALFFIFO;
						bytestransferred+=SD_HALFFIFOBYTES;
					}
					timeout=0X3FFFFFFF;	//写数据溢出时间
				}else
				{
					if(timeout==0)return SD_DATA_TIMEOUT; 
					timeout--;
				}
			} 
			if(SDIO->STA&(1<<3))		//数据超时错误
			{										   
		 		SDIO->ICR|=1<<3; 		//清错误标志
				return SD_DATA_TIMEOUT;
		 	}else if(SDIO->STA&(1<<1))	//数据块CRC错误
			{
		 		SDIO->ICR|=1<<1; 		//清错误标志
				return SD_DATA_CRC_FAIL;		   
			}else if(SDIO->STA&(1<<4)) 	//接收fifo下溢错误
			{
		 		SDIO->ICR|=1<<4; 		//清错误标志
				return SD_TX_UNDERRUN;		 
			}else if(SDIO->STA&(1<<9)) 	//接收起始位错误
			{
		 		SDIO->ICR|=1<<9; 		//清错误标志
				return SD_START_BIT_ERR;		 
			}   										   
			if(SDIO->STA&(1<<8))		//发送结束
			{															 
				if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
				{
					bsp_sdio_SendCmd(SD_CMD_STOP_TRANSMISSION,1,0);		//发送CMD12+结束传输 	   
					errorstatus=bsp_sdio_CmdR1Error(SD_CMD_STOP_TRANSMISSION);//等待R1响应   
					if(errorstatus!=SD_OK)return errorstatus;	 
				}
			}
			ENABLE_INT();//开启总中断
	 		SDIO->ICR=0X5FF;	 		//清除所有标记 
	    }else if(DeviceMode==SD_DMA_MODE)
		{
	   		TransferError=SD_OK;
			StopCondition=1;			//多块写,需要发送停止传输指令 
			TransferEnd=0;				//传输结束标置位，在中断服务置1
			SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//配置产生数据接收完成中断
			bsp_sd_DMAConfig((uint32_t*)buff,nblks*blksize,1);		//SDIO DMA配置
	 	 	SDIO->DCTRL|=1<<3;								//SDIO DMA使能. 
			timeout=SDIO_DATATIMEOUT;
	 		while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//等待传输完成 
			if(timeout==0)	 								//超时
			{									  
				bsp_sd_Config();	 					//重新初始化SD卡,可以解决写入死机的问题
	 			return SD_DATA_TIMEOUT;			//超时	 
	 		}
			timeout=SDIO_DATATIMEOUT;
			while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
	 		if(timeout==0)return SD_DATA_TIMEOUT;			//超时	 
	 		if(TransferError!=SD_OK)return TransferError;	 
		}
  	}
 	SDIO->ICR=0X5FF;	 		//清除所有标记
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
	if(SDIO->STA & (1 << 8))//接收完成中断
	{	 
		if(StopCondition == 1)
		{
			bsp_sdio_SendCmd(SD_CMD_STOP_TRANSMISSION, 1, 0);		//发送CMD12,结束传输 	   
			TransferError = bsp_sdio_CmdR1Error(SD_CMD_STOP_TRANSMISSION);
		}else TransferError = SD_OK;	
		
 		SDIO->ICR|=1<<8;//清除完成中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
 		TransferEnd = 1;
		
		return(TransferError);
	}/*  end if(SDIO->STA & (1 << 8))  */
	
	
 	if(SDIO->STA&(1<<1))//数据CRC错误
	{
		SDIO->ICR |= 1 << 1;//清除中断标记
		SDIO->MASK &= ~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
		TransferError = SD_DATA_CRC_FAIL;
		return(SD_DATA_CRC_FAIL);
	}/*  end if(SDIO->STA&(1<<1))  */
	
	
 	if(SDIO->STA&(1<<3))//数据超时错误
	{
		SDIO->ICR|=1<<3;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
		TransferError = SD_DATA_TIMEOUT;
		return(SD_DATA_TIMEOUT);
	}
	if(SDIO->STA&(1<<5))//FIFO上溢错误
	{
		SDIO->ICR|=1<<5;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
		TransferError = SD_RX_OVERRUN;
		return(SD_RX_OVERRUN);
	}
	if(SDIO->STA&(1<<4))//FIFO下溢错误
	{
		SDIO->ICR|=1<<4;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
		TransferError = SD_TX_UNDERRUN;
		return(SD_TX_UNDERRUN);
	}
	if(SDIO->STA&(1<<9))//起始位错误
	{
		SDIO->ICR|=1<<9;//清除中断标记
		SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//关闭相关中断
		TransferError = SD_START_BIT_ERR;
		return(SD_START_BIT_ERR);
	}
	return(SD_OK);
}


/*
*********************************************************************************************************
*                                 bsp_sdio_CmdR0Error         
*
* Description: CMD0的响应,对于CMD0来说,无需响应,只有命令已经发送
*             
* Arguments  : None.
*
* Reutrn     : 1> SD_OK:命令执行成功
*							 2> SD_CMD_RSP_TIMEOUT:命令响应超时
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
		if((SDIO->STA & (1 << 7))) /*  位7命令已发送,不需要响应  */
			break;
	}
	
	/*  命令响应超时  */
	if(timeout == 0) return SD_CMD_RSP_TIMEOUT;
	SDIO->ICR = 0X5FF;  /*  清除中断清零寄存器  */
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
			break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)
	} 
	if(status & (1 << 2))					//响应超时
	{																				    
 		SDIO->ICR = 1 << 2;					//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	
 	if(status & (1 << 0))					//CRC错误
	{																				    
 		SDIO->ICR = 1 << 0;					//清除标志
		return SD_CMD_CRC_FAIL;
	}		
	if(SDIO->RESPCMD != cmd)
		return SD_ILLEGAL_CMD;//命令不匹配 
	SDIO->ICR=0X5FF;	 				//清除标记
	return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS);//返回卡响应
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
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
  	if((timeout==0)||(status&(1<<2)))	//响应超时
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT; 
		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return errorstatus;
	}	 
	if(status&1<<0)						//CRC错误
	{								   
		errorstatus=SD_CMD_CRC_FAIL;
		SDIO->ICR|=1<<0;				//清除响应标志
 	}
	SDIO->ICR=0X5FF;	 				//清除标记
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
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
 	if(status&(1<<2))					//响应超时
	{											 
		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 
   	SDIO->ICR=0X5FF;	 				//清除标记
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
	SDIO->ICR=0X5FF;	 				//清除标记
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
		if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
	if(status&(1<<2))					//响应超时
	{																				    
 		SDIO->ICR|=1<<2;				//清除命令响应超时标志
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	if(status&1<<0)						//CRC错误
	{								   
		SDIO->ICR|=1<<0;				//清除响应标志
 		return SD_CMD_CRC_FAIL;
	}
	if(SDIO->RESPCMD!=cmd)				//判断是否响应cmd命令
	{
 		return SD_ILLEGAL_CMD; 		
	}	    
	SDIO->ICR=0X5FF;	 				//清除所有标记
	rspr1=SDIO->RESP1;					//得到响应 	 
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		*prca=(uint16_t)(rspr1>>16);			//右移16位得到,rca
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
			break;//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
	}
 	if((timeout == 0) || (status & (1 << 2)))	//响应超时
	{																				    
		sd_err = SD_CMD_RSP_TIMEOUT;	//当前卡不是2.0兼容卡,或者不支持设定的电压范围
		SDIO->ICR |= 1 << 2;				//清除命令响应超时标志
		return sd_err;
	}	 
	if(status & (1 << 6))						//成功接收到响应
	{								   
		sd_err = SD_OK;
		SDIO->ICR |= 1 << 6;				//清除响应标志
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
		return SD_LOCK_UNLOCK_FAILED;//SD卡处于LOCKED状态		    
	
 	errorstatus = bsp_sd_FindSCR(RCA,scr);						//得到SCR寄存器数据
	
 	if(errorstatus!=SD_OK)
		return errorstatus;
	
	if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//支持宽总线
	{
	 	bsp_sdio_SendCmd(SD_CMD_APP_CMD,1,(uint32_t)RCA<<16);	//发送CMD55+RCA,短响应											  
	 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
	 	if(errorstatus!=SD_OK)return errorstatus; 
	 	bsp_sdio_SendCmd(SD_CMD_APP_SD_SET_BUSWIDTH,1,arg);//发送ACMD6,短响应,参数:10,4位;00,1位.											  
		errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
		return errorstatus;
	}else return SD_REQUEST_NOT_APPLICABLE;				//不支持宽总线设置 
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
	bsp_sdio_SendCmd(SD_CMD_SEND_STATUS,1,(uint32_t)RCA<<16);		//发送CMD13 	   
	statu=SDIO->STA;
	while(!(statu&((1<<0)|(1<<6)|(1<<2))))statu=SDIO->STA;//等待操作完成
   	if(statu&(1<<0))			//CRC检测失败
	{
		SDIO->ICR|=1<<0;		//清除错误标记
		return SD_CMD_CRC_FAIL;
	}
   	if(statu&(1<<2))			//命令超时 
	{
		SDIO->ICR|=1<<2;		//清除错误标记
		return SD_CMD_RSP_TIMEOUT;
	}
 	if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	SDIO->ICR=0X5FF;	 		//清除所有标记
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
	
 	bsp_sdio_SendCmd(SD_CMD_SET_BLOCKLEN,1,8);			//发送CMD16,短响应,设置Block Size为8字节											  
 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SET_BLOCKLEN);
 	if(errorstatus!=SD_OK)return errorstatus;	    
	
	bsp_sdio_SendCmd(SD_CMD_APP_CMD,1,(uint32_t)rca<<16);	//发送CMD55,短响应 									  
 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_APP_CMD);
 	if(errorstatus!=SD_OK)return errorstatus;
	
	bsp_sdio_SendData_Cfg(SD_DATATIMEOUT,8,3,1);		//8个字节长度,block为8字节,SD卡到SDIO.
	bsp_sdio_SendCmd(SD_CMD_SD_APP_SEND_SCR,1,0);		//发送ACMD51,短响应,参数为0											  
 	errorstatus=bsp_sdio_CmdR1Error(SD_CMD_SD_APP_SEND_SCR);
 	if(errorstatus!=SD_OK)return errorstatus;							 
  
 	while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
	{ 
		if(SDIO->STA&(1<<21))//接收FIFO数据可用
		{
			*(tempscr+index)=SDIO->FIFO;	//读取FIFO内容
			index++;
			if(index>=2)break;
		}
	}
 	if(SDIO->STA&(1<<3))		//接收数据超时
	{										 
 		SDIO->ICR|=1<<3;		//清除标记
		return SD_DATA_TIMEOUT;
	}
	else if(SDIO->STA&(1<<1))	//已发送/接收的数据块CRC校验错误
	{
 		SDIO->ICR|=1<<1;		//清除标记
		return SD_DATA_CRC_FAIL;   
	}
	else if(SDIO->STA&(1<<5))	//接收FIFO溢出
	{
 		SDIO->ICR|=1<<5;		//清除标记
		return SD_RX_OVERRUN;   	   
	}
	else if(SDIO->STA&(1<<9))	//起始位检测错误
	{
 		SDIO->ICR|=1<<9;		//清除标记
		return SD_START_BIT_ERR;    
	}
   	SDIO->ICR=0X5FF;	 		//清除标记	 
	//把数据顺序按8位为单位倒过来.   	
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
* Description: SD卡读数据函数
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
	long long lsector = sector; /*  long long型可以支持大于2G的卡  */
	uint8_t n;
	if(CardType != SDIO_STD_CAPACITY_SD_CARD_V1_1) 
		lsector<<=9;
	
	if((uint32_t)buff%4!=0)
	{
	 	for(n=0;n<cnt;n++)
		{
		 	sta=bsp_sd_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//单个sector的读操作
			memcpy(buff,SDIO_DATA_BUFFER,512);
			buff+=512;
		} 
	}else
	{
		if(cnt==1)sta=bsp_sd_ReadBlock(buff,lsector,512);    	//单个sector的读操作
		else sta=bsp_sd_ReadBlocks(buff,lsector,512,cnt);//多个sector  
	}
	return (sta == SD_OK) ? 0 : (uint8_t)sta;
}

/*
*********************************************************************************************************
*                                         bsp_sd_WriteDisk 
*
* Description: 写SD卡
*             
* Arguments  : 1> buff:数据缓存区
*							 2> sector:扇区
*              3> cnt:数量
*
* Reutrn     : 0:数据写入成功
*							 其他:失败
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
		 	sta=bsp_sd_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//单个sector的写操作
			buff+=512;
		} 
	}else
	{
		if(cnt==1)sta=bsp_sd_WriteBlock(buff,lsector,512);    	//单个sector的写操作
		else sta=bsp_sd_WriteBlocks(buff,lsector,512,cnt);	//多个sector  
	}
	
	return (sta == SD_OK) ? 0 : (uint8_t)sta;
}

/*
*********************************************************************************************************
*                                    bsp_sdio_SetClk      
*
* Description: SDIO时钟初始化设置
*             
* Arguments  : 1> clkdiv:时钟分频系数
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_sdio_SetClk(uint8_t clkdiv)
{
	uint32_t tmpreg = 0;
	
	tmpreg = SDIO->CLKCR;  /*  位31:15保留,必须保持复位值  */
	
	tmpreg &= 0XFFFFFF00;  /*  清除低8位,时钟分频系数位  */
	tmpreg |= (clkdiv & 0XFF) << 0; /*  位7:0时钟分频系数  */
	
	SDIO->CLKCR = tmpreg;  /*  写入寄存器  */
}

/*
*********************************************************************************************************
*                                       bsp_sdio_SendCmd   
*
* Description: SDIO发送命令函数
*             
* Arguments  : 1> index:命令索引,低6位有效
*              2> waitrsp:期待的响应，00/10,无响应, 01:短响应, 11:长响应
*              3> arg:命令参数
*
* Reutrn     : 无
*
* Note(s)    : 无
*********************************************************************************************************
*/
void bsp_sdio_SendCmd(uint8_t index, uint8_t waitrsp, uint32_t arg)
{
	uint32_t tmpreg = 0;
	
	SDIO->ARG = arg;  /*  必须在写CMD寄存器之前ARG寄存器  */
	
	tmpreg = SDIO->CMD;  /*  SDIO->CMD寄存器的位31:15为保留位,不可改变，因此需要先读该寄存器  */	
	tmpreg &= 0XFFFFF800; /*  该寄存器只有低15位有效，先清零  */
	tmpreg |= (index & 0x3f); /*  位5:0,命令索引  */
	tmpreg |= ((waitrsp & 0x03) << 6); /*  位6:7,等待响应位  */
	tmpreg |= (0 << 8); /*  不需要等待  */
	tmpreg |= (1 << 10);	/*  使能命令路径状态机  */
	
	SDIO->CMD = tmpreg; /*  写入命令  */
}


/*
*********************************************************************************************************
*                                       bsp_sdio_SendData_Cfg   
*
* Description: SDIO发送控制函数，配置发送的超时时间、数据块大小、数据长度等
*             
* Arguments  : 1> timeout:发送超时时间
*              2> len:发送数据长度
*              3> blksize:数据块大小
*						   4> dir:传输方向。0:控制器到卡，1:卡到控制器
*
* Reutrn     : 无返回值
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_sdio_SendData_Cfg(uint32_t timeout, uint32_t len, uint8_t blksize, uint8_t dir)
{
	uint32_t tmpreg;
	
	SDIO->DTIMER = timeout;
	SDIO->DLEN = len & 0X1FFFFFF;  /*  低25位有效  */
	
	tmpreg = SDIO->DCTRL; /*  保留其他位  */
	tmpreg &= 0XFFFFFF08; /*  清除之前的设置  */
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
	uint32_t tmpreg=0;//重新设置
	while(DMA2_Stream3->CR&0X01);	//等待DMA可配置 
	DMA2->LIFCR|=0X3D<<22;			//清空之前该stream3上的所有中断标志
	
	
	DMA2_Stream3->PAR=(uint32_t)&SDIO->FIFO;	//DMA2 外设地址
	DMA2_Stream3->M0AR=(uint32_t)buff; 	//DMA2,存储器0地址;	 
	DMA2_Stream3->NDTR=0; 			//DMA2,传输数据量0,外设流控制 
	tmpreg|=dir<<6;		//数据传输方向控制
	tmpreg|=0<<8;		//非循环模式(即使用普通模式)
	tmpreg|=0<<9;		//外设非增量模式
	tmpreg|=1<<10;		//存储器增量模式
	tmpreg|=2<<11;		//外设数据长度:32位
	tmpreg|=2<<13;		//存储器数据长度:32位
	tmpreg|=3<<16;		//最高优先级
	tmpreg|=1<<21;		//外设突发4次传输
	tmpreg|=1<<23;		//存储器突发4次传输
	tmpreg|=(uint32_t)4<<25;	//通道选择
	DMA2_Stream3->CR=tmpreg; 
	
	tmpreg=DMA2_Stream3->FCR;
	tmpreg&=0XFFFFFFF8;	//清除DMDIS和FTH
	tmpreg|=1<<2;		//FIFO使能
	tmpreg|=3<<0;		//全FIFO
	DMA2_Stream3->FCR=tmpreg;
	DMA2_Stream3->CR|=1<<5;	//外设流控制 
	DMA2_Stream3->CR|=1<<0;	//开启DMA传输 
}

void SDIO_IRQHandler(void) 
{											
 	bsp_sd_ProcessIRQSrc();//处理所有SDIO相关中断
}	


