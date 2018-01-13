# include "app_wdog.h"



/*
*********************************************************************************************************
*                                         app_wdog_Config 
*
* Description: 初始化看门狗
*             
* Arguments : feedTime:看门狗复位时间，也就是再次喂狗时间间隔，单位为ms
*
* Note(s)   : 注意，看门狗一旦打开，就不能再关闭
*********************************************************************************************************
*/
void app_wdog_Config(uint16_t feedTime)
{
	uint16_t reload = 0;
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   /*  使能看门狗写寄存器  */
	
	IWDG_SetPrescaler(4);						/*  看门狗时钟分频固定为4  */
	
	reload = (40 * feedTime) / 64;		/*  计数重装载值  */
	IWDG_SetReload(reload);
	
	IWDG_ReloadCounter();
	IWDG_Enable();					/*  开启看门狗  */
}



/*
*********************************************************************************************************
*                                          app_wdog_Feed
*
* Description: 看门狗喂食函数
*             
* Arguments : 无
*
* Note(s)   : 如果打开看门狗，该函数一定要再喂食时间内调用，否则会引发系统复位
*********************************************************************************************************
*/
void app_wdog_Feed(void)
{
	IWDG_ReloadCounter();			//喂狗
}

