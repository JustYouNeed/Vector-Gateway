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




/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */


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
	

//	位带操作,实现51类似的GPIO控制功能
//	具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//  IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
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


 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入


/* 宏开关，选择是否使用按键模块，默认开启*/
# define USE_KEY	1u

/* 宏开关，选择是否使用LED模块，默认开启 */
# define USE_LED	1u

/* 宏开关，选择是否使用SPI模块，默认关闭 */
# define USE_SPI	0u

/* 宏开关，选择是否使用串口模块，默认开启 */
# define USE_USART	1u

/* 宏开关，选择是否使用SDIO模块，默认关闭 */
# define USE_SDIO	0

/* 宏开关，选择是否使用LCD模块，默认关闭 */
# define USE_LCD	0

/* 宏开关，选择是否使用SRAM模块，默认关闭 */
# define USE_SRAM	0

/* 宏开关，选择是否使用ADC模块，默认关闭 */
# define USE_ADC	0

/* 宏开关，选择是否使用ADC模块，默认关闭 */
# define USE_DAC	0

/* 宏开关，选择是否使用看门狗，默认关闭 */
# define USE_WWDG	0

/* 宏开关，选择是否使用窗口看门狗模块，默认关闭 */
# define USE_IWDG	0

/* 宏开关，选择是否使用定时器模块，默认开启 */
/* 当关闭这一模块时，将无法使用软件定时器以及延时函数 */
# define USE_TIM	1u

/* 宏开关，选择是否使用PWM模块，默认关闭 */
# define USE_PWM	0

/* 宏开关，选择是否使用USB模块，默认关闭 */
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


