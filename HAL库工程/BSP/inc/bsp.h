# ifndef __BSP_H
# define __BSP_H

# include "stdio.h"
# include "string.h"

# define VECTOR_F4		1u

# define OS_SUPPORT		1u

# if defined (STM32F10X_HD)|| defined (STM32F10X_MD)|| defined (STM32F10X_LD)
	# define VECTOR_F1		1u
# elif defined (STM32F40_41xxx)
	# define VECTOR_F4		1u
# endif

# ifdef VECTOR_F1
	# include "stm32f10x.h"
	# define ADDR_OFFSET		0
# else
	# include "stm32f4xx.h"
	# define ADDR_OFFSET		8
# endif

# if OS_SUPPORT > 0u
# include "includes.h"
# endif




/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */


# ifndef null
# define null 0
# endif

# ifndef NULL
# define NULL 0
# endif

# ifndef TRUE	
	# define TRUE 1
# endif

# ifndef true
	# define true 1
# endif
	
# ifndef FALSE
	# define FALSE	0
# endif
	
# ifndef false
	# define false 0
# endif
	
# include "bsp_key.h"
# include "bsp_led.h"
# include "bsp_usart.h"
# include "bsp_timer.h"
//# include "bsp_i2c.h"
//# include "bsp_spi.h"
# include "bsp_malloc.h"
//# include "bsp_audio.h"
//# include "bsp_wifi.h"
	

//	λ������,ʵ��51���Ƶ�GPIO���ƹ���
//	����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//  IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12 + ADDR_OFFSET) //0x4001080C + ADDR_OFFSET
#define GPIOB_ODR_Addr    (GPIOB_BASE+12 + ADDR_OFFSET) //0x40010C0C + ADDR_OFFSET
#define GPIOC_ODR_Addr    (GPIOC_BASE+12 + ADDR_OFFSET) //0x4001100C + ADDR_OFFSET
#define GPIOD_ODR_Addr    (GPIOD_BASE+12 + ADDR_OFFSET) //0x4001140C + ADDR_OFFSET
#define GPIOE_ODR_Addr    (GPIOE_BASE+12 + ADDR_OFFSET) //0x4001180C + ADDR_OFFSET
#define GPIOF_ODR_Addr    (GPIOF_BASE+12 + ADDR_OFFSET) //0x40011A0C + ADDR_OFFSET  
#define GPIOG_ODR_Addr    (GPIOG_BASE+12 + ADDR_OFFSET) //0x40011E0C + ADDR_OFFSET  

#define GPIOA_IDR_Addr    (GPIOA_BASE+8 + ADDR_OFFSET) //0x40010808 + ADDR_OFFSET
#define GPIOB_IDR_Addr    (GPIOB_BASE+8 + ADDR_OFFSET) //0x40010C08 + ADDR_OFFSET
#define GPIOC_IDR_Addr    (GPIOC_BASE+8 + ADDR_OFFSET) //0x40011008 + ADDR_OFFSET
#define GPIOD_IDR_Addr    (GPIOD_BASE+8 + ADDR_OFFSET) //0x40011408 + ADDR_OFFSET
#define GPIOE_IDR_Addr    (GPIOE_BASE+8 + ADDR_OFFSET) //0x40011808 + ADDR_OFFSET
#define GPIOF_IDR_Addr    (GPIOF_BASE+8 + ADDR_OFFSET) //0x40011A08 + ADDR_OFFSET 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8 + ADDR_OFFSET) //0x40011E08 + ADDR_OFFSET 


 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //��� 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //����

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //��� 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //����


/* �꿪�أ�ѡ���Ƿ�ʹ�ð���ģ�飬Ĭ�Ͽ���*/
# define USE_KEY	1u

/* �꿪�أ�ѡ���Ƿ�ʹ��LEDģ�飬Ĭ�Ͽ��� */
# define USE_LED	1u

/* �꿪�أ�ѡ���Ƿ�ʹ��SPIģ�飬Ĭ�Ϲر� */
# define USE_SPI	0u

/* �꿪�أ�ѡ���Ƿ�ʹ�ô���ģ�飬Ĭ�Ͽ��� */
# define USE_USART	1u

/* �꿪�أ�ѡ���Ƿ�ʹ��SDIOģ�飬Ĭ�Ϲر� */
# define USE_SDIO	0

/* �꿪�أ�ѡ���Ƿ�ʹ��LCDģ�飬Ĭ�Ϲر� */
# define USE_LCD	0

/* �꿪�أ�ѡ���Ƿ�ʹ��SRAMģ�飬Ĭ�Ϲر� */
# define USE_SRAM	0

/* �꿪�أ�ѡ���Ƿ�ʹ��ADCģ�飬Ĭ�Ϲر� */
# define USE_ADC	0

/* �꿪�أ�ѡ���Ƿ�ʹ��ADCģ�飬Ĭ�Ϲر� */
# define USE_DAC	0

/* �꿪�أ�ѡ���Ƿ�ʹ�ÿ��Ź���Ĭ�Ϲر� */
# define USE_WWDG	0

/* �꿪�أ�ѡ���Ƿ�ʹ�ô��ڿ��Ź�ģ�飬Ĭ�Ϲر� */
# define USE_IWDG	0

/* �꿪�أ�ѡ���Ƿ�ʹ�ö�ʱ��ģ�飬Ĭ�Ͽ��� */
/* ���ر���һģ��ʱ�����޷�ʹ�������ʱ���Լ���ʱ���� */
# define USE_TIM	1u

/* �꿪�أ�ѡ���Ƿ�ʹ��PWMģ�飬Ĭ�Ϲر� */
# define USE_PWM	0

/* �꿪�أ�ѡ���Ƿ�ʹ��USBģ�飬Ĭ�Ϲر� */
# define USE_USB	0

# define USE_MALLOC		1u


void bsp_Config(void);

void bsp_sys_Reset(void);
void bsp_sys_Standby(void);

void bsp_clk_Config(uint32_t PLLn, uint32_t PLLm, uint32_t PLLp, uint32_t PLLq);
uint8_t bsp_clk_Set(uint32_t PLLn, uint32_t PLLm, uint32_t PLLp, uint32_t PLLq);

void bsp_gpio_Config(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pinx, uint32_t GPIO_Mode, uint32_t GPIO_OType, uint32_t GPIO_Speed, uint32_t GPIO_PuPd);
uint8_t bsp_gpio_ReadPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void bsp_gpio_WritePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, uint8_t Pin_Value);
void bsp_gpio_TogglePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
void bsp_gpio_AFSet(GPIO_TypeDef *GPIOx, uint8_t GPIO_Pinx, uint8_t GPIO_AFx);

void bsp_nvic_Config(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_SubPriority,uint8_t NVIC_Channel,uint8_t NVIC_Group);
void bsp_nvic_EXTISet(uint8_t GPIOx, uint8_t GPIO_Pinx, uint8_t TRIM);
void bsp_nvic_PriorityGroupConfig(uint8_t NVIC_Group);
void bsp_nvic_SetVectorTable(uint32_t NVIC_VectorTable, uint32_t Offset);





# endif


