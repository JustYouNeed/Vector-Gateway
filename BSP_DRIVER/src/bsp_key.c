# include "bsp_key.h"


# ifdef	VECTOR_F1
	static uint8_t IsKeyUpPress(void) { return ((KEY_UP_PORT->IDR & KEY_UP_PIN) == 0)?1:0;}
	static uint8_t IsKeyOkPress(void) { return ((KEY_OK_PORT->IDR & KEY_OK_PIN) == 0)?1:0;}
	static uint8_t IsKeyDownPress(void) { return ((KEY_DOWN_PORT->IDR & KEY_DOWN_PIN) == 0)?1:0;}
# else
	static uint8_t IsKeyUpPress(void) { return ((KEY_UP_PORT->IDR & KEY_UP_PIN) == 0)?1:0;}
	static uint8_t IsKeyOkPress(void) { return ((KEY_OK_PORT->IDR & KEY_OK_PIN) == 0)?1:0;}
	static uint8_t IsKeyDownPress(void) { return ((KEY_DOWN_PORT->IDR & KEY_DOWN_PIN) == 0)?1:0;}
# endif


static Key_Fifo_Str		 bsp_key_fifo;   /* the key fifo struct */
static Key_Str		     bsp_key[BSP_KEY_COUNT];  /* struct of each key */

	
	
	
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
void bsp_key_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*If you are using the F1 series of chips*/
# ifdef	VECTOR_F1
	RCC_APB2PeriphClockCmd(RCC_KEY_ALL, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*If you are using the F4 series of chips*/
# elif defined	VECTOR_F4
	/*Enable the clock*/
	RCC_AHB1PeriphClockCmd(RCC_KEY_ALL, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
# endif
	GPIO_InitStructure.GPIO_Pin = KEY_UP_PIN;
	GPIO_Init(KEY_UP_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = KEY_OK_PIN;
	GPIO_Init(KEY_OK_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = KEY_DOWN_PIN;
	GPIO_Init(KEY_DOWN_PORT, &GPIO_InitStructure);	
}


/*
*********************************************************************************************************
*                                       bsp_key_FIFOConfig   
*
* Description: Initialize the key FIFO
*             
* Arguments : None.
*
* Note(s)   : The function is called by bsp_key_Config and can not be called by the user individually
*********************************************************************************************************
*/
void bsp_key_FIFOConfig(void)
{
	uint8_t i = 0;
	
	bsp_key_fifo.Read = 0;  /* Clear the read fifo pointer */
	bsp_key_fifo.Write = 0; /* Clear the write fifo pointer */
	
	bsp_key_fifo.IsConfig = 1;  /* button has been initialized */
	/* Configure a default value for each key structure */
	for(i = 0; i < BSP_KEY_COUNT; i++)
	{
		bsp_key[i].LongTime = KEY_LONG_TIME;
		bsp_key[i].Count = KEY_FILTER_TIME / 2;
		bsp_key[i].State = KEY_NONE;
		bsp_key[i].RepeatCount = 0;
		bsp_key[i].RepeatSpeed = 0;
	}
	
	bsp_key[KEY_ID_UP].IsKeyPressDunc = IsKeyUpPress;
	bsp_key[KEY_ID_OK].IsKeyPressDunc = IsKeyOkPress;
	bsp_key[KEY_ID_DOWN].IsKeyPressDunc = IsKeyDownPress;
}


/*
*********************************************************************************************************
*                                        bsp_key_Config  
*
* Description: Configure the key
*             
* Arguments : None.
*
* Note(s)   : None. 
*********************************************************************************************************
*/
void bsp_key_Config(void)
{
	bsp_key_GPIOConfig();/*  Configure the GPIO port for the key  */
	bsp_key_FIFOConfig();/*  Initialize the key FIFO   */
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
uint8_t bsp_key_GetValue(void)
{
	uint8_t key;
	if(!bsp_key_fifo.IsConfig)
	{
		while(1);
	}
	
	if(bsp_key_fifo.Read == bsp_key_fifo.Write) 
	{
	//	LED0 = ~LED0;
		return KEY_NONE;
	}
	else
	{
		key = bsp_key_fifo.Fifo[bsp_key_fifo.Read];
		if( ++bsp_key_fifo.Read >= KEY_FIFO_SIZE) bsp_key_fifo.Read = 0;
		return key;
	}
}

/*
*********************************************************************************************************
*                                bsp_key_Put          
*
* Description: put一个按键值到按键FIFO里
*             
* Arguments : KeyValue:按键值
*
* Note(s)   : KeyValue的值应该小于等于9，因为默认按键只有3个，每个按键三种状态
*********************************************************************************************************
*/
void bsp_key_Put(uint8_t KeyValue)
{
	if(KeyValue > 9) return ;
	
	bsp_key_fifo.Fifo[bsp_key_fifo.Write] = KeyValue;  /*  将按键值写入FIFO  */
	
	if(++bsp_key_fifo.Write >= KEY_FIFO_SIZE)   /*  如果FIFO写满了，则从头开始  */
		bsp_key_fifo.Write = 0;
}


/*
*********************************************************************************************************
*                                          bsp_key_GetState
*
* Description: 获取按键的状态
*             
* Arguments : KeyId:按键ID
*
* Note(s)   : KeyId在bsp_key.h中定义
*********************************************************************************************************
*/
uint8_t bsp_key_GetState(KEY_ID_ENUM KeyId)
{
	return bsp_key[KeyId].State;
}

/*
*********************************************************************************************************
*                                      bsp_key_Clear    
*
* Description: 清空按键FIFO
*             
* Arguments : 无
*
* Note(s)   : 无
*********************************************************************************************************
*/
void bsp_key_Clear(void)
{
	bsp_key_fifo.Read = bsp_key_fifo.Write;
}

/*
*********************************************************************************************************
*                               bsp_key_Detect           
*
* Description: 检测按键状态，
*             
* Arguments : Id:想检测的按键ID
*
* Note(s)   : 该函数不应该在本文件之外的地方调用
*********************************************************************************************************
*/
void bsp_key_Detect(uint8_t Id)
{
	Key_Str *pKey;   /*  先声明一个按键结构体  */
	
	pKey = &bsp_key[Id];		/*  获取相应ID的按键结构体  */
	
	if(pKey->IsKeyPressDunc())   /*  如果按键状态有改变  */
	{
		
		if(pKey->Count < KEY_FILTER_TIME) 	/*  软件消抖  */
			pKey->Count  = KEY_FILTER_TIME;
		else if(pKey->Count < 2 * KEY_FILTER_TIME) 
			pKey->Count ++;
		else
		{
			if(pKey->State == 0)
			{
				
				pKey->State = 1;
				bsp_key_Put((uint8_t)(3 * Id + 1));
			}
			
			/*  If this variable is greater than 0, 
					it means that long press detection is on
			*/
			if(pKey->LongTime > 0)
			{
				/*  Long press detection time has arrived  */
				if(++pKey->LongCount == pKey->LongTime)
				{
					/* Put the button value into fifo of long press */
					bsp_key_Put((uint8_t)(3 * Id + 3));
				}
			}
			else  /*  it means that long press detection is off */
			{
				if(pKey->RepeatSpeed > 0)
				{
					if( ++ pKey->RepeatCount >= pKey->RepeatSpeed)
					{
						pKey->RepeatCount = 0;
						bsp_key_Put((uint8_t)(3 * Id + 1));
					}
				}
			}
		}
	}
	else
	{
		if(pKey->Count > KEY_FILTER_TIME)
			pKey->Count  = KEY_FILTER_TIME;
		else if(pKey->Count != 0)
			pKey->Count -- ;
		else
		{
			if(pKey->State == 1)
			{
				pKey->State = 0;
				bsp_key_Put((uint8_t)(3 * Id + 2));
			}
		}
		
		pKey->RepeatCount = 0;
		pKey->LongCount = 0;
	}
}

/*
*********************************************************************************************************
*                                bsp_key_Scan         
*
* Description: 按键状态扫描函数，扫描按键状态
*             
* Arguments : 无
*
* Note(s)   : 该函数不应该在本文件外的地方调用
*********************************************************************************************************
*/
void bsp_key_Scan(void)
{
	uint8_t i = 0;
	for(i = 0; i < BSP_KEY_COUNT; i++)
	{
		bsp_key_Detect(i);
	}
}










