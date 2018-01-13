/*************************************************   板级支持头文件   *********************************************************/
# include "bsp.h"

/******************************************************  分割线  **************************************************************/

/*****************************************************   应用头文件   *********************************************************/
# include "app_w25qxx.h"
# include "app_audio.h"
# include "app_wdog.h"

/******************************************************  分割线  **************************************************************/

/*************************************************   操作系统头文件   *********************************************************/
# include "includes.h"
# include "os_app_hooks.h"

/******************************************************  分割线  **************************************************************/

/*************************************************   文件系统头文件   *********************************************************/
# include "ff.h"    
# include "diskio.h"

/******************************************************  分割线  **************************************************************/

/**************************************************   USB应用头文件   *********************************************************/
# include "usbd_msc_core.h"
# include "usbd_usr.h"
# include "usbd_desc.h"
# include "usb_conf.h" 
# include "usbh_core.h"

/******************************************************  分割线  **************************************************************/

/*****************************************************   LWIP头文件   *********************************************************/
# include "lwip/netif.h"
# include "lwip_comm.h"
# include "lwipopts.h"
# include "lwip_app_ping.h"
# include "lwip_app_udp.h"

/******************************************************  分割线  **************************************************************/

__ALIGN_BEGIN USBH_HOST	USB_Host __ALIGN_END ;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END ;
uint8_t Enum_Done      = 0; 
USB_OTG_CORE_HANDLE USB_OTG_dev;		/*  USB设备  */
extern FATFS * fat_FLASH;				/*  文件系统变量  */
extern FILE*  file;							/*  文件变量  */
extern UINT br, bw;							/*  文件读写状态  */
extern FILINFO * fileInfo;			/*  文件信息  */
extern DIR dir;									/*  文件目录  */


/*  启动任务相关数据  */
# define START_TASK_PRIO	3			
# define START_TASK_STK_SIZE		512		
OS_TCB StartTaskTCB;					
CPU_STK START_TASK_STK[START_TASK_STK_SIZE];		
void task_StartTask(void *p_arg);				

/*  LED状态指示相关任务  */
# define LED_TASK_PRIO		4
# define LED_TASK_STK_SIZE	128
OS_TCB LedTaskTCB;
CPU_STK LED_TASK_STK[LED_TASK_STK_SIZE];
void task_LedTask(void *p_arg);


/*  打印任务  */
# define PRINT_TASK_PRIO		5
# define PRINT_TASK_STK_SIZE	1024
OS_TCB PrintTaskTCB;
CPU_STK PRINT_TASK_STK[PRINT_TASK_STK_SIZE];
void task_PrintTask(void *p_arg);


/*  喂狗任务  */
# define WDOG_TASK_PRIO		10
# define WDOG_TASK_STK_SIZE	128
OS_TCB WdogTaskTCB;
CPU_STK WDOG_TASK_STK[WDOG_TASK_STK_SIZE];
void task_WdogTask(void *p_arg);


/*  按键扫描任务  */
# define KEY_TASK_PRIO		9
# define KEY_TASK_STK_SIZE	128
OS_TCB KeyTaskTCB;
CPU_STK KEY_TASK_STK[KEY_TASK_STK_SIZE];
void task_KeyTask(void *p_arg);


/*
*********************************************************************************************************
*                                         main 
*
* Description: C程序标准入口函数
*             
* Arguments : 无
*
* Note(s)   : 无
*********************************************************************************************************
*/
int main(void)
{ 
	OS_ERR err;
	uint8_t result = 0;
	
	CPU_SR_ALLOC();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_Config();
	bsp_led_ON(1);
	disk_initialize(1);
	
	result = f_mount(fat_FLASH, "1:",1);
	bsp_UsartPrintf(COM1, "fmount Flash:%d\r\n", result);

	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	
	OSInit(&err);
//	
//  result = lwip_config();
//	bsp_UsartPrintf(COM3, "lwip:%d\r\n", result);
	OS_CRITICAL_ENTER();
	
	OSTaskCreate( (OS_TCB *)&StartTaskTCB,
							  (CPU_CHAR *)"Start Task",
								(OS_TASK_PTR )task_StartTask,
								(void *)0,
								(OS_PRIO )START_TASK_PRIO,
								(CPU_STK	*)&START_TASK_STK[0],
								(CPU_STK_SIZE )START_TASK_STK_SIZE/10,
								(CPU_STK_SIZE )START_TASK_STK_SIZE,
								(OS_MSG_QTY )0,
								(OS_TICK )0,
								(void *)0,
								(OS_OPT )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
								(OS_ERR *)&err);
								
	
	OS_CRITICAL_EXIT();
	OSStart(&err);
							
  while(1);
}


/*
*********************************************************************************************************
*                                       task_StartTask   
*
* Description: 创建  LED状态指示任务、打印任务
*             
* Arguments : p_arg:任意类型数据指针，在这里并没有用到
*
* Note(s)   : 本任务只运行一次，完成后会将自身挂起，别的地方不允许恢复
*********************************************************************************************************
*/
void task_StartTask(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	p_arg = p_arg;
	
	CPU_Init();
# if LWIP_DHCP == 1
	lwip_DHCPTask_Create();
# endif
	
# if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
# endif
	
# ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
# endif

# if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
# endif
	
	app_wdog_Config(5000);		/*  在其他部分初始化完后开启看门狗，喂狗间隔5S  */
	
	OS_CRITICAL_ENTER();	//进入临界区

	/*  创建LED状态指示任务  */
	OSTaskCreate((OS_TCB 	* )&LedTaskTCB,		
							 (CPU_CHAR	* )"Led Task", 		
							 (OS_TASK_PTR )task_LedTask, 			
							 (void		* )0,					
							 (OS_PRIO	  )LED_TASK_PRIO,     
							 (CPU_STK   * )&LED_TASK_STK[0],	
							 (CPU_STK_SIZE)LED_TASK_STK_SIZE/10,	
							 (CPU_STK_SIZE)LED_TASK_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,					
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 (OS_ERR 	* )&err);				
				 
	/*  创建打印任务  */
	OSTaskCreate((OS_TCB 	* )&PrintTaskTCB,		
							 (CPU_CHAR	* )"Print Task", 		
							 (OS_TASK_PTR )task_PrintTask, 			
							 (void		* )0,					
							 (OS_PRIO	  )PRINT_TASK_PRIO,     	
							 (CPU_STK   * )&PRINT_TASK_STK[0],	
							 (CPU_STK_SIZE)PRINT_TASK_STK_SIZE/10,	
							 (CPU_STK_SIZE)PRINT_TASK_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,				
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
							 (OS_ERR 	* )&err);
								 				
	/*  创建看门狗任务  */
	OSTaskCreate((OS_TCB 	* )&WdogTaskTCB,		
							 (CPU_CHAR	* )"Wdog Task", 		
							 (OS_TASK_PTR )task_WdogTask, 			
							 (void		* )0,					
							 (OS_PRIO	  )WDOG_TASK_PRIO,     	
							 (CPU_STK   * )&WDOG_TASK_STK[0],	
							 (CPU_STK_SIZE)WDOG_TASK_STK_SIZE/10,	
							 (CPU_STK_SIZE)WDOG_TASK_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,				
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
							 (OS_ERR 	* )&err);
									 				
	/*  创建按键任务  */
	OSTaskCreate((OS_TCB 	* )&KeyTaskTCB,		
							 (CPU_CHAR	* )"Wdog Task", 		
							 (OS_TASK_PTR )task_KeyTask, 			
							 (void		* )0,					
							 (OS_PRIO	  )KEY_TASK_PRIO,     	
							 (CPU_STK   * )&KEY_TASK_STK[0],	
							 (CPU_STK_SIZE)KEY_TASK_STK_SIZE/10,	
							 (CPU_STK_SIZE)KEY_TASK_STK_SIZE,		
							 (OS_MSG_QTY  )0,					
							 (OS_TICK	  )0,					
							 (void   	* )0,				
							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
							 (OS_ERR 	* )&err);						

	bsp_led_OFF(1);							 
	OS_TaskSuspend((OS_TCB *)&StartTaskTCB, &err);		/*  挂起开始任务  */
	OS_CRITICAL_EXIT();								
}



/*
*********************************************************************************************************
*                                          task_LedTask
*
* Description: LED状态指示任务，由灯的状态来指示系统运行情况
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void task_LedTask(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;

	while(DEF_ON)
	{
		bsp_led_Toggle(2);
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT, &err);
	}
}


/*
*********************************************************************************************************
*                                         task_PrintTask 
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void task_PrintTask(void *p_arg)
{
	OS_ERR err;
	uint8_t key;
	
	p_arg = p_arg;
	
//	lwip_app_udpTest();
	while(DEF_ON)
	{
		key = bsp_key_GetValue();
		if(key == KEY_OK_PRESS)
		{
			bsp_UsartPrintf(COM3, "key is pressed\r\n");
		}
		else if(key == KEY_OK_LONG)
		{
			bsp_UsartPrintf(COM3, "System Reseting....\r\n");
			bsp_tim_DelayMs(1000);
			bsp_SoftReset();
		}
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT, &err);
	}
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
void task_WdogTask(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		app_wdog_Feed();
		OSTimeDlyHMSM(0,0,3,0,OS_OPT_TIME_HMSM_STRICT, &err);
	}
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
void task_KeyTask(void *p_arg)
{
	OS_ERR err;
	
	while(1)
	{
		bsp_key_Scan();
		OSTimeDlyHMSM(0,0,0,20,OS_OPT_TIME_HMSM_STRICT, &err);
	}
}

