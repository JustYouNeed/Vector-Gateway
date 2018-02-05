# include "lwip_app_udp.h"


/*  ¼ì²âÍøÏß×´Ì¬ÈÎÎñ  */
# define UDP_TASK_PRIO		6
# define UDP_TASK_STK_SIZE	512
OS_TCB UDPTestTaskTCB;
CPU_STK UDP_TASK_STK[UDP_TASK_STK_SIZE];
//void task_UDPTestTask(void *p_arg);

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
static void _cb_udpCilent(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	struct pbuf *q;
	const char *reply = "this is a reply\r\n";
	
	pbuf_free(p);
	
	q = pbuf_alloc(PBUF_TRANSPORT, strlen(reply)+1, PBUF_RAM);
	if(q == NULL) return ;
	memset(q->payload, 0, q->len);
	memcpy(q->payload, reply, strlen(reply));
	
	udp_sendto(upcb, q, addr, port);
	pbuf_free(q);
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
void lwip_app_udpTest(void)
{
	struct udp_pcb *udp;
	struct ip_addr remoteIP;
	err_t err;
	
	udp = udp_new();
	IP4_ADDR(&remoteIP, 192, 168, 137, 2);
	
	if(udp != NULL)
	{
		err = udp_bind(udp, IP_ADDR_ANY, 8080);
		if(err == ERR_OK)
		{
			udp_recv(udp, _cb_udpCilent, NULL);
		}
	}
}




static void lwip_app_udp_Thread(void *p_arg)
{
	static struct netbuf *buf;
	static struct netconn *conn;
	static ip_addr_t *addr;
	static unsigned short port;
	
	err_t err;
	
	conn = netconn_new(NETCONN_UDP);
	netconn_bind(conn, NULL, 7);
	
	while(1)
	{
		err = netconn_recv(conn, &buf);
		if(err == ERR_OK)
		{
			addr = netbuf_fromaddr(buf);
			port = netbuf_fromport(buf);
			
			err = netconn_send(conn, buf);
			if(err != ERR_OK)
			{
				//LWIP_DEBUGF();
			}
			netbuf_delete(buf);
		}
	}
	
}


void lwip_app_udp_CreateThread(void)
{
	OS_ERR err;
	OSTaskCreate( (OS_TCB *)&UDPTestTaskTCB,
							(CPU_CHAR *)"Start Task",
							(OS_TASK_PTR )lwip_app_udp_Thread,
							(void *)0,
							(OS_PRIO )UDP_TASK_PRIO,
							(CPU_STK	*)&UDP_TASK_STK[0],
							(CPU_STK_SIZE )UDP_TASK_STK_SIZE/10,
							(CPU_STK_SIZE )UDP_TASK_STK_SIZE,
							(OS_MSG_QTY )0,
							(OS_TICK )0,
							(void *)0,
							(OS_OPT )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							(OS_ERR *)&err);
}

