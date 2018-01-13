# include "bsp_timer.h"

# if OS_SUPPORT > 0u
	# include "includes.h"
	
	# ifdef OS_CRITICAL_METHOD
		# define OS_RUNNING		OSRunning
		# define OS_TICKS_SEC	OS_TICKS_PER_SEC
		# define OS_INT_NESTING	OSIntNesting
	# endif
	
	# ifdef	CPU_CFG_CRITICAL_METHOD
		# define OS_RUNNING		OSRunning
		# define OS_TICKS_SEC	OSCfg_TickRate_Hz
		# define OS_INT_NESTING	OSIntNestingCtr
	# endif	
# endif

static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

__IO int32_t g_iRunTime = 0;



/*已使用定时器标志位，变量的8个位对应着8个定时器*/
//static uint8_t s_ucUsedTIM = 0x00;
SoftTimer_Str SoftTimer[SOFT_TIMER_COUNT];

//static _cbTimerCallBack TIM1_CallBack = 0;
//static _cbTimerCallBack TIM2_CallBack = 0;
//static _cbTimerCallBack TIM3_CallBack = 0;
//static _cbTimerCallBack TIM4_CallBack = 0;
//static _cbTimerCallBack TIM5_CallBack = 0;
//static _cbTimerCallBack TIM6_CallBack = 0;

/*
*********************************************************************************************************
*                                          bsp_tim_SoftConfig
*
* Description: 初始化软件定时器
*             
* Arguments : 无
*
* Note(s)   : 无
*********************************************************************************************************
*/
void bsp_tim_SoftConfig(void)
{
	uint8_t i = 0;
# if OS_SUPPORT > 0u
	uint32_t reload;
# endif
	
	for(i = 0; i < SOFT_TIMER_COUNT; i++)
	{
		SoftTimer[i].v_uiCount = 0;
		SoftTimer[i].v_uiPreLoad = 0;
		SoftTimer[i].v_ucFlag = 0;
		SoftTimer[i].v_ucMode = 0;
		SoftTimer[i]._cbTimer = 0;
	}
	

	
# if OS_SUPPORT > 0u			/*  如果需要支持操作系统，则滴答定时器需要改变  */
	SysTick_Config(SysTick_CLKSource_HCLK_Div8);
	reload = 168/8;
	reload *= 1000000 / OS_TICKS_SEC;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->LOAD = reload;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
# else
	SysTick_Config(SystemCoreClock / 1000);
# endif
}

/*
*********************************************************************************************************
*                                        bsp_tim_SoftDec  
*
* Description: 递减每个软件定时器的计数器
*             
* Arguments : pTimer:SoftTimer_Str结构体指针，指向一个软件定时器
*
* Note(s)   : 无
*********************************************************************************************************
*/
void bsp_tim_SoftDec(SoftTimer_Str * pTimer)
{
	if(pTimer->v_uiCount >0)		/*  只有在计数器的值大于零才需要递减  */
	{
		if(--pTimer->v_uiCount ==0 )  /*  时间到后标志置位  */
		{
			pTimer->v_ucFlag = 1;
			
			
			if(pTimer->v_ucMode == TIMER_MODE_AUTO)			/*  定时器模式为自动重装载时重装计数器  */
			{
				pTimer->v_uiCount = pTimer->v_uiPreLoad;
			}
			
			if(pTimer->_cbTimer)			/*  设置回调函数时调用回调函数  */
			{
				pTimer->_cbTimer();
			}
		}
	}
}
/*
*********************************************************************************************************
*                                          SysTick_ISR
*
* Description: 滴答定时器中断服务函数
*             
* Arguments : 无
*
* Note(s)   : 无
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
# if OS_SUPPORT < 1u
	uint8_t i = 0;
# endif
	
	if(s_uiDelayCount > 0)		/*  延时计数器  */
	{
		if(-- s_uiDelayCount == 0) s_ucTimeOutFlag = 1;			/*  延时完成  */
	}
	
	g_iRunTime ++;				/*  运行时间计数器  */
	if(g_iRunTime == 0x7fffffff) g_iRunTime = 0;			/*  运行时间计数器为32位，最大值为 0x7fffffff */
	
# if OS_SUPPORT > 0u			/*  如果需要支持操作系统  */
	if(OS_RUNNING == 1)
	{
		OSIntEnter();						//进入中断
		OSTimeTick();       				//调用ucos的时钟服务程序               
		OSIntExit();       	 				//触发任务切换软中断
	}
# else
	for(i = 0; i < SOFT_TIMER_COUNT; i++)		/*  递减软件定时器的值  */
	{
		bsp_tim_SoftDec(&SoftTimer[i]);
	}
# endif
	
}
/*
*********************************************************************************************************
*                                          SysTick_Handler
*
* Description: 滴答定时器中断
*             
* Arguments : 无
*
* Note(s)   : 无
*********************************************************************************************************
*/
void SysTick_Handler(void)
{
	SysTick_ISR();
}

/*
*********************************************************************************************************
*                                          bsp_tim_DelayMs
*
* Description: 毫秒级延时函数
*             
* Arguments : ui_nMs:要延时的时间
*
* Note(s)   : 延时采用滴答定时器实现，因为滴答定时器为16位定时器，所以该最大延时时间为65534
*********************************************************************************************************
*/
void bsp_tim_DelayMs(uint16_t ui_nMs)
{
	if(ui_nMs == 0) return ;
	else if(ui_nMs == 1) ui_nMs = 2;
	
	DISABLE_INT();
	
	s_uiDelayCount = ui_nMs;
	s_ucTimeOutFlag = 0;
	
	ENABLE_INT();
	
	while(1)
	{
		if(s_ucTimeOutFlag == 1) break;
	}
}
/*
*********************************************************************************************************
*                                          bsp_tim_DelayUs
*
* Description: 微秒级延时函数
*             
* Arguments : ui_nUs:延时时长，微秒
*
* Note(s)   : 最大延时为65535微秒
*********************************************************************************************************
*/
void bsp_tim_DelayUs(uint16_t ui_nUs)
{
		uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;
       
		reload = SysTick->LOAD;                
    ticks = ui_nUs * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* 刚进入时的计数器值 */

    while (1)
    {
			tnow = SysTick->VAL;    
			if (tnow != told)
			{    
				/* SYSTICK是一个递减的计数器 */    
				if (tnow < told)
				{
						tcnt += told - tnow;    
				}
				/* 重新装载递减 */
				else
				{
						tcnt += reload - tnow + told;    
				}        
				told = tnow;

				/* 时间超过/等于要延迟的时间,则退出 */
				if (tcnt >= ticks)
				{
					break;
				}
			}  
    }
}
/*
*********************************************************************************************************
*                                          bsp_tim_CreateTimer
*
* Description: 创建一个软件定时器
*             
* Arguments : ucTimerId:软件定时器ID
*							uiPeriod:定时周期
*							_cbTimer:定时器回调函数
*							eMode:定时器模式
*
* Note(s)   : 1.软件定时器个数有限制，当传入ID超过最大定时器数量时将会创建失败
*							2.软件定时器不能覆盖已经使用的定时器，需要将原来的删除后才能创建
*********************************************************************************************************
*/
int8_t bsp_tim_CreateTimer(uint8_t ucTimerId, uint32_t uiPeriod, _cbTimerCallBack  _cbTimer, TIMER_MODE_ENUM eMode)
{
	if(SoftTimer[ucTimerId].ucUsed == 1) return -1;		/*  已经使用了的定时器不能再次创建  */
	if(ucTimerId > SOFT_TIMER_COUNT) return -2;				/*  超过定时器数量  */
	
	DISABLE_INT();				/*  定时器创建的时候不能被打断，关闭中断  */
	SoftTimer[ucTimerId].v_uiCount = uiPeriod;
	SoftTimer[ucTimerId].v_ucFlag = 0;
	SoftTimer[ucTimerId].v_ucMode = eMode;
	SoftTimer[ucTimerId].v_uiPreLoad = uiPeriod;
	ENABLE_INT();					/*  开启中断  */
	return 0;
}
/*
*********************************************************************************************************
*                                          bsp_tim_DeleteTimer
*
* Description: 删除一个软件定时器
*             
* Arguments : ucTimerId:软件定时器ID
*
* Note(s)   : 未使用的定时器不能被删除
*********************************************************************************************************
*/
int8_t bsp_tim_DeleteTimer(uint8_t ucTimerId)
{
	if(SoftTimer[ucTimerId].ucUsed == 0)  return -1;
	if(ucTimerId > SOFT_TIMER_COUNT) return -2;
	
	DISABLE_INT();
	
	SoftTimer[ucTimerId].v_ucMode = 0;
	SoftTimer[ucTimerId].v_uiPreLoad = 0;
	SoftTimer[ucTimerId].v_uiCount = 0;
	
	ENABLE_INT();
	
	return 0;
}
/*
*********************************************************************************************************
*                                          bsp_tim_TimerCheck
*
* Description: 检查软件定时器是否到达定时时间
*             
* Arguments : ucTimerId:定时器ID
*
* Note(s)   : 
*********************************************************************************************************
*/
int8_t bsp_tim_TimerCheck(uint8_t ucTimerId)
{
	if(SoftTimer[ucTimerId].ucUsed == 0) return -1;
	
	if(ucTimerId > SOFT_TIMER_COUNT) return -2;
	
	return (SoftTimer[ucTimerId].v_ucFlag == 1)?0:1;
}

/*
*********************************************************************************************************
*                                          bsp_tim_GetRunTime
*
* Description: 获取系统运行时间
*             
* Arguments : 无
*
* Note(s)   : 无
*********************************************************************************************************
*/
int32_t bsp_tim_GetRunTime(void)
{
	int32_t runTime;
	DISABLE_INT();
	
	runTime = g_iRunTime;
	
	ENABLE_INT();
	
	return runTime;
}
/*
*********************************************************************************************************
*                                          bsp_tim_CheckRunTime
*
* Description: 检测时间差
*             
* Arguments : iLastTime:上一个时刻的时间值
*
* Note(s)   : 
*********************************************************************************************************
*/
int32_t bsp_tim_CheckRunTime(int32_t iLastTime)
{
	int32_t nowTime;
	int32_t timeDiff;
	
	DISABLE_INT();
	
	nowTime = g_iRunTime;
	
	ENABLE_INT();
	
	if (nowTime >= iLastTime)
	{
		timeDiff = nowTime - iLastTime;
	}
	else
	{
		timeDiff = 0x7FFFFFFF - iLastTime + nowTime;
	}
	
	return timeDiff;
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
_cbTimerCallBack  bsp_tim_SetTimerCB(uint8_t TimerId, _cbTimerCallBack  _cbTimer)
{
	_cbTimerCallBack _cbTimerTemp;
	_cbTimerTemp = SoftTimer[TimerId]._cbTimer;
	
	SoftTimer[TimerId]._cbTimer = _cbTimer;
	
	return _cbTimerTemp;
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
int8_t bsp_TimerHardConfig(uint8_t TIMx, uint32_t uiPeriod)
{
	if(TIMx < 2 || TIMx > 5) return -1;
	
	if(TIMx == 2)
	{
		
	}else if(TIMx == 3)
	{
		
	}else if(TIMx == 4)
	{
		
	}else if(TIMx == 5)
	{
		
	}
	return 0;
}


//void TIM2_IRQHandler(void)
//{
//	
//}

//void TIM3_IRQHandler(void)
//{
//	
//}

//void TIM4_IRQHandler(void)
//{
//	
//}

//void TIM5_IRQHandler(void)
//{
//	
//}



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
void bsp_TimerPWMGPIOConfig(GPIO_TypeDef *GPIOx,uint16_t GPIO_PinX, TIM_TypeDef* TIMx, uint8_t ucChannel)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
# ifdef VECTOR_F1
# elif defined VECTOR_F4
	bsp_GPIOClcokCmd(GPIOx);
	bsp_TIMClockCmd(TIMx);
	
		/* 配置GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_PinX;	/* 带入的形参 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

	/* 连接到AF功能 */
	GPIO_PinAFConfig(GPIOx, bsp_GetPinSource(GPIO_PinX), bsp_GetTIMPinAF(TIMx));
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
void bsp_TimerPWMConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, TIM_TypeDef* TIMx, uint8_t ucChannel, uint32_t uiFreq, uint32_t uiDutyCycle)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	
	bsp_TimerPWMGPIOConfig(GPIOx, GPIO_Pin, TIMx, ucChannel);
	
	    /*-----------------------------------------------------------------------
		system_stm32f4xx.c 文件中 void SetSysClock(void) 函数对时钟的配置如下：

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		因为APB1 prescaler != 1, 所以 APB1上的TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		因为APB2 prescaler != 1, 所以 APB2上的TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 定时器有 TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
		APB2 定时器有 TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 定时器 */
		uiTIMxCLK = SystemCoreClock;
	}
	else	/* APB1 定时器 */
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (uiFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* 分频比 = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / uiFreq  - 1;		/* 自动重装的值 */
	}
	else if (uiFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* 分频比 = 100 */
		usPeriod =  (uiTIMxCLK / 100) / uiFreq  - 1;		/* 自动重装的值 */
	}
	else	/* 大于4K的频率，无需分频 */
	{
		usPrescaler = 0;					/* 分频比 = 1 */
		usPeriod = uiTIMxCLK / uiFreq - 1;	/* 自动重装的值 */
	}
	
		/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 和 TIM8 必须设置 */	

	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = (uiDutyCycle * usPeriod) / 10000;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;	/* only for TIM1 and TIM8. */	
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;			/* only for TIM1 and TIM8. */		
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;		/* only for TIM1 and TIM8. */
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;		/* only for TIM1 and TIM8. */
	
		if (ucChannel == 1)
	{
		TIM_OC1Init(TIMx, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (ucChannel == 2)
	{
		TIM_OC2Init(TIMx, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (ucChannel == 3)
	{
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	else if (ucChannel == 4)
	{
		TIM_OC4Init(TIMx, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	
	TIM_ARRPreloadConfig(TIMx, ENABLE);

	/* TIMx enable counter */
	TIM_Cmd(TIMx, ENABLE);

	/* 下面这句话对于TIM1和TIM8是必须的，对于TIM2-TIM6则不必要 */
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
	}
}









