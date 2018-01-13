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
	SysTick_Config(0xFFFFFFFB);
	reload = 168/8;
	reload *= 10000000 / OS_TICKS_SEC;
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
	
	HAL_IncTick();
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
void bsp_tim_DelayMs(uint32_t ui_nMs)
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









