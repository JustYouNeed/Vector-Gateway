# include "lwip_app_udp.h"


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

