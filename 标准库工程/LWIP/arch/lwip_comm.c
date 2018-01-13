# include "lwip_comm.h"
# include "netif/etharp.h"
# include "ethernetif.h"
# include "lwip/dhcp.h"
# include "lwip/mem.h"
# include "lwip/memp.h"
# include "lwip/init.h"
# include "lwip/timers.h"
# include "lwip/tcp_impl.h"
# include "lwip/ip_frag.h"
# include "lwip/tcpip.h" 

lwip_dev lwip_info;


CPU_STK * TCPIP_THREAD_TASK_STK;

# if LWIP_DHCP == 1
	# define LWIP_DHCP_TASK_PRIO	7
	# define LWIP_DHCP_TASK_STK_SIZE	512
	OS_TCB LWIP_DHCP_TCB;
	CPU_STK	*LWIP_DHCP_TASK_STK;
	void task_LwipDHCPTask(void *p_arg);
# endif

OS_SEM		LWIP_Input;
OS_MUTEX	LWIP_Mutex;

u32 TCPTimer=0;			//TCP查询计时器
u32 ARPTimer=0;			//ARP查询计时器
u32 lwip_localtime;		//lwip本地时间计数器,单位:ms


uint32_t lwip_time;
extern u32 memp_get_memorysize(void);
extern u8_t *memp_memory;
extern u8_t *ram_heap;

struct netif lwip_netif;				//定义一个全局的网络接口




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
void lwip_default_info_set(lwip_dev *ip_info)
{
	uint32_t sn0;
	sn0 = *(vu32 *)(0x1FFF7A10);
	ip_info->mac[0] = 2;
	ip_info->mac[1] = 0;
	ip_info->mac[2] = 0;
	ip_info->mac[3] = (sn0 >> 16) & 0xff;
	ip_info->mac[4] = (sn0 >> 8) & 0xff;
	ip_info->mac[5] = sn0 & 0xff;
	
	ip_info->remoteip[0] = 192;
	ip_info->remoteip[1] = 168;
	ip_info->remoteip[2] = 1;
	ip_info->remoteip[3] = 104;
	
	ip_info->localip[0] = 192;
	ip_info->localip[1] = 168;
	ip_info->localip[2] = 137;
	ip_info->localip[3] = 33;
	
	ip_info->netmask[0] = 255;
	ip_info->netmask[1] = 255;
	ip_info->netmask[2] = 255;
	ip_info->netmask[3] = 0;
	
	ip_info->gateway[0] = 192;
	ip_info->gateway[1] = 168;
	ip_info->gateway[2] = 137;
	ip_info->gateway[3] = 1;
	ip_info->dhcpStatus = 0;
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
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//得到memp_memory数组大小
	memp_memory=bsp_mem_Malloc(SRAMIN,mempsize);	//为memp_memory申请内存
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//得到ram heap大小
	ram_heap=bsp_mem_Malloc(SRAMIN,ramheapsize);	//为ram_heap申请内存 
# if LWIP_DHCP == 1
	LWIP_DHCP_TASK_STK = (CPU_STK *)bsp_mem_Malloc(SRAMIN, LWIP_DHCP_TASK_STK_SIZE);
	if(!LWIP_DHCP_TASK_STK)
	{
		lwip_comm_mem_free();
		return 1;
	}
#endif
	
	TCPIP_THREAD_TASK_STK = (CPU_STK *)bsp_mem_Malloc(SRAMIN, TCPIP_THREAD_STACKSIZE);
	if(!memp_memory||!ram_heap || !TCPIP_THREAD_TASK_STK)//有申请失败的
	{
		bsp_UsartPrintf(COM3, "Failed\r\n");
		lwip_comm_mem_free();
		return 1;
	}
	return 0;	
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
void lwip_comm_mem_free(void)
{ 	
	bsp_mem_Free(SRAMIN,memp_memory);
	bsp_mem_Free(SRAMIN,ram_heap);
# if LWIP_DHCP == 1
	bsp_mem_Free(SRAMIN,LWIP_DHCP_TASK_STK);
# endif
	bsp_mem_Free(SRAMIN,TCPIP_THREAD_TASK_STK);
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
uint8_t lwip_config(void)
{
	
	CPU_SR_ALLOC();
	
	struct netif *netif_init;
	struct ip_addr localip;
	struct ip_addr netmask;
	struct ip_addr gateway;
	
	if(ETH_Malloc()) return 1;
	lwip_comm_mem_malloc();
	if(bsp_LANConfig()) return 2;
	tcpip_init(NULL,NULL);
	lwip_default_info_set(&lwip_info);

# if LWIP_DHCP == 1
	localip.addr = 0;
	netmask.addr = 0;
	gateway.addr = 0;
	dhcp_start(&lwip_netif);
# else
	IP4_ADDR(&localip, lwip_info.localip[0], lwip_info.localip[1], lwip_info.localip[2], lwip_info.localip[3]);
	IP4_ADDR(&netmask, lwip_info.netmask[0], lwip_info.netmask[1], lwip_info.netmask[2], lwip_info.netmask[3]);
	IP4_ADDR(&gateway, lwip_info.gateway[0], lwip_info.gateway[1], lwip_info.gateway[2], lwip_info.gateway[3]);
	
	bsp_UsartPrintf(COM3,"网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwip_info.mac[0],lwip_info.mac[1],lwip_info.mac[2],lwip_info.mac[3],lwip_info.mac[4],lwip_info.mac[5]);
	bsp_UsartPrintf(COM3,"静态IP地址........................%d.%d.%d.%d\r\n",lwip_info.localip[0],lwip_info.localip[1],lwip_info.localip[2],lwip_info.localip[3]);
	bsp_UsartPrintf(COM3,"子网掩码..........................%d.%d.%d.%d\r\n",lwip_info.netmask[0],lwip_info.netmask[1],lwip_info.netmask[2],lwip_info.netmask[3]);
	bsp_UsartPrintf(COM3,"默认网关..........................%d.%d.%d.%d\r\n",lwip_info.gateway[0],lwip_info.gateway[1],lwip_info.gateway[2],lwip_info.gateway[3]);
# endif
	OS_CRITICAL_ENTER();
	netif_init = netif_add(&lwip_netif, &localip, &netmask, &gateway, NULL, &ethernetif_init, &tcpip_input);
	OS_CRITICAL_EXIT();
	if(netif_init == NULL) return 3;
	
	netif_set_default(&lwip_netif);
	netif_set_up(&lwip_netif);
	return 0;	
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
void lwip_pkt_handle(void)
{
	ethernetif_input(&lwip_netif);
}


# if LWIP_DHCP == 1
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
void lwip_DHCPTask_Create(void)
{
	CPU_SR_ALLOC();
	OS_ERR err;
	OS_CRITICAL_ENTER();
	
	OSTaskCreate((OS_TCB *)&LWIP_DHCP_TCB,
							 (CPU_CHAR *)"Lwip DHCP Task",
							 (OS_TASK_PTR )task_LwipDHCPTask,
							 (void *)0,
							 (OS_PRIO )LWIP_DHCP_TASK_PRIO,
							 (CPU_STK *)&LWIP_DHCP_TASK_STK[0],
							 (CPU_STK_SIZE )LWIP_DHCP_TASK_STK_SIZE/10,
							 (CPU_STK_SIZE )LWIP_DHCP_TASK_STK_SIZE,
							 (OS_MSG_QTY )0,
							 (OS_TICK )0,
							 (void *)0,
							 (OS_OPT )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							 (OS_ERR *)&err);
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
void lwip_DHCPTask_Delete(void)
{
	OS_ERR err;
	dhcp_stop(&lwip_netif);
	OSTaskDel((OS_TCB *)&LWIP_DHCP_TCB,
						(OS_ERR *)&err);
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
void task_LwipDHCPTask(void *p_arg)
{
	OS_ERR err;
	uint32_t localip = 0;
  uint32_t netmask = 0;
	uint32_t gateway = 0;
	//	bsp_UsartPrintf(COM3,"run here\r\n");
	dhcp_start(&lwip_netif);
	lwip_info.dhcpStatus = 0;
	
	bsp_UsartPrintf(COM3, "正在查找DHCP服务器，请稍等............\r\n");
	while(DEF_ON)
	{
		bsp_UsartPrintf(COM3, "正在获取地址...\r\n");
		localip = lwip_netif.ip_addr.addr;
		netmask = lwip_netif.netmask.addr;
		gateway = lwip_netif.gw.addr;
		if(localip != 0)
		{
			lwip_info.dhcpStatus=2;	//DHCP成功
 			bsp_UsartPrintf(COM3, "网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwip_info.mac[0],lwip_info.mac[1],lwip_info.mac[2],lwip_info.mac[3],lwip_info.mac[4],lwip_info.mac[5]);
			//解析出通过DHCP获取到的IP地址
			lwip_info.localip[3]=(uint8_t)(localip>>24); 
			lwip_info.localip[2]=(uint8_t)(localip>>16);
			lwip_info.localip[1]=(uint8_t)(localip>>8);
			lwip_info.localip[0]=(uint8_t)(localip);
			bsp_UsartPrintf(COM3, "通过DHCP获取到IP地址..............%d.%d.%d.%d\r\n",lwip_info.localip[0],lwip_info.localip[1],lwip_info.localip[2],lwip_info.localip[3]);
			//解析通过DHCP获取到的子网掩码地址
			lwip_info.netmask[3]=(uint8_t)(netmask>>24);
			lwip_info.netmask[2]=(uint8_t)(netmask>>16);
			lwip_info.netmask[1]=(uint8_t)(netmask>>8);
			lwip_info.netmask[0]=(uint8_t)(netmask);
			bsp_UsartPrintf(COM3, "通过DHCP获取到子网掩码............%d.%d.%d.%d\r\n",lwip_info.netmask[0],lwip_info.netmask[1],lwip_info.netmask[2],lwip_info.netmask[3]);
			//解析出通过DHCP获取到的默认网关
			lwip_info.gateway[3]=(uint8_t)(gateway>>24);
			lwip_info.gateway[2]=(uint8_t)(gateway>>16);
			lwip_info.gateway[1]=(uint8_t)(gateway>>8);
			lwip_info.gateway[0]=(uint8_t)(gateway);
			bsp_UsartPrintf(COM3, "通过DHCP获取到的默认网关..........%d.%d.%d.%d\r\n",lwip_info.gateway[0],lwip_info.gateway[1],lwip_info.gateway[2],lwip_info.gateway[3]);
			break;
		}else if(lwip_netif.dhcp->tries>LWIP_DHCP_MAX_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
		{  
			lwip_info.dhcpStatus=0XFF;//DHCP失败.
			//使用静态IP地址
			IP4_ADDR(&(lwip_netif.ip_addr),lwip_info.localip[0],lwip_info.localip[1],lwip_info.localip[2],lwip_info.localip[3]);
			IP4_ADDR(&(lwip_netif.netmask),lwip_info.netmask[0],lwip_info.netmask[1],lwip_info.netmask[2],lwip_info.netmask[3]);
			IP4_ADDR(&(lwip_netif.gw),lwip_info.gateway[0],lwip_info.gateway[1],lwip_info.gateway[2],lwip_info.gateway[3]);
			bsp_UsartPrintf(COM3, "DHCP服务超时,使用静态IP地址!\r\n");
			bsp_UsartPrintf(COM3, "网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwip_info.mac[0],lwip_info.mac[1],lwip_info.mac[2],lwip_info.mac[3],lwip_info.mac[4],lwip_info.mac[5]);
			bsp_UsartPrintf(COM3, "静态IP地址........................%d.%d.%d.%d\r\n",lwip_info.localip[0],lwip_info.localip[1],lwip_info.localip[2],lwip_info.localip[3]);
			bsp_UsartPrintf(COM3, "子网掩码..........................%d.%d.%d.%d\r\n",lwip_info.netmask[0],lwip_info.netmask[1],lwip_info.netmask[2],lwip_info.netmask[3]);
			bsp_UsartPrintf(COM3, "默认网关..........................%d.%d.%d.%d\r\n",lwip_info.gateway[0],lwip_info.gateway[1],lwip_info.gateway[2],lwip_info.gateway[3]);
			break;
		} 
		
		OSTimeDlyHMSM(0,0,0,250,OS_OPT_TIME_HMSM_STRICT, &err);
	}
	lwip_DHCPTask_Delete();
}

# endif



