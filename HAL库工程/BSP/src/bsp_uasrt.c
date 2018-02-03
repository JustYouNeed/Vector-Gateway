# include "bsp_usart.h"
# include <stdarg.h>

/* ���ʹ�ܴ���1�����崮��1������ */
# if USART1_EN > 0u
	static Usart_Str Usart1_Info;
	static uint8_t Usart1TxBuff[USART1_TX_BUFF_SIZE];
	static uint8_t Usart1RxBuff[USART1_RX_BUFF_SIZE];
# endif

/* ���ʹ�ܴ���2�����崮��2������ */
# if USART2_EN > 0u
	static Usart_Str Usart2_Info;
	static uint8_t Usart2TxBuff[USART2_TX_BUFF_SIZE];
	static uint8_t Usart2RxBuff[USART2_RX_BUFF_SIZE];
# endif
/* ���ʹ�ܴ���3�����崮��3������ */
# if USART3_EN > 0u
	static Usart_Str Usart3_Info;
	static uint8_t Usart3TxBuff[USART3_TX_BUFF_SIZE];
	static uint8_t Usart3RxBuff[USART3_RX_BUFF_SIZE];
# endif
/* ���ʹ�ܴ���4�����崮��4������ */
# if USART4_EN > 0u
	static Usart_Str Usart4_Info;
	static uint8_t Usart4TxBuff[USART4_TX_BUFF_SIZE];
	static uint8_t Usart4RxBuff[USART4_RX_BUFF_SIZE];
# endif
/* ���ʹ�ܴ���6�����崮��5������ */
# if USART5_EN > 0u
	static Usart_Str Usart5_Info;
	static uint8_t Usart5TxBuff[USART5_TX_BUFF_SIZE];
	static uint8_t Usart5RxBuff[USART5_RX_BUFF_SIZE];
# endif
/* ���ʹ�ܴ���6�����崮��6������ */
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
			/* ������ͻ�������Ϊ�յĻ����ȴ����ͻ��������� */
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
//	bsp_usart_StructConfig();   /* �����ȳ�ʼ����ؽṹ������ */
//	bsp_usart_GPIOConfig();   /* �������ų�ʼ�� */
//	bsp_usart_NVICConfig();   /* �����жϳ�ʼ�� */
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
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
//			
//			//USART1_TX   GPIOA.9
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
//			 
//			//USART1_RX	  GPIOA.10��ʼ��
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
//		# else
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
//			
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
//			GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,  GPIO_AF_USART1);
//			GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
//			
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOA, &GPIO_InitStructure);

//			/* ���� USART Rx Ϊ���ù��� */
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
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOB, &GPIO_InitStructure);

//			/* ���� USART Rx Ϊ���ù��� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//			GPIO_Init(GPIOB, &GPIO_InitStructure);
//		# endif
//	# endif
//	
//		/* ��2���� ���ô���Ӳ������ */
//	USART_InitStructure.USART_BaudRate = USART1_BAUD;	/* ������ */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(USART1, &USART_InitStructure);

//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		ע��: ��Ҫ�ڴ˴��򿪷����ж�
//		�����ж�ʹ����SendUart()������
//	*/
//	USART_Cmd(USART1, ENABLE);		/* ʹ�ܴ��� */

//	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
//		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
//# endif
//	
//# if USART2_EN > 0u
//	# if USART2_AF_EN < 1u
//		# ifdef VECTOR_F1
//			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//			
//			//USART2_TX   GPIOA.2
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
//			 
//			//USART2_RX	  GPIOA.3��ʼ��
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
//			GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  
//		# else
//				/* �� GPIO ʱ�� */
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

//			/* �� UART ʱ�� */
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//			/* �� PD5 ӳ��Ϊ USART2_TX */
//			GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);

//			/* �� PD6 ӳ��Ϊ USART2_RX */
//			GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);

//			/* ���� USART Tx Ϊ���ù��� */
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOD, &GPIO_InitStructure);

//			/* ���� USART Rx Ϊ���ù��� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//			GPIO_Init(GPIOD, &GPIO_InitStructure);
//		# endif
//	# else
//		# ifdef VECTOR_F1
//		# else
//						/* �� GPIO ʱ�� */
//			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

//			/* �� UART ʱ�� */
//			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//			/* �� PA2 ӳ��Ϊ USART2_TX. ��STM32-V5���У�PA2 �ܽ�������̫�� */
//			//GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

//			/* �� PA3 ӳ��Ϊ USART2_RX */
//			GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

//			/* ���� USART Tx Ϊ���ù��� */
//			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//			//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//			//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			//GPIO_Init(GPIOA, &GPIO_InitStructure);

//			/* ���� USART Rx Ϊ���ù��� */
//			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//			GPIO_Init(GPIOA, &GPIO_InitStructure);
//		# endif
//	# endif
//	
//		/* ��2���� ���ô���Ӳ������ */
//	USART_InitStructure.USART_BaudRate = USART2_BAUD;	/* ������ */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx;		/* ��ѡ�����ģʽ */
//	USART_Init(USART2, &USART_InitStructure);

//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		ע��: ��Ҫ�ڴ˴��򿪷����ж�
//		�����ж�ʹ����SendUart()������
//	*/
//	USART_Cmd(USART2, ENABLE);		/* ʹ�ܴ��� */

//	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
//		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
//# endif
//	
# if USART3_EN > 0u
	# ifdef VECTOR_F1

	# else
//		RCC->AHB1ENR |= (1 << 3);		/*  ʹ��GPIOD  */
//		RCC->APB1ENR |= (1 << 18);	/*  ʹ�ܴ���3  */
//		
//		bsp_gpio_Config(GPIOD, (1<<8) | (1<<9), 2, 0, 2, 1);
//		bsp_gpio_AFSet(GPIOD, 8, 7);
//		bsp_gpio_AFSet(GPIOD, 9, 7);
		
	# endif
		/* ��2���� ���ô���Ӳ������ */	
//		temp=(float)(48*1000000)/(USART3_BAUD*16);//�õ�USARTDIV@OVER8=0
//		mantissa=temp;				 //�õ���������
//		fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
//		mantissa<<=4;
//		mantissa+=fraction; 
//		
//		USART3->BRR = mantissa;        /*  ���ò�����  */
//		USART3->CR1 |= (1 << 2);  /*  CR1�Ĵ���λ2:������ʹ��λ  */
//		USART3->CR1 |= (1 << 3);  /*  CR1�Ĵ���λ3:������ʹ��λ  */
//		USART3->CR1 |= (1 << 5);	/*  CR1�Ĵ���λ5:�����ж�ʹ��  */
//		USART3->CR1 &= ~(1 << 15);  /*  CR1�Ĵ���λ15:������ģʽ, ����Ϊ0,16��������  */
//		USART3->CR1 |= (1 << 13);  /*  CR1�Ĵ���λ13:USARTʹ��λ,��������  */
//		
	bsp_usart_Config();
		
	/*
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		ע��: ��Ҫ�ڴ˴��򿪷����ж�
		�����ж�ʹ����SendUart()������
	*/

	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//	USART3->SR &= ~(1 << 6);     /* �巢����ɱ�־��Transmission Complete flag */
# endif 
	
//# if USART4_EN > 0u
//	# ifdef VECTOR_F1
//	# else
//			/* �� GPIO ʱ�� */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

//		/* �� UART ʱ�� */
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

//		/* �� PC10 ӳ��Ϊ UART4_TX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART1);

//		/* �� PC11 ӳ��Ϊ UART4_RX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART1);

//		/* ���� USART Tx Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		/* ���� USART Rx Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);
//	# endif
//		/* ��2���� ���ô���Ӳ������ */
//	USART_InitStructure.USART_BaudRate = UART1_BAUD;	/* ������ */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(UART4, &USART_InitStructure);

//	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		ע��: ��Ҫ�ڴ˴��򿪷����ж�
//		�����ж�ʹ����SendUart()������
//	*/
//	USART_Cmd(UART4, ENABLE);		/* ʹ�ܴ��� */

//	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
//		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//	USART_ClearFlag(UART4, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
//# endif

//# if USART5_EN > 0u
//	# ifdef VECTOR_F1
//	# else
//			/* �� GPIO ʱ�� */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOD, ENABLE);

//		/* �� UART ʱ�� */
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);

//		/* �� PC12 ӳ��Ϊ UART5_TX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5);

//		/* �� PD2 ӳ��Ϊ UART5_RX */
//		GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5);

//		/* ���� UART Tx Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		/* ���� UART Rx Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//		GPIO_Init(GPIOD, &GPIO_InitStructure);
//	# endif
//		/* ��2���� ���ô���Ӳ������ */
//	USART_InitStructure.USART_BaudRate = UART5_BAUD;	/* ������ */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(UART5, &USART_InitStructure);

//	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		ע��: ��Ҫ�ڴ˴��򿪷����ж�
//		�����ж�ʹ����SendUart()������
//	*/
//	USART_Cmd(UART5, ENABLE);		/* ʹ�ܴ��� */

//	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
//		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//	USART_ClearFlag(UART5, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
//# endif
//	
//# if USART6_EN > 0u
//	# ifdef VECTOR_F1
//	# else
//			/* �� GPIO ʱ�� */
//		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC |RCC_AHB1Periph_GPIOG, ENABLE);

//		/* �� UART ʱ�� */
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);

//		/* �� PG14 ӳ��Ϊ USART6_TX */
//		GPIO_PinAFConfig(GPIOG, GPIO_PinSource14, GPIO_AF_USART6);

//		/* �� PC7 ӳ��Ϊ USART6_RX */
//		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6);

//		/* �� PG8 ӳ��Ϊ USART6_RTS */
//		GPIO_PinAFConfig(GPIOG, GPIO_PinSource8, GPIO_AF_USART6);

//		/* �� PG15 ӳ��Ϊ USART6_CTS */
//		GPIO_PinAFConfig(GPIOG, GPIO_PinSource15, GPIO_AF_USART6);

//		/* ���� PG14/USART6_TX Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	/* �������Ϊ���� */
//		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	/* �ڲ���������ʹ�� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	/* ����ģʽ */

//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
//		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//		GPIO_Init(GPIOG, &GPIO_InitStructure);

//		/* ���� PC7/USART6_RX Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//		GPIO_Init(GPIOC, &GPIO_InitStructure);

//		/* ���� PG8/USART6_RTS Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//		GPIO_Init(GPIOG, &GPIO_InitStructure);

//		/* ���� PG15/USART6_CTS Ϊ���ù��� */
//		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//		GPIO_Init(GPIOG, &GPIO_InitStructure);
//	# endif
//		/* ��2���� ���ô���Ӳ������ */
//	USART_InitStructure.USART_BaudRate = UART6_BAUD;	/* ������ */
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No ;
//	//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;	/* ѡ��Ӳ������ */
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	/* ��ҪӲ������ */
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//	USART_Init(USART6, &USART_InitStructure);

//	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);	/* ʹ�ܽ����ж� */
//	/*
//		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
//		ע��: ��Ҫ�ڴ˴��򿪷����ж�
//		�����ж�ʹ����SendUart()������
//	*/
//	USART_Cmd(USART6, ENABLE);		/* ʹ�ܴ��� */

//	/* CPU��Сȱ�ݣ��������úã����ֱ��Send�����1���ֽڷ��Ͳ���ȥ
//		�����������1���ֽ��޷���ȷ���ͳ�ȥ������ */
//	USART_ClearFlag(USART6, USART_FLAG_TC);     /* �巢����ɱ�־��Transmission Complete flag */
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
		/* ʹ�ܴ���1�ж� */
	bsp_nvic_Config(3, 3, USART1_IRQn, 0);
# endif
	
# if USART2_EN > 0u
		/* ʹ�ܴ���2�ж� */
	bsp_nvic_Config(3, 3, USART2_IRQn, 0);
# endif

	
# if USART3_EN > 0u
	/* ʹ�ܴ���3�ж�t */
	bsp_nvic_Config(3, 3, USART3_IRQn, 0);
# endif

	
# if USART4_EN > 0u
	/* ʹ�ܴ���4�ж�t */
	bsp_nvic_Config(3, 3, USART4_IRQn, 0);
# endif

	
# if USART5_EN > 0u
	/* ʹ�ܴ���5�ж�t */
	bsp_nvic_Config(3, 3, USART5_IRQn, 0);
# endif

	
# if USART6_EN > 0u
	/* ʹ�ܴ���6�ж�t */
	bsp_nvic_Config(3, 3, USART6_IRQn, 0);
# endif
}






/*
*********************************************************************************************************
*                                     bsp_usart_Put     
*
* Description: ���������͵��������ݽ��ջ�����
*             
* Arguments  : 1> pUsart: ���ڽṹ��ָ��
*              2> byte  ��Ҫ���͵�����
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_usart_Put(Usart_Str * pUsart, uint8_t byte)
{
	pUsart->pRxBuff[pUsart->RxWrite] = byte;  /*  ������������ջ�����  */
	if(++ pUsart->RxWrite >= pUsart->RxBuffSize)  /*  ѭ������  */
		pUsart->RxWrite = 0;
	if(pUsart->RxCount < pUsart->RxBuffSize)	/*  �������ݼ�������  */
		pUsart->RxCount ++;
}
/*
*********************************************************************************************************
*                                   bsp_usart_IRQHandler       
*
* Description: �����ж�ִ�к���
*             
* Arguments  : 1> pUsart: ���ڽṹ��
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_usart_IRQHandler(Usart_Str * pUsart)
{
	uint8_t RecvByte;
	/* ������ڽ��յ����� */
	if((pUsart->uart->SR >> 5) & 0x01)  /*  SR�Ĵ�����λ5:��ȡ���ݼĴ�����Ϊ��  */
	{
		/* �Ӵ��ڻ������������� */
		RecvByte = pUsart->uart->DR;
		bsp_usart_Put(pUsart, RecvByte);
	}
	
	/*  ���ͼĴ���Ϊ�յ�ʱ����԰ѷ��ͻ������������͵����ͼĴ���DR  */
	if((pUsart->uart->SR >> 7) & 0x01)  /*  SR�Ĵ�����λ7:�������ݼĴ���Ϊ��  */
	{
		if(pUsart->TxCount == 0)  /*  û��������Ҫ����  */
		{
			pUsart->uart->CR1 &= ~(1 << 7); /*  CR1�Ĵ���λ7:TXE�ж�ʹ��λ, ��ֹ�ж�  */
			pUsart->uart->CR1 |= (1 << 6);   /*  CR1�Ĵ���λ6:��������ж�λ,���������ж�  */
		}
		else		/*  �����ݷ��ͳ�ȥ  */
		{
			pUsart->uart->DR = pUsart->pTxBuff[pUsart->TxRead];
			if(++ pUsart->TxRead >= pUsart->TxBuffSize)
			{
				pUsart->TxRead = 0;
			}
			pUsart->TxCount --;
		}
	}
	else if((pUsart->uart->SR >> 6) & 0x01) /*  SR�Ĵ���λ6:�������,��������жϷ���  */
	{
		if(pUsart->TxCount == 0)
		{
			pUsart->uart->CR1 &= ~(1 << 6);   /*  CR1�Ĵ���λ6:��������ж�λ,�رշ����ж�  */
			if(pUsart->_cbSendOver)		/*  ��������˷�����ɻص�������ִ��  */
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
* Description: ����в���ϵͳ�Ļ�����̰߳�ȫ��ӡ����
*             
* Arguments  : 1> format: ��ʽ������
*              2> ����: Ҫ��ӡ�ı���
*
* Reutrn     : None.
*
* Note(s)    : �ú���ֻ�����в���ϵͳ��ʱ��ſ���ʹ��
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
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
int _sys_exit(int x) 
{ 
	x = x; 
	return 0;
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART3->SR&0X40)==0);//ѭ������,ֱ���������   
	USART3->DR = (uint8_t) ch;      
	return ch;
}

int _ttywrch(int ch)
{
	ch = ch;
	return 0;
}
#endif



