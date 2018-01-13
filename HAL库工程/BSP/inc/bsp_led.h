# ifndef __BSP_LED_H
# define __BSP_LED_H

# include "stm32f4xx.h"
# include "bsp.h"

# ifdef VECTOR_F1
	# define RCC_LED_ALL	(RCC_APB2Periph_GPIOE)

	# define LED0_GPIO_PORT		GPIOE
	# define LED0_GPIO_PIN		GPIO_Pin_2
	# define LED0_GPIO_MODE		GPIO_Mode_Out_PP

	# define LED1_GPIO_PORT		GPIOE
	# define LED1_GPIO_PIN		GPIO_Pin_3
	# define LED1_GPIO_MODE		GPIO_Mode_Out_PP
# else
	# define LED0_GPIO_PORT		GPIOD
	# define LED0_GPIO_PIN		GPIO_PIN_0

	# define LED1_GPIO_PORT		GPIOD
	# define LED1_GPIO_PIN		GPIO_PIN_1
# endif

void bsp_led_Config(void);
void bsp_led_GPIOConfig(void);
void bsp_led_Toggle(uint8_t LedId);
void bsp_led_ON(uint8_t LedId);
void bsp_led_OFF(uint8_t LedId);
uint8_t bsp_led_GetState(uint8_t LedId);
# endif

