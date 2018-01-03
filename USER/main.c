# include "bsp.h"

# include "app_w25qxx.h"
# include "app_audio.h"

# include "includes.h"
# include "os_app_hooks.h"

# include "ff.h"    
# include "diskio.h"


#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 

# include "lwip/netif.h"
# include "lwip_comm.h"
# include "lwipopts.h"
# include "lwip_app_ping.h"
# include "lwip_app_udp.h"

USB_OTG_CORE_HANDLE USB_OTG_dev;		/*    */
extern FATFS * fat_FLASH;				/*    */
extern FILE*  file;							/*    */
extern UINT br, bw;							/*    */
extern FILINFO * fileInfo;			/*    */
extern DIR dir;									/*    */



# define START_TASK_PRIO	3			/*    */
# define START_TASK_STK_SIZE		512		/*    */
OS_TCB StartTaskTCB;					/*    */
CPU_STK START_TASK_STK[START_TASK_STK_SIZE];		/*    */
void task_StartTask(void *p_arg);				/*    */


# define LED_TASK_PRIO		4
# define LED_TASK_STK_SIZE	128
OS_TCB LedTaskTCB;
CPU_STK LED_TASK_STK[LED_TASK_STK_SIZE];
void task_LedTask(void *p_arg);

# define PRINT_TASK_PRIO		5
# define PRINT_TASK_STK_SIZE	1024
OS_TCB PrintTaskTCB;
CPU_STK PRINT_TASK_STK[PRINT_TASK_STK_SIZE];
void task_PrintTask(void *p_arg);


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
int main(void)
{ 
	OS_ERR err;
	uint8_t result = 0;
	
	
	CPU_SR_ALLOC();
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	bsp_Config();
	disk_initialize(1);
	
	result = f_mount(fat_FLASH, "1:",1);
	bsp_UsartPrintf(COM1, "fmount Flash:%d\r\n", result);

	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
	
	OSInit(&err);
	
  result = lwip_config();
	bsp_UsartPrintf(COM3, "lwip:%d\r\n", result);
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
								
//					

	
  while(1)
	{
//		bsp_led_Toggle(0);
//		bsp_tim_DelayMs(100);
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
	
	OS_CRITICAL_ENTER();	//进入临界区

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
								 
								 
	OS_TaskSuspend((OS_TCB *)&StartTaskTCB, &err);
	OS_CRITICAL_EXIT();								
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
void task_LedTask(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;

	while(DEF_ON)
	{
		bsp_led_Toggle(1);
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
void task_PrintTask(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
//	_ping_dev ping;
	
//	IP4_ADDR(&ping.ping_ip, 192, 168, 137, 2);
//	lwip_app_pingConfig(&ping);
	lwip_app_udpTest();
	while(DEF_ON)
	{
		bsp_UsartPrintf(COM3, "hello world\r\n");
//		lwip_app_ping(&ping);
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT, &err);
	}
}
