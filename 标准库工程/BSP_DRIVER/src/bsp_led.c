# include "bsp_led.h"


/*  This variable indicates whether the LED is initialized   */
static uint8_t IsLedConfig = 0;


/*
*********************************************************************************************************
*                                        bsp_led_GPIOConfig  
*
* Description: Initialize the GPIO of LED
*             
* Arguments : None. 
*
* Note(s)   : None.
*********************************************************************************************************
*/
void bsp_led_GPIOConfig(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

# ifdef VECTOR_F1
//	RCC_APB2PeriphClockCmd(RCC_LED_ALL, ENABLE);
	bsp_GPIOClcokCmd(LED0_GPIO_PORT);
	bsp_GPIOClcokCmd(LED1_GPIO_PORT);
	
	GPIO_InitStructure.GPIO_Pin = LED0_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = LED0_GPIO_MODE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = LED1_GPIO_MODE;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
# else
	//RCC_AHB1PeriphClockCmd(RCC_LED_ALL, ENABLE);
	bsp_GPIOClcokCmd(LED0_GPIO_PORT);
	bsp_GPIOClcokCmd(LED1_GPIO_PORT);
	
  GPIO_InitStructure.GPIO_Pin = LED0_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = LED0_GPIO_MODE;
  GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
  GPIO_InitStructure.GPIO_PuPd = LED1_GPIO_MODE;
  GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_SetBits(LED0_GPIO_PORT, LED0_GPIO_PIN);
	GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
# endif
}



/*
*********************************************************************************************************
*                                          bsp_led_Config
*
* Description: Initialize the LED
*             
* Arguments : None.
*
* Note(s)   : None.
*********************************************************************************************************
*/
void bsp_led_Config(void)
{
	bsp_led_GPIOConfig();
	IsLedConfig = 1;
}




/*
*********************************************************************************************************
*                                        bsp_led_Toggle  
*
* Description:��תLED��״̬
*             
* Arguments : LedId��LED ID,��ֵΪ��ʱ��ת����LED
*
* Note(s)   : None.
*********************************************************************************************************
*/
void bsp_led_Toggle(uint8_t LedId)
{
	if(IsLedConfig == 1)
	{
		switch(LedId)
		{
			case 0:LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN;
						 LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN;break;
			case 1:LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN;break;
			case 2:LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN;break;
			default:break;
		}
	}
}
/*
*********************************************************************************************************
*                                       bsp_led_ON   
*
* Description: ����һ��LED
*             
* Arguments : LedId:LED ID������ֵΪ��ʱ��������LED
*
* Note(s)   : ��
*********************************************************************************************************
*/
void bsp_led_ON(uint8_t LedId)
{
	if(IsLedConfig == 1)
	{
		switch(LedId)
		{
		# ifdef VECTOR_F4
			case 0:LED0_GPIO_PORT->BSRRH = LED0_GPIO_PIN;
						 LED1_GPIO_PORT->BSRRH = LED1_GPIO_PIN;
						 break;
			case 1:LED0_GPIO_PORT->BSRRH = LED0_GPIO_PIN;break;
			case 2:LED1_GPIO_PORT->BSRRH = LED1_GPIO_PIN;break;
		# else
			case 0:LED0_GPIO_PORT->ODR &= ~LED0_GPIO_PIN;
						 LED1_GPIO_PORT->ODR &= ~LED1_GPIO_PIN;
						 break;
			case 1:LED0_GPIO_PORT->ODR &= ~LED0_GPIO_PIN;break;
			case 2:LED1_GPIO_PORT->ODR &= ~LED1_GPIO_PIN;break;
		# endif
			default:break;
		}
	}
}
/*
*********************************************************************************************************
*                                         bsp_led_OFF 
*
* Description: �ر�һ��LED
*             
* Arguments : LedId:LED ID,��IDΪ0ʱ�ر�����LED
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_led_OFF(uint8_t LedId)
{
	if(IsLedConfig == 1)
	{
		switch(LedId)
		{
		# ifdef VECTOR_F4
			case 0:LED0_GPIO_PORT->BSRRL = LED0_GPIO_PIN;
						 LED1_GPIO_PORT->BSRRL = LED1_GPIO_PIN;
						 break;
			case 1:LED0_GPIO_PORT->BSRRL = LED0_GPIO_PIN;break;
			case 2:LED1_GPIO_PORT->BSRRL = LED1_GPIO_PIN;break;
		# else
			case 0:LED0_GPIO_PORT->ODR = LED0_GPIO_PIN;
						 LED1_GPIO_PORT->ODR = LED1_GPIO_PIN;
						 break;
			case 1:LED0_GPIO_PORT->ODR = LED0_GPIO_PIN;break;
			case 2:LED1_GPIO_PORT->ODR = LED1_GPIO_PIN;break;
		# endif
			default:break;
		}
	}
}
