# include "bsp_usart.h"
# include <stdarg.h>

/* 如果使能串口1，则定义串口1的数据 */
# if USART1_EN > 0u
	static Usart_Str Usart1_Info;
	static uint8_t Usart1TxBuff[USART1_TX_BUFF_SIZE];
	static uint8_t Usart1RxBuff[USART1_RX_BUFF_SIZE];
# endif

/* 如果使能串口2，则定义串口2的数据 */
# if USART2_EN > 0u
	static Usart_Str Usart2_Info;
	static uint8_t Usart2TxBuff[USART2_TX_BUFF_SIZE];
	static uint8_t Usart2RxBuff[USART2_RX_BUFF_SIZE];
# endif
/* 如果使能串口3，则定义串口3的数据 */
# if USART3_EN > 0u
	static Usart_Str Usart3_Info;
	static uint8_t Usart3TxBuff[USART3_TX_BUFF_SIZE];
	static uint8_t Usart3RxBuff[USART3_RX_BUFF_SIZE];
# endif
/* 如果使能串口4，则定义串口4的数据 */
# if USART4_EN > 0u
	static Usart_Str Usart4_Info;
	static uint8_t Usart4TxBuff[USART4_TX_BUFF_SIZE];
	static uint8_t Usart4RxBuff[USART4_RX_BUFF_SIZE];
# endif
/* 如果使能串口6，则定义串口5的数据 */
# if USART5_EN > 0u
	static Usart_Str Usart5_Info;
	static uint8_t Usart5TxBuff[USART5_TX_BUFF_SIZE];
	static uint8_t Usart5RxBuff[USART5_RX_BUFF_SIZE];
# endif
/* 如果使能串口6，则定义串口6的数据 */
# if USART6_EN > 0u
	static Usart_Str Usart6_Info;
	static uint8_t Usart6TxBuff[USART6_TX_BUFF_SIZE];
	static uint8_t Usart6RxBuff[USART6_RX_BUFF_SIZE];
# endif



UART_HandleTypeDef USART3_Handler;

void bsp_usart_Config(void)
{

  USART3_Handler.Instance = USART3;
  USART3_Handler.Init.BaudRate = 115200;
  USART3_Handler.Init.WordLength = UART_WORDLENGTH_8B;
  USART3_Handler.Init.StopBits = UART_STOPBITS_1;
  USART3_Handler.Init.Parity = UART_PARITY_NONE;
  USART3_Handler.Init.Mode = UART_MODE_TX_RX;
  USART3_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  USART3_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&USART3_Handler);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART3)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();
		__HAL_RCC_GPIOD_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_8|GPIO_PIN_9);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}



USART_TypeDef * bsp_usart_GetPort(COM_PORT_ENUM Port)
{	
	if(Port == COM1)
	{
		# if USART1_EN > 0u
			return USART1;
		# else
			return 0;
		# endif
	}
	else 	if(Port == COM2)
	{
		# if USART2_EN > 0u
			return USART2;
		# else
			return 0;
		# endif
	}
	else 	if(Port == 3)
	{
		# if USART3_EN > 0u
			return USART3;
		# else
			return 0;
		# endif
	}
	else 	if(Port == COM4)
	{
		# if USART4_EN > 0u
			return USART4;
		# else
			return 0;
		# endif
	}
	else 	if(Port == COM5)
	{
		# if USART5_EN > 0u
			return USART5;
		# else
			return 0;
		# endif
	}
	else 	if(Port == COM6)
	{
		# if USART6_EN > 0u
			return USART6;
		# else
			return 0;
		# endif
	}
	
	return 0;
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
Usart_Str *bsp_usart_GetStr(COM_PORT_ENUM Port)
{
# if USART1_EN > 0u
	if(Port == COM1) 	return &Usart1_Info;
	else 	
# endif
# if USART2_EN > 0u
	if(Port == COM2) return &Usart2_Info;
	else 	
# endif
# if USART3_EN > 0u
	if(Port == COM3) return &Usart3_Info; 
	else 	
# endif
# if USART4_EN > 0u
	if(Port == COM4) return &Usart4_Info; 
	else 	
# endif
# if USART5_EN > 0u
	if(Port == COM5) return &Usart5_Info; 
	else 	
# endif
# if USART6_EN > 0u
	if(Port == COM6) return &Usart6_Info;
# endif
	return 0;
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
void bsp_usart_SendDataToBuff(COM_PORT_ENUM Port, uint8_t *pBuff, uint16_t Lenght)
{
	Usart_Str * SendUsart;
	uint16_t i = 0;
	SendUsart = bsp_usart_GetStr(Port);
	if(SendUsart == 0) return ;
	
	for (i = 0; i < Lenght; i++)
	{
			/* 如果发送缓存区不为空的话，等待发送缓存区空闲 */
		while(1)
		{
			__IO uint16_t Count;
			
			DISABLE_INT();
			Count = SendUsart->TxCount;
			ENABLE_INT();
			
			if(Count < SendUsart->TxBuffSize) break;
		}
		
		SendUsart->pTxBuff[SendUsart->TxWrite] = pBuff[i];
		
		DISABLE_INT();
		if(++ SendUsart->TxWrite >= SendUsart->TxBuffSize)
		{
			SendUsart->TxWrite = 0;
		}
		SendUsart->TxCount ++;
		ENABLE_INT();
	}
  SendUsart->uart->CR1 |= (1 << 7);
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
uint8_t bsp_usart_GetChar(COM_PORT_ENUM Port, uint8_t *pByte)
{
	Usart_Str * pUsart;
	
	uint16_t Count;
	pUsart = bsp_usart_GetStr(Port);
	
	DISABLE_INT();
	Count = pUsart->RxCount;
	ENABLE_INT();
	
	if(Count == 0)	return 0;
	else
	{
		*pByte = pUsart->pRxBuff[pUsart->RxRead];
		//printf("%c",*pByte);
		DISABLE_INT();
		if(++ pUsart->RxRead >= pUsart->RxBuffSize)
			pUsart->RxRead = 0;
		
		pUsart->RxCount -- ;
		ENABLE_INT();
		return 1;
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
void bsp_usart_ClearTxBuff(COM_PORT_ENUM Port)
{
	Usart_Str *pUsart;
	pUsart = bsp_usart_GetStr(Port);
	
	if(pUsart == 0) return ;
	pUsart->TxCount = 0;
	pUsart->TxRead = 0;
	pUsart->TxWrite = 0;
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
void bsp_usart_ClearRxBuff(COM_PORT_ENUM Port)
{
	Usart_Str *pUsart;
	pUsart = bsp_usart_GetStr(Port);
	
	if(pUsart == 0) return ;
	pUsart->RxCount = 0;
	pUsart->RxRead = 0;
	pUsart->RxWrite = 0;
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
//void bsp_usart_Config(void)
//{
//	bsp_usart_StructConfig();   /* 必须先初始化相关结构体数据 */
//	bsp_usart_GPIOConfig();   /* 串口引脚初始化 */
//	bsp_usart_NVICConfig();   /* 串口中断初始化 */
//}

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
void bsp_usart_StructConfig(void)
{
# if USART1_EN > 0u
	Usart1_Info.uart = USART1;
	Usart1_Info.pTxBuff = Usart1TxBuff;
	Usart1_Info.pRxBuff = Usart1RxBuff;
	Usart1_Info.RxBuffSize = USART1_RX_BUFF_SIZE;
	Usart1_Info.TxBuffSize = USART1_TX_BUFF_SIZE;
	Usart1_Info.RxWrite = 0;
	Usart1_Info.RxRead = 0;
	Usart1_Info.RxCount  = 0;
	Usart1_Info.TxCount = 0;
	Usart1_Info._cbRecvData = 0;
	Usart1_Info._cbSendBefor = 0;
	Usart1_Info._cbSendOver = 0;
# endif

# if USART2_EN > 0u
	Usart2_Info.uart = USART2;
	Usart2_Info.pTxBuff = Usart2TxBuff;
	Usart2_Info.pRxBuff = Usart2RxBuff;
	Usart2_Info.RxBuffSize = USART2_RX_BUFF_SIZE;
	Usart2_Info.TxBuffSize = USART2_TX_BUFF_SIZE;
	Usart2_Info.RxWrite = 0;
	Usart2_Info.RxRead = 0;
	Usart2_Info.RxCount  = 0;
	Usart2_Info.TxCount = 0;
	Usart2_Info._cbRecvData = 0;
	Usart2_Info._cbSendBefor = 0;
	Usart2_Info._cbSendOver = 0;
# endif
	
# if USART3_EN > 0u
	Usart3_Info.uart = USART3;
	Usart3_Info.pTxBuff = Usart3TxBuff;
	Usart3_Info.pRxBuff = Usart3RxBuff;
	Usart3_Info.RxBuffSize = USART3_RX_BUFF_SIZE;
	Usart3_Info.TxBuffSize = USART3_TX_BUFF_SIZE;
	Usart3_Info.RxWrite = 0;
	Usart3_Info.RxRead = 0;
	Usart3_Info.RxCount  = 0;
	Usart3_Info.TxCount = 0;
	Usart3_Info._cbRecvData = 0;
	Usart3_Info._cbSendBefor = 0;
	Usart3_Info._cbSendOver = 0;
# endif
	
# if USART4_EN > 0u
	Usart4_Info.uart = USART4;
	Usart4_Info.pTxBuff = Usart4TxBuff;
	Usart4_Info.pRxBuff = Usart4RxBuff;
	Usart4_Info.RxBuffSize = USART4_RX_BUFF_SIZE;
	Usart4_Info.TxBuffSize = USART4_TX_BUFF_SIZE;
	Usart4_Info.RxWrite = 0;
	Usart4_Info.RxRead = 0;
	Usart4_Info.RxCount  = 0;
	Usart4_Info.TxCount = 0;
	Usart4_Info._cbRecvData = 0;
	Usart4_Info._cbSendBefor = 0;
	Usart4_Info._cbSendOver = 0;
# endif
	
# if USART5_EN > 0u
	Usart5_Info.uart = USART5;
	Usart5_Info.pTxBuff = Usart5TxBuff;
	Usart5_Info.pRxBuff = Usart5RxBuff;
	Usart5_Info.RxBuffSize = USART5_RX_BUFF_SIZE;
	Usar5_Info.TxBuffSize = USART5_TX_BUFF_SIZE;
	Usart5_Info.RxWrite = 0;
	Usart5_Info.RxRead = 0;
	Usart5_Info.RxCount  = 0;
	Usart5_Info.TxCount = 0;
	Usart5_Info._cbRecvData = 0;
	Usart5_Info._cbSendBefor = 0;
	Usart5_Info._cbSendOver = 0;
# endif
	
# if USART6_EN > 0u
	Usart6_Info.uart = USART6;
	Usart6_Info.pTxBuff = Usart6TxBuff;
	Usart6_Info.pRxBuff = Usart6RxBuff;
	Usart6_Info.RxBuffSize = USART6_RX_BUFF_SIZE;
	Usart6_Info.TxBuffSize = USART6_TX_BUFF_SIZE;
	Usart6_Info.RxWrite = 0;
	Usart6_Info.RxRead = 0;
	Usart6_Info.RxCount  = 0;
	Usart6_Info.TxCount = 0;
	Usart6_Info._cbRecvData = 0;
	Usart6_Info._cbSendBefor = 0;
	Usart6_Info._cbSendOver = 0;
# endif
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
void bsp_usart_GPIOConfig(void)
{
	
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//# if USART1_EN > 0u
//	# if USART1_AF_EN < 1u
//		# ifdef VECTOR_F1			 
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
//			
//			//USART1_TX   GPIOA.9
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
//			 
//			//USART1_RX	  GPIOA.10初始化
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
//		# else
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//			
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//			GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
//			GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
//			
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOA, &GPIO_InitStructure);

//			/* 配置 USART Rx 为复用功能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//			GPIO_Init(GPIOA, &GPIO_InitStructure);
//	  # endif
//	# else
//		# ifdef VECTOR_F1
//		# else
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//			GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
//			GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
//			
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOB, &GPIO_InitStructure);

//			/* 配置 USART Rx 为复用功能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//			GPIO_Init(GPIOB, &GPIO_InitStructure);
//		# endif
//	# endif
//	
//		/* 第2步： 配置串口硬件参数 */
//	USART_InitStructure.USART_BaudRate = USART1_BAUD;	/* 波特率 */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(USART1, &USART_InitStructure);

//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		注意: 不要在此处打开发送中断
//		发送中断使能在SendUart()函数打开
//	*/
//	USART_Cmd(USART1, ENABLE);		/* 使能串口 */

//	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
//		如下语句解决第1个字节无法正确发送出去的问题 */
//	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
//# endif
//	
//# if USART2_EN > 0u
//	# if USART2_AF_EN < 1u
//		# ifdef VECTOR_F1
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//			
//			//USART2_TX   GPIOA.2
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
//			 
//			//USART2_RX	  GPIOA.3初始化
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  
//		# else
//				/* 打开 GPIO 时钟 */
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

//			/* 打开 UART 时钟 */
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//			/* 将 PD5 映射为 USART2_TX */
//			GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);

//			/* 将 PD6 映射为 USART2_RX */
//			GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

//			/* 配置 USART Tx 为复用功能 */
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOD, &GPIO_InitStructure);

//			/* 配置 USART Rx 为复用功能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//			GPIO_Init(GPIOD, &GPIO_InitStructure);
//		# endif
//	# else
//		# ifdef VECTOR_F1
//		# else
//						/* 打开 GPIO 时钟 */
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

//			/* 打开 UART 时钟 */
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//			/* 将 PA2 映射为 USART2_TX. 在STM32-V5板中，PA2 管脚用于以太网 */
//			//GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

//			/* 将 PA3 映射为 USART2_RX */
//			GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

//			/* 配置 USART Tx 为复用功能 */
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//			//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//			//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			//GPIO_Init(GPIOA, &GPIO_InitStructure);

//			/* 配置 USART Rx 为复用功能 */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOA, &GPIO_InitStructure);
//		# endif
//	# endif
//	
//		/* 第2步： 配置串口硬件参数 */
//	USART_InitStructure.USART_BaudRate = USART2_BAUD;	/* 波特率 */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx;		/* 仅选择接收模式 */
//	USART_Init(USART2, &USART_InitStructure);

//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		注意: 不要在此处打开发送中断
//		发送中断使能在SendUart()函数打开
//	*/
//	USART_Cmd(USART2, ENABLE);		/* 使能串口 */

//	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
//		如下语句解决第1个字节无法正确发送出去的问题 */
//	USART_ClearFlag(USART2, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
//# endif
//	
# if USART3_EN > 0u
	# ifdef VECTOR_F1

	# else
//		RCC->AHB1ENR |= (1 << 3);		/*  使能GPIOD  */
//		RCC->APB1ENR |= (1 << 18);	/*  使能串口3  */
//		
//		bsp_gpio_Config(GPIOD, (1<<8) | (1<<9), 2, 0, 2, 1);
//		bsp_gpio_AFSet(GPIOD, 8, 7);
//		bsp_gpio_AFSet(GPIOD, 9, 7);
		
	# endif
		/* 第2步： 配置串口硬件参数 */	
//		temp=(float)(48*1000000)/(USART3_BAUD*16);//得到USARTDIV@OVER8=0
//		mantissa=temp;				 //得到整数部分
//		fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
//		mantissa<<=4;
//		mantissa+=fraction; 
//		
//		USART3->BRR = mantissa;        /*  设置波特率  */
//		USART3->CR1 |= (1 << 2);  /*  CR1寄存器位2:接收器使能位  */
//		USART3->CR1 |= (1 << 3);  /*  CR1寄存器位3:发送器使能位  */
//		USART3->CR1 |= (1 << 5);	/*  CR1寄存器位5:接收中断使能  */
//		USART3->CR1 &= ~(1 << 15);  /*  CR1寄存器位15:过采样模式, 设置为0,16倍过采样  */
//		USART3->CR1 |= (1 << 13);  /*  CR1寄存器位13:USART使能位,开启串口  */
//		
	bsp_usart_Config();
		
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		注意: 不要在此处打开发送中断
		发送中断使能在SendUart()函数打开
	*/

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
		如下语句解决第1个字节无法正确发送出去的问题 */
//	USART3->SR &= ~(1 << 6);     /* 清发送完成标志，Transmission Complete flag */
# endif 
	
//# if USART4_EN > 0u
//	# ifdef VECTOR_F1
//	# else
//			/* 打开 GPIO 时钟 */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//		/* 打开 UART 时钟 */
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

//		/* 将 PC10 映射为 UART4_TX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART1);

//		/* 将 PC11 映射为 UART4_RX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);

//		/* 配置 USART Tx 为复用功能 */
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		/* 配置 USART Rx 为复用功能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
//	# endif
//		/* 第2步： 配置串口硬件参数 */
//	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* 波特率 */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(UART4, &USART_InitStructure);

//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		注意: 不要在此处打开发送中断
//		发送中断使能在SendUart()函数打开
//	*/
//	USART_Cmd(UART4, ENABLE);		/* 使能串口 */

//	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
//		如下语句解决第1个字节无法正确发送出去的问题 */
//	USART_ClearFlag(UART4, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
//# endif

//# if USART5_EN > 0u
//	# ifdef VECTOR_F1
//	# else
//			/* 打开 GPIO 时钟 */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);

//		/* 打开 UART 时钟 */
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

//		/* 将 PC12 映射为 UART5_TX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

//		/* 将 PD2 映射为 UART5_RX */
//		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

//		/* 配置 UART Tx 为复用功能 */
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		/* 配置 UART Rx 为复用功能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//		GPIO_Init(GPIOD, &GPIO_InitStructure);
//	# endif
//		/* 第2步： 配置串口硬件参数 */
//	USART_InitStructure.USART_BaudRate = UART5_BAUD;	/* 波特率 */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(UART5, &USART_InitStructure);

//	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		注意: 不要在此处打开发送中断
//		发送中断使能在SendUart()函数打开
//	*/
//	USART_Cmd(UART5, ENABLE);		/* 使能串口 */

//	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
//		如下语句解决第1个字节无法正确发送出去的问题 */
//	USART_ClearFlag(UART5, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
//# endif
//	
//# if USART6_EN > 0u
//	# ifdef VECTOR_F1
//	# else
//			/* 打开 GPIO 时钟 */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOG, ENABLE);

//		/* 打开 UART 时钟 */
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

//		/* 将 PG14 映射为 USART6_TX */
//		GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

//		/* 将 PC7 映射为 USART6_RX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

//		/* 将 PG8 映射为 USART6_RTS */
//		GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_USART6);

//		/* 将 PG15 映射为 USART6_CTS */
//		GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_USART6);

//		/* 配置 PG14/USART6_TX 为复用功能 */
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* 输出类型为推挽 */
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* 内部上拉电阻使能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* 复用模式 */

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOG, &GPIO_InitStructure);

//		/* 配置 PC7/USART6_RX 为复用功能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		/* 配置 PG8/USART6_RTS 为复用功能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//		GPIO_Init(GPIOG, &GPIO_InitStructure);

//		/* 配置 PG15/USART6_CTS 为复用功能 */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//		GPIO_Init(GPIOG, &GPIO_InitStructure);
//	# endif
//		/* 第2步： 配置串口硬件参数 */
//	USART_InitStructure.USART_BaudRate = UART6_BAUD;	/* 波特率 */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;	/* 选择硬件流控 */
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* 不要硬件流控 */
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(USART6, &USART_InitStructure);

//	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	/* 使能接收中断 */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		注意: 不要在此处打开发送中断
//		发送中断使能在SendUart()函数打开
//	*/
//	USART_Cmd(USART6, ENABLE);		/* 使能串口 */

//	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
//		如下语句解决第1个字节无法正确发送出去的问题 */
//	USART_ClearFlag(USART6, USART_FLAG_TC);     /* 清发送完成标志，Transmission Complete flag */
//# endif
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
void bsp_usart_NVICConfig(void)
{
# if USART1_EN > 0u
		/* 使能串口1中断 */
	bsp_nvic_Config(3, 3, USART1_IRQn, 0);
# endif
	
# if USART2_EN > 0u
		/* 使能串口2中断 */
	bsp_nvic_Config(3, 3, USART2_IRQn, 0);
# endif

	
# if USART3_EN > 0u
	/* 使能串口3中断t */
	bsp_nvic_Config(3, 3, USART3_IRQn, 0);
# endif

	
# if USART4_EN > 0u
	/* 使能串口4中断t */
	bsp_nvic_Config(3, 3, USART4_IRQn, 0);
# endif

	
# if USART5_EN > 0u
	/* 使能串口5中断t */
	bsp_nvic_Config(3, 3, USART5_IRQn, 0);
# endif

	
# if USART6_EN > 0u
	/* 使能串口6中断t */
	bsp_nvic_Config(3, 3, USART6_IRQn, 0);
# endif
}






/*
*********************************************************************************************************
*                                     bsp_usart_Put     
*
* Description: 将数据推送到串口数据接收缓存区
*             
* Arguments  : 1> pUsart: 串口结构体指针
*              2> byte  ：要推送的数据
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_usart_Put(Usart_Str * pUsart, uint8_t byte)
{
	pUsart->pRxBuff[pUsart->RxWrite] = byte;  /*  将数据送入接收缓存区  */
	if(++ pUsart->RxWrite >= pUsart->RxBuffSize)  /*  循环队列  */
		pUsart->RxWrite = 0;
	if(pUsart->RxCount < pUsart->RxBuffSize)	/*  接收数据计数增加  */
		pUsart->RxCount ++;
}
/*
*********************************************************************************************************
*                                   bsp_usart_IRQHandler       
*
* Description: 串口中断执行函数
*             
* Arguments  : 1> pUsart: 串口结构体
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_usart_IRQHandler(Usart_Str * pUsart)
{
	uint8_t RecvByte;
	/* 如果串口接收到数据 */
	if((pUsart->uart->SR >> 5) & 0x01)  /*  SR寄存器的位5:读取数据寄存器不为空  */
	{
		/* 从串口缓存区读出数据 */
		RecvByte = pUsart->uart->DR;
		bsp_usart_Put(pUsart, RecvByte);
	}
	
	/*  发送寄存器为空的时候可以把发送缓存区的数据送到发送寄存器DR  */
	if((pUsart->uart->SR >> 7) & 0x01)  /*  SR寄存器的位7:发送数据寄存器为空  */
	{
		if(pUsart->TxCount == 0)  /*  没有数据需要发送  */
		{
			pUsart->uart->CR1 &= ~(1 << 7); /*  CR1寄存器位7:TXE中断使能位, 禁止中断  */
			pUsart->uart->CR1 |= (1 << 6);   /*  CR1寄存器位6:发送完成中断位,开启发送中断  */
		}
		else		/*  将数据发送出去  */
		{
			pUsart->uart->DR = pUsart->pTxBuff[pUsart->TxRead];
			if(++ pUsart->TxRead >= pUsart->TxBuffSize)
			{
				pUsart->TxRead = 0;
			}
			pUsart->TxCount --;
		}
	}
	else if((pUsart->uart->SR >> 6) & 0x01) /*  SR寄存器位6:发送完成,发送完成中断发生  */
	{
		if(pUsart->TxCount == 0)
		{
			pUsart->uart->CR1 &= ~(1 << 6);   /*  CR1寄存器位6:发送完成中断位,关闭发送中断  */
			if(pUsart->_cbSendOver)		/*  如果配置了发送完成回调函数则执行  */
				pUsart->_cbSendOver(0);
		}
		else
		{
			pUsart->uart->DR = pUsart->pTxBuff[pUsart->TxRead];
			if(++ pUsart->TxRead >= pUsart->TxBuffSize)
				pUsart->TxRead = 0;
			pUsart->TxCount --;
		}
	}
	
}


# if USART1_EN > 0u
void USART1_IRQHandler(void)
{
	bsp_usart_IRQHandler(&Usart1_Info);
	
}
# endif

# if USART2_EN > 0u
void USART2_IRQHandler(void)
{
	bsp_usart_IRQHandler(&Usart2_Info);
	
}
# endif

# if USART3_EN > 0u
void USART3_IRQHandler(void)
{
	bsp_usart_IRQHandler(&Usart3_Info);
	
}
# endif

# if USART4_EN > 0u
void USART4_IRQHandler(void)
{
	bsp_usart_IRQHandler(&Usart4_Info);
}
# endif

# if USART5_EN > 0u
void USART5_IRQHandler(void)
{
	bsp_usart_IRQHandler(&Usart5_Info);
}
# endif

# if USART6_EN > 0u
void USART6_IRQHandler(void)
{
	bsp_usart_IRQHandler(&Usart6_Info);
	
}
# endif


uint16_t bsp_usart_Printf(COM_PORT_ENUM Port, const char *format, ...)
{
	uint16_t i = 0;
	char buf[256];
	
	va_list arg;
	va_start(arg ,format);
	
	i = vsprintf(buf, format, arg);
	
	bsp_usart_SendDataToBuff(Port, (uint8_t *)buf, i);
	
	va_end(arg);
	return i;
}

# if OS_SUPPORT > 0u
/*
*********************************************************************************************************
*                             os_printf             
*
* Description: 如果有操作系统的话添加线程安全打印函数
*             
* Arguments  : 1> format: 格式化参数
*              2> 其他: 要打印的变量
*
* Reutrn     : None.
*
* Note(s)    : 该函数只有在有操作系统的时候才可以使用
*********************************************************************************************************
*/
OS_SEM PrintSem;
void os_printf(char *format, ...)
{
	OS_ERR err;
	va_list   v_args;
	CPU_CHAR  buf_str[128 + 1];
	
	va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);
				   
	OSSemPend((OS_SEM *)&PrintSem,
			(OS_TICK )0u,
			(OS_OPT  )OS_OPT_PEND_BLOCKING,
			(CPU_TS *)0,
			(OS_ERR *)&err);
	
	printf("%s", buf_str);
				   
	(void)OSSemPost((OS_SEM  *)&PrintSem,
                   (OS_OPT   )OS_OPT_POST_1,
                   (OS_ERR  *)&err);
}
# endif

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
	return 0;
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//循环发送,直到发送完毕   
	USART3->DR = (uint8_t) ch;      
	return ch;
}

int _ttywrch(int ch)
{
	ch = ch;
	return 0;
}
#endif



