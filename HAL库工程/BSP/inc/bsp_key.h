# ifndef __BSP_KEY_H
# define __BSP_KEY_H


# include "bsp.h"



# ifdef	VECTOR_F1
	# define RCC_KEY_ALL	(RCC_APB2Periph_GPIOE)
	
	# define KEY_UP_PORT	GPIOE
	# define KEY_UP_PIN		GPIO_PIN_9
	
	# define KEY_OK_PORT	GPIOE
	# define KEY_OK_PIN		GPIO_PIN_9
	
	# define KEY_DOWN_PORT	GPIOE
	# define KEY_DOWN_PIN	GPIO_PIN_9
# else
	# define RCC_KEY_ALL	(RCC_AHB1Periph_GPIOE)
	
	# define KEY_UP_PORT	GPIOF
	# define KEY_UP_PIN		GPIO_PIN_9
	
	# define KEY_OK_PORT	GPIOA
	# define KEY_OK_PIN		GPIO_PIN_0
	
	# define KEY_DOWN_PORT	GPIOE
	# define KEY_DOWN_PIN	GPIO_PIN_6
# endif


# define BSP_KEY_COUNT		3
# define KEY_LONG_TIME		220
# define KEY_FILTER_TIME	1

typedef enum{
	KEY_ID_UP = 0x00,
	KEY_ID_OK,
	KEY_ID_DOWN,
}KEY_ID_ENUM;

typedef enum
{
	KEY_NONE = 0x00,
	
	KEY_UP_PRESS,
	KEY_UP_UP,
	KEY_UP_LONG,
	
	KEY_OK_PRESS,
	KEY_OK_UP,
	KEY_OK_LONG,
	
	KEY_DOWN_PRESS,
	KEY_DOWN_UP,
	KEY_DOWN_LONG
}KEY_STAT_ENUM;

# define KEY_FIFO_SIZE	10

typedef struct 
{
	uint8_t Fifo[KEY_FIFO_SIZE];
	uint8_t Read;
	uint8_t Write;
	
	uint8_t IsConfig;
}Key_Fifo_Str;

/*
	每个按键对应1个全局的结构体变量。
*/
typedef struct
{
	uint8_t (*IsKeyPressDunc)(void);
	
	uint8_t Count;
	uint8_t  State;
	uint8_t RepeatSpeed;
	uint8_t RepeatCount;
	uint16_t LongCount;
	uint16_t LongTime;
}Key_Str;


void bsp_key_Config(void);
//void bsp_key_GPIOConfig(void);
//void bsp_key_FIFOConfig(void);
void bsp_key_Scan(void);
//void bsp_key_Detect(uint8_t Id);
void bsp_key_Clear(void);

void bsp_key_Put(uint8_t KeyValue);
uint8_t bsp_key_GetValue(void);
uint8_t bsp_key_GetState(KEY_ID_ENUM KeyId);


# endif


