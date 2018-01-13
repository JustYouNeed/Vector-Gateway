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
# else
	# include "stm32f4xx.h"
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


# ifdef VECTOR_F1
  //	λ������,ʵ��51���Ƶ�GPIO���ƹ���
  //	����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
	//  IO�ڲ����궨��
	#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
	#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
	#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
	//IO�ڵ�ַӳ��
	#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
	#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
	#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
	#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
	#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
	#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
	#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

	#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
	#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
	#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
	#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
	#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
	#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
	#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
# elif defined (VECTOR_F4)
	//λ������,ʵ��51���Ƶ�GPIO���ƹ���
	//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).M4ͬM3����,ֻ�ǼĴ�����ַ����.
	//IO�ڲ����궨��
	#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
	#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
	#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
	//IO�ڵ�ַӳ��
	#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
	#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
	#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
	#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
	#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
	#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
	#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
	#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
	#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

	#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
	#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
	#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
	#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
	#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
	#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
	#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
	#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
	#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
# endif

 
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




void bsp_SoftReset(void);



void bsp_Config(void);


# endif


