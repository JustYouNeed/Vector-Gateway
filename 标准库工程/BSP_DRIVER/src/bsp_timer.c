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



/*��ʹ�ö�ʱ����־λ��������8��λ��Ӧ��8����ʱ��*/
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
* Description: ��ʼ�������ʱ��
*             
* Arguments : ��
*
* Note(s)   : ��
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
	

	
# if OS_SUPPORT > 0u			/*  �����Ҫ֧�ֲ���ϵͳ����δ�ʱ����Ҫ�ı�  */
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
* Description: �ݼ�ÿ�������ʱ���ļ�����
*             
* Arguments : pTimer:SoftTimer_Str�ṹ��ָ�룬ָ��һ�������ʱ��
*
* Note(s)   : ��
*********************************************************************************************************
*/
void bsp_tim_SoftDec(SoftTimer_Str * pTimer)
{
	if(pTimer->v_uiCount >0)		/*  ֻ���ڼ�������ֵ���������Ҫ�ݼ�  */
	{
		if(--pTimer->v_uiCount ==0 )  /*  ʱ�䵽���־��λ  */
		{
			pTimer->v_ucFlag = 1;
			
			
			if(pTimer->v_ucMode == TIMER_MODE_AUTO)			/*  ��ʱ��ģʽΪ�Զ���װ��ʱ��װ������  */
			{
				pTimer->v_uiCount = pTimer->v_uiPreLoad;
			}
			
			if(pTimer->_cbTimer)			/*  ���ûص�����ʱ���ûص�����  */
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
* Description: �δ�ʱ���жϷ�����
*             
* Arguments : ��
*
* Note(s)   : ��
*********************************************************************************************************
*/
void SysTick_ISR(void)
{
# if OS_SUPPORT < 1u
	uint8_t i = 0;
# endif
	
	if(s_uiDelayCount > 0)		/*  ��ʱ������  */
	{
		if(-- s_uiDelayCount == 0) s_ucTimeOutFlag = 1;			/*  ��ʱ���  */
	}
	
	g_iRunTime ++;				/*  ����ʱ�������  */
	if(g_iRunTime == 0x7fffffff) g_iRunTime = 0;			/*  ����ʱ�������Ϊ32λ�����ֵΪ 0x7fffffff */
	
# if OS_SUPPORT > 0u			/*  �����Ҫ֧�ֲ���ϵͳ  */
	if(OS_RUNNING == 1)
	{
		OSIntEnter();						//�����ж�
		OSTimeTick();       				//����ucos��ʱ�ӷ������               
		OSIntExit();       	 				//���������л����ж�
	}
# else
	for(i = 0; i < SOFT_TIMER_COUNT; i++)		/*  �ݼ������ʱ����ֵ  */
	{
		bsp_tim_SoftDec(&SoftTimer[i]);
	}
# endif
	
}
/*
*********************************************************************************************************
*                                          SysTick_Handler
*
* Description: �δ�ʱ���ж�
*             
* Arguments : ��
*
* Note(s)   : ��
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
* Description: ���뼶��ʱ����
*             
* Arguments : ui_nMs:Ҫ��ʱ��ʱ��
*
* Note(s)   : ��ʱ���õδ�ʱ��ʵ�֣���Ϊ�δ�ʱ��Ϊ16λ��ʱ�������Ը������ʱʱ��Ϊ65534
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
* Description: ΢�뼶��ʱ����
*             
* Arguments : ui_nUs:��ʱʱ����΢��
*
* Note(s)   : �����ʱΪ65535΢��
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
    ticks = ui_nUs * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */  
    
    tcnt = 0;
    told = SysTick->VAL;             /* �ս���ʱ�ļ�����ֵ */

    while (1)
    {
			tnow = SysTick->VAL;    
			if (tnow != told)
			{    
				/* SYSTICK��һ���ݼ��ļ����� */    
				if (tnow < told)
				{
						tcnt += told - tnow;    
				}
				/* ����װ�صݼ� */
				else
				{
						tcnt += reload - tnow + told;    
				}        
				told = tnow;

				/* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
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
* Description: ����һ�������ʱ��
*             
* Arguments : ucTimerId:�����ʱ��ID
*							uiPeriod:��ʱ����
*							_cbTimer:��ʱ���ص�����
*							eMode:��ʱ��ģʽ
*
* Note(s)   : 1.�����ʱ�����������ƣ�������ID�������ʱ������ʱ���ᴴ��ʧ��
*							2.�����ʱ�����ܸ����Ѿ�ʹ�õĶ�ʱ������Ҫ��ԭ����ɾ������ܴ���
*********************************************************************************************************
*/
int8_t bsp_tim_CreateTimer(uint8_t ucTimerId, uint32_t uiPeriod, _cbTimerCallBack  _cbTimer, TIMER_MODE_ENUM eMode)
{
	if(SoftTimer[ucTimerId].ucUsed == 1) return -1;		/*  �Ѿ�ʹ���˵Ķ�ʱ�������ٴδ���  */
	if(ucTimerId > SOFT_TIMER_COUNT) return -2;				/*  ������ʱ������  */
	
	DISABLE_INT();				/*  ��ʱ��������ʱ���ܱ���ϣ��ر��ж�  */
	SoftTimer[ucTimerId].v_uiCount = uiPeriod;
	SoftTimer[ucTimerId].v_ucFlag = 0;
	SoftTimer[ucTimerId].v_ucMode = eMode;
	SoftTimer[ucTimerId].v_uiPreLoad = uiPeriod;
	ENABLE_INT();					/*  �����ж�  */
	return 0;
}
/*
*********************************************************************************************************
*                                          bsp_tim_DeleteTimer
*
* Description: ɾ��һ�������ʱ��
*             
* Arguments : ucTimerId:�����ʱ��ID
*
* Note(s)   : δʹ�õĶ�ʱ�����ܱ�ɾ��
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
* Description: ��������ʱ���Ƿ񵽴ﶨʱʱ��
*             
* Arguments : ucTimerId:��ʱ��ID
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
* Description: ��ȡϵͳ����ʱ��
*             
* Arguments : ��
*
* Note(s)   : ��
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
* Description: ���ʱ���
*             
* Arguments : iLastTime:��һ��ʱ�̵�ʱ��ֵ
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
	
		/* ����GPIO */
	GPIO_InitStructure.GPIO_Pin = GPIO_PinX;	/* ������β� */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

	/* ���ӵ�AF���� */
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
		system_stm32f4xx.c �ļ��� void SetSysClock(void) ������ʱ�ӵ��������£�

		HCLK = SYSCLK / 1     (AHB1Periph)
		PCLK2 = HCLK / 2      (APB2Periph)
		PCLK1 = HCLK / 4      (APB1Periph)

		��ΪAPB1 prescaler != 1, ���� APB1�ϵ�TIMxCLK = PCLK1 x 2 = SystemCoreClock / 2;
		��ΪAPB2 prescaler != 1, ���� APB2�ϵ�TIMxCLK = PCLK2 x 2 = SystemCoreClock;

		APB1 ��ʱ���� TIM2, TIM3 ,TIM4, TIM5, TIM6, TIM6, TIM12, TIM13,TIM14
		APB2 ��ʱ���� TIM1, TIM8 ,TIM9, TIM10, TIM11

	----------------------------------------------------------------------- */
	if ((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10) || (TIMx == TIM11))
	{
		/* APB2 ��ʱ�� */
		uiTIMxCLK = SystemCoreClock;
	}
	else	/* APB1 ��ʱ�� */
	{
		uiTIMxCLK = SystemCoreClock / 2;
	}

	if (uiFreq < 100)
	{
		usPrescaler = 10000 - 1;					/* ��Ƶ�� = 10000 */
		usPeriod =  (uiTIMxCLK / 10000) / uiFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else if (uiFreq < 3000)
	{
		usPrescaler = 100 - 1;					/* ��Ƶ�� = 100 */
		usPeriod =  (uiTIMxCLK / 100) / uiFreq  - 1;		/* �Զ���װ��ֵ */
	}
	else	/* ����4K��Ƶ�ʣ������Ƶ */
	{
		usPrescaler = 0;					/* ��Ƶ�� = 1 */
		usPeriod = uiTIMxCLK / uiFreq - 1;	/* �Զ���װ��ֵ */
	}
	
		/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = usPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;		/* TIM1 �� TIM8 �������� */	

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

	/* ������仰����TIM1��TIM8�Ǳ���ģ�����TIM2-TIM6�򲻱�Ҫ */
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
	}
}









