/*************************************************   �弶֧��ͷ�ļ�   *********************************************************/
# include "bsp_led.h"
# include "bsp_timer.h"
# include "bsp_key.h"
# include "bsp_malloc.h"

/******************************************************  �ָ���  **************************************************************/

/*****************************************************   Ӧ��ͷ�ļ�   *********************************************************/
# include "app_w25qxx.h"
# include "app_wdog.h"
# include "app_sys.h"

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
# include "usb_device.h"
# include "usb_host.h"
# include "usbd_user.h"

/******************************************************  �ָ���  **************************************************************/

/*****************************************************   LWIPͷ�ļ�   *********************************************************/
# include "lwip/netif.h"
# include "lwip_comm.h"
# include "lwipopts.h"
# include "lwip_app_telnet.h"
# include "lwip_app_httpserver.h"
# include "lwip_app_tcpcilent.h"

/******************************************************  �ָ���  **************************************************************/


extern void SystemClock_Config(void);

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


/*  ����ɨ������  */
# define KEY_TASK_PRIO		9
# define KEY_TASK_STK_SIZE	128
OS_TCB KeyTaskTCB;
CPU_STK KEY_TASK_STK[KEY_TASK_STK_SIZE];
void task_KeyTask(void *p_arg);


/*  ι������  */
# define WDOG_TASK_PRIO		10
# define WDOG_TASK_STK_SIZE	128
OS_TCB WdogTaskTCB;
CPU_STK WDOG_TASK_STK[WDOG_TASK_STK_SIZE];
void task_WdogTask(void *p_arg);


/*  �������״̬����  */
# define LINK_TASK_PRIO		11
# define LINK_TASK_STK_SIZE	1024
OS_TCB LinkCheckTaskTCB;
CPU_STK LINK_TASK_STK[LINK_TASK_STK_SIZE];
void task_LinkCheckTask(void *p_arg);


extern void ETH_MACDMAConfig(ETH_HandleTypeDef *heth, uint32_t err);
extern FATFS * fat_FLASH;				/*  �ļ�ϵͳ����  */
extern FILE*  file;							/*  �ļ�����  */
extern UINT br, bw;							/*  �ļ���д״̬  */
extern FILINFO * fileInfo;			/*  �ļ���Ϣ  */
extern DIR dir;									/*  �ļ�Ŀ¼  */


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

	uint8_t result = 0;
int main(void)
{ 
	OS_ERR err;

	HAL_Init();
	CPU_SR_ALLOC();

	SystemClock_Config();
	bsp_tim_SoftConfig();
	bsp_led_Config();
	
	bsp_led_ON(1);
	
	
	bsp_key_Config();
	bsp_mem_Config();
	
	MX_USB_DEVICE_Init();
	
//	bsp_tim_DelayMs(2000);
	
//	disk_initialize(1);
//	
//	result = f_mount(fat_FLASH, "1:",1);


	OSInit(&err);
	
	if(lwip_config() == 0)  /* ֻ�����������ӵ�ʱ����ʼ���ɹ� */
	{
//		lwip_app_telnetConfig();
//		lwip_app_HTTPServerConfig();
//		lwip_app_TCPClientConfig();
	}
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
	
//	app_wdog_Config(10000);		/*  ���������ֳ�ʼ����������Ź���ι�����10S  */
	
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
								 				
//	/*  �������Ź�����  */
//	OSTaskCreate((OS_TCB 	* )&WdogTaskTCB,		
//							 (CPU_CHAR	* )"Wdog Task", 		
//							 (OS_TASK_PTR )task_WdogTask, 			
//							 (void		* )0,					
//							 (OS_PRIO	  )WDOG_TASK_PRIO,     	
//							 (CPU_STK   * )&WDOG_TASK_STK[0],	
//							 (CPU_STK_SIZE)WDOG_TASK_STK_SIZE/10,	
//							 (CPU_STK_SIZE)WDOG_TASK_STK_SIZE,		
//							 (OS_MSG_QTY  )0,					
//							 (OS_TICK	  )0,					
//							 (void   	* )0,				
//							 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
//							 (OS_ERR 	* )&err);
							 
	/*  ��������״̬�������  */
	OSTaskCreate((OS_TCB 	* )&LinkCheckTaskTCB,		
							 (CPU_CHAR	* )"Link Check Task", 		
							 (OS_TASK_PTR )task_LinkCheckTask, 			
							 (void		* )0,					
							 (OS_PRIO	  )LINK_TASK_PRIO,     	
							 (CPU_STK   * )&LINK_TASK_STK[0],	
							 (CPU_STK_SIZE)LINK_TASK_STK_SIZE/10,	
							 (CPU_STK_SIZE)LINK_TASK_STK_SIZE,		
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

	bsp_led_OFF(2);
	while(DEF_ON)
	{
		if(Sys_Info.LinkStatus == PHY_LINKED_STATUS)
		{	
			bsp_led_Toggle(2);
			OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT, &err);
		}
		else
		{
			bsp_led_Toggle(2);
			OSTimeDlyHMSM(0,0,0,50,OS_OPT_TIME_HMSM_STRICT, &err);
		}
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
	p_arg = p_arg;	
	while(DEF_ON)
	{

		usb_printf("Link Status:%d\r\n", Sys_Info.LinkStatus);
		OSTimeDlyHMSM(0, 0, 0, 500, OS_OPT_TIME_HMSM_STRICT, &err);
	}
}


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void task_WdogTask(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
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
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void task_LinkCheckTask(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	uint32_t error;
	uint32_t phyreg = 0x00;
	uint8_t Status = 0x00;
	
	HAL_ETH_ReadPHYRegister(&ETH_Handler, PHY_BSR, &phyreg);
	Status =  (phyreg & PHY_LINKED_STATUS);
	
	while(1)
	{
		HAL_ETH_ReadPHYRegister(&ETH_Handler, PHY_BSR, &phyreg);
		Status =  (phyreg & PHY_LINKED_STATUS);
		
		if((Status != PHY_LINKED_STATUS) && (Sys_Info.LinkStatus == PHY_LINKED_STATUS))/* ���߶Ͽ��� */
		{
			usb_printf("Connection interrupted! Please Check!!!\r\n");			
			
			netif_set_link_down(&lwip_netif);  /* ֪ͨLWIP���ڵ��� */
		}
		if((Status == PHY_LINKED_STATUS) && (Sys_Info.LinkStatus != PHY_LINKED_STATUS))  /* ������������ */
		{
			Sys_Info.LinkStatus = Status;
			usb_printf("Reconnect\r\n");
			
			if(Sys_Info.LwipInitStatus == 1)
			{
				if(bsp_lan_Config() ==  HAL_OK)
				{
					netif_set_link_up(&lwip_netif);  /*  ֪ͨLWIP��Ʒ�Ѿ�����  */
				}
			}
			else
			{
				CPU_SR_ALLOC();
				OS_CRITICAL_ENTER();
				lwip_config();
				netif_set_link_up(&lwip_netif);
				OS_CRITICAL_EXIT();
			}
		}
		
		Sys_Info.LinkStatus = Status; /* ������������״̬ */
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT, &err);
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



