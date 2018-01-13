# include "bsp.h"
# include "lwip_app_ping.h"
# include "lwip/sys.h"
# include "lwip/opt.h"
# include "lwip/inet_chksum.h"

static u16_t ping_seq_num;

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
u8_t _cb_ping_recv(void *arg, struct raw_pcb * pcb, struct pbuf *p, ip_addr_t *addr)
{
	struct icmp_echo_hdr *iecho;
	_ping_dev *ping = (_ping_dev *)arg;
	if(p->tot_len >= (PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr)))
	{
		iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + PBUF_IP_HLEN);
		
		if(iecho->type == ICMP_ER && iecho->id == PING_ID && (iecho->seqno == htons(ping_seq_num)))
		{
			ping->ping_recv_time = sys_now();
			ping->ping_time = ping->ping_send_time - ping->ping_recv_time;
			
			pbuf_free(p);
			return 1;
		}
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
static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len)
{
	size_t i;
	size_t data_len = len - sizeof(struct icmp_echo_hdr);
	
	ICMPH_TYPE_SET(iecho, ICMP_ECHO);
	ICMPH_CODE_SET(iecho, 0);
	
	iecho->chksum = 0;
	iecho->id = PING_ID;
	iecho->seqno = htons(++ping_seq_num);
	
	for(i = 0; i < data_len; i++)
	{
		((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
	}
	
	iecho->chksum = inet_chksum(iecho, len);
}

/*
*********************************************************************************************************
*                                        lwip_app_pingConfig  
*
* Description: ping命令初始化函数，在ping之前调用，
*             
* Arguments : ping:_ping_dev结构体指针
*
* Note(s)   : 无
*********************************************************************************************************
*/
void lwip_app_pingConfig(_ping_dev *ping)
{
	ping->ping_pcb = raw_new(IP_PROTO_ICMP);   /* 申请一个新的IP_PROTO_ICMP协议控制块 */
	raw_recv(ping->ping_pcb, _cb_ping_recv, (void *)ping);   /* 注册回调函数，解析回复 */
	raw_bind(ping->ping_pcb, IP_ADDR_ANY);	 /* 设置本地IP地址 */
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
void lwip_app_ping(_ping_dev *ping)
{
	struct pbuf *p;
	struct icmp_echo_hdr *iecho;
	size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;
	
	ping->ping_recv_time = 0;
	ping->ping_send_time = 0;
	ping->ping_time = 0;
	p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
	if(p == NULL) return ;
	
	if((p->len == p->tot_len) && (p->next == NULL))
	{
		iecho = (struct icmp_echo_hdr*)p->payload;
		ping_prepare_echo(iecho, (u16_t )ping_size);
		raw_sendto(ping->ping_pcb, p, &ping->ping_ip);
		
		ping->ping_send_time = sys_now();
	}
	
	pbuf_free(p);
}


