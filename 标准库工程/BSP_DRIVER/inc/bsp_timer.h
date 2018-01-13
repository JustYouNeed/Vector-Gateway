# ifndef __BSP_TIMER_H
# define __BSP_TIMER_H

# include "bsp.h"



/* 定义软件定时器使用的定时器 */
# define USE_TIM2		1u
# define USE_TIM3   0u
# define USE_TIM4   0u
# define USE_TIM5   0u



# if USE_TIM2 > 0u
	# define BSP_TIMER				TIM2
	# define BSP_TIMER_IRQ		TIM2_IRQn
	# define BSP_TIM_RCC			RCC_APB1Periph_TIM2
#endif

# if USE_TIM3 > 0u
	# define BSP_TIMER				TIM3
	# define BSP_TIMER_IRQ		TIM3_IRQn
	# define BSP_TIM_RCC			RCC_APB1Periph_TIM3
#endif

# if USE_TIM4 > 0u
	# define BSP_TIMER				TIM4
	# define BSP_TIMER_IRQ		TIM4_IRQn
	# define BSP_TIM_RCC			RCC_APB1Periph_TIM4
#endif

# if USE_TIM5 > 0u
	# define BSP_TIMER				TIM5
	# define BSP_TIMER_IRQ		TIM5_IRQn
	# define BSP_TIM_RCC			RCC_APB1Periph_TIM5
#endif


# define SOFT_TIMER_COUNT			4

typedef void (*_cbTimerCallBack)(void);

typedef struct
{
	volatile uint8_t v_ucMode;	
	volatile uint8_t v_ucFlag;
	volatile uint32_t v_uiCount;
	volatile uint32_t v_uiPreLoad;
	
	uint8_t ucUsed;
	_cbTimerCallBack _cbTimer;
	
}SoftTimer_Str;


typedef struct
{
	uint16_t uiPeriod;
	_cbTimerCallBack _cbTimer;
}HardTimer_Str,*pHardTimer_Str;

typedef enum
{
	TIMER_MODE_ONCE = 0x00,
	TIMER_MODE_AUTO
}TIMER_MODE_ENUM;

void bsp_tim_SoftConfig(void);  /* 初始化软件定时器 */
//void bsp_tim_SoftDec(SoftTimer_Str * pTimer);
int8_t bsp_tim_CreateTimer(uint8_t ucTimerId, uint32_t uiPeriod, _cbTimerCallBack  _cbTimer, TIMER_MODE_ENUM eMode);
int8_t bsp_tim_DeleteTimer(uint8_t ucTimerId);
_cbTimerCallBack bsp_tim_SetTimerCB(uint8_t TimerId, _cbTimerCallBack  _cbTimer);
int8_t bsp_tim_TimerCheck(uint8_t ucTimerId);
int32_t bsp_tim_GetRunTime(void);
int32_t bsp_tim_CheckRunTime(int32_t iLastTime);
_cbTimerCallBack bsp_tim_GetCB(uint8_t ucTimerId);

void bsp_tim_DelayMs(uint16_t ui_nMs);
void bsp_tim_DelayUs(uint16_t ui_nUs);


int8_t bsp_TimerHardConfig(uint8_t TIMx, uint32_t uiPeriod);
void bsp_TimerHardStart(uint8_t TIMx);
uint8_t bsp_TimerHardStop(uint8_t TIMx);
_cbTimerCallBack bsp_TimerHardSetCallBack(uint8_t TIMx, _cbTimerCallBack _cbTimer);


void bsp_TimerPWMConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t ucChannel, uint32_t uiFreq, uint32_t uiDutyCycle);
void bsp_TimerPWMGPIOConfig(GPIO_TypeDef *GPIOx,uint16_t GPIO_PinX, TIM_TypeDef* TIMx, uint8_t ucChannel);



# endif

