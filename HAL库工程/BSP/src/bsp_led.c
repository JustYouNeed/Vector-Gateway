/**
  *******************************************************************************************************
  * File Name: 
  * Author: 
  * Version:  
  * Date: 
  * Brief: 
  *******************************************************************************************************
  * History
  *
  *
  *
  *
  *
  *
  *
  *
  *******************************************************************************************************
  */

# include "bsp_led.h"


/*  This variable indicates whether the LED is initialized   */
static uint8_t IsLedConfig = 0;


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
void bsp_led_GPIOConfig(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = LED0_GPIO_PIN | LED1_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(LED0_GPIO_PORT, &GPIO_InitStructure);
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
void bsp_led_Config(void)
{
	bsp_led_GPIOConfig();
	IsLedConfig = 1;
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
void bsp_led_Toggle(uint8_t LedId)
{
	if(IsLedConfig == 1)
	{
		switch(LedId)
		{
			case 0:LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN;
						 LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN;break;
			case 1:LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN;
						 LED1_GPIO_PORT->BSRR = (uint32_t)LED1_GPIO_PIN;
						break;
			case 2:LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN;
						 LED0_GPIO_PORT->BSRR = (uint32_t)LED0_GPIO_PIN;
						 break;
			default:break;
		}
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
void bsp_led_ON(uint8_t LedId)
{
	if(IsLedConfig == 1)
	{
		switch(LedId)
		{
//			case 0:LED0_GPIO_PORT->BSRRL = LED0_GPIO_PIN;
//						 LED1_GPIO_PORT->BSRRL = LED1_GPIO_PIN;
//						 break;
			case 1:LED0_GPIO_PORT->BSRR = (uint32_t)LED0_GPIO_PIN << 16U;
						 LED1_GPIO_PORT->BSRR = LED1_GPIO_PIN;break;
			case 2:LED1_GPIO_PORT->BSRR = (uint32_t)LED1_GPIO_PIN << 16U;
						 LED0_GPIO_PORT->BSRR = LED0_GPIO_PIN;break;
			default:break;
		}
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
void bsp_led_OFF(uint8_t LedId)
{
	switch(LedId)
	{
		case 0:LED0_GPIO_PORT->BSRR = LED0_GPIO_PIN;
					 LED1_GPIO_PORT->BSRR = LED1_GPIO_PIN;
					 break;
		case 1:LED0_GPIO_PORT->BSRR = LED0_GPIO_PIN;
					 LED1_GPIO_PORT->BSRR = LED1_GPIO_PIN;break;
		case 2:LED1_GPIO_PORT->BSRR = LED1_GPIO_PIN;
					 LED0_GPIO_PORT->BSRR = LED0_GPIO_PIN;break;
		default:break;
	}
}
