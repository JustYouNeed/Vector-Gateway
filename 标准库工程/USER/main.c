/*************************************************   �弶֧��ͷ�ļ�   *********************************************************/
# include "bsp.h"

/******************************************************  �ָ���  **************************************************************/

/*****************************************************   Ӧ��ͷ�ļ�   *********************************************************/
# include "app_w25qxx.h"
# include "app_audio.h"
# include "app_wdog.h"

/******************************************************  �ָ���  **************************************************************/

/*************************************************   ����ϵͳͷ�ļ�   *********************************************************/
# include "includes.h"
# include "os_app_hooks.h"

/******************************************************  �ָ���  **************************************************************/

/*************************************************   �ļ�ϵͳͷ�ļ�   *********************************************************/
# include "ff.h"    
# include "diskio.h"

/******************************************************  �ָ���  **************************************************************/

/**************************************************   USBӦ��ͷ�ļ�   *********************************************************/
# include "usbd_msc_core.h"
# include "usbd_usr.h"
# include "usbd_desc.h"
# include "usb_conf.h" 
# include "usbh_core.h"

/******************************************************  �ָ���  **************************************************************/

/*****************************************************   LWIPͷ�ļ�   *********************************************************/
# include "lwip/netif.h"
# include "lwip_comm.h"
# include "lwipopts.h"
# include "lwip_app_ping.h"
# include "lwip_app_udp.h"

/******************************************************  �ָ���  **************************************************************/

__ALIGN_BEGIN USBH_HOST	USB_Host __ALIGN_END ;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END ;
uint8_t Enum_Done      = 0; 
USB_OTG_CORE_HANDLE USB_OTG_dev;		/*  USB�豸  */
extern FATFS * fat_FLASH;				/*  �ļ�ϵͳ����  */
extern FILE*  file;							/*  �ļ�����  */
extern UINT br, bw;							/*  �ļ���д״̬  */
extern FILINFO * fileInfo;			/*  �ļ���Ϣ  */
extern DIR dir;									/*  �ļ�Ŀ¼  */


/*  ���������������  */
# define START_TASK_PRIO	3			
# define START_TASK_STK_SIZE		512		
OS_TCB StartTaskTCB;					
CPU_STK START_TASK_STK[START_TASK_STK_SIZE];		
void task_StartTask(void *p_arg);				

/*  LED״ָ̬ʾ�������  */
# define LED_TASK_PRIO		4
# define LED_TASK_STK_SIZE	128
OS_TCB LedTaskTCB;
CPU_STK LED_TASK_STK[LED_TASK_STK_SIZE];
void task_LedTask(void *p_arg);


/*  ��ӡ����  */
# define PRINT_TASK_PRIO		5
# define PRINT_TASK_STK_SIZE	1024
OS_TCB PrintTaskTCB;
CPU_STK PRINT_TASK_STK[PRINT_TASK_STK_SIZE];
void task_PrintTask(void *p_arg);


/*  ι������  */
# define WDOG_TASK_PRIO		10
# define WDOG_TASK_STK_SIZE	128
OS_TCB WdogTaskTCB;
CPU_STK WDOG_TASK_STK[WDOG_TASK_STK_SIZE];
void task_WdogTask(void *p_arg);


/*  ����ɨ������  */
# define KEY_TASK_PRIO		9
# define KEY_TASK_STK_SIZE	128
OS_TCB KeyTaskTCB;
CPU_STK KEY_TASK_STK[KEY_TASK_STK_SIZE];
void task_KeyTask(void *p_arg);


/*
*********************************************************************************************************
*                                         main 
*
* Description: C�����׼��ں���
*             
* Arguments : ��
*
* Note(s)   : ��
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
* Description: ����  LED״ָ̬ʾ���񡢴�ӡ����
*             
* Arguments : p_arg:������������ָ�룬�����ﲢû���õ�
*
* Note(s)   : ������ֻ����һ�Σ���ɺ�Ὣ������𣬱�ĵط�������ָ�
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
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
# endif
	
# ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
# endif

# if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
# endif
	
	app_wdog_Config(5000);		/*  ���������ֳ�ʼ����������Ź���ι�����5S  */
	
	OS_CRITICAL_ENTER();	//�����ٽ���

	/*  ����LED״ָ̬ʾ����  */
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
				 
	/*  ������ӡ����  */
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
								 				
	/*  �������Ź�����  */
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
									 				
	/*  ������������  */
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
	OS_TaskSuspend((OS_TCB *)&StartTaskTCB, &err);		/*  ����ʼ����  */
	OS_CRITICAL_EXIT();								
}



/*
*********************************************************************************************************
*                                          task_LedTask
*
* Description: LED״ָ̬ʾ�����ɵƵ�״̬��ָʾϵͳ�������
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

