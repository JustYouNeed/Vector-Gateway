# include "lwip_app_tcpcilent.h"



static void _cb_tcp_cilent_conn_err(void *arg, err_t err)
{
	printf("connect error!!\r\n");
	printf("try to connect\r\n");
	lwip_app_TCPClientConfig();
}

static err_t _cb_tcp_cilent_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	if(p != NULL)
	{
		tcp_recved(pcb, p->tot_len);
		tcp_write(pcb, p->payload, p->len, 1);
		pbuf_free(p);
	}
	else if(err == ERR_OK)
	{
		tcp_close(pcb);
		lwip_app_TCPClientConfig();
		return ERR_OK;
	}
	return ERR_OK;
}

static err_t _cb_tcp_cilent_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
//	printf("cilent sent data ok\r\n");
	return ERR_OK;
}

static err_t _cb_tcp_cilent_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
	char string[] = "this is a cilent \r\n";
	int i = 0;
	char *buff = bsp_mem_Malloc(SRAMIN, sizeof(char)*1024);
	
	for(i = 0; i< 1024;i++)
	{
		buff[i] = i;
	}
	tcp_recv(pcb, _cb_tcp_cilent_recv);
	tcp_sent(pcb, _cb_tcp_cilent_sent);
	
	while(1)
	{
		tcp_write(pcb, buff, 1024, 1);
	}
	
	return ERR_OK;
}

void lwip_app_TCPClientConfig(void)
{
	struct tcp_pcb *pcb = NULL;
	struct ip_addr server_ip;
	
	pcb = tcp_new();
	IP4_ADDR(&server_ip, 192, 168, 1, 110);
	tcp_connect(pcb, &server_ip, 8080, _cb_tcp_cilent_connected);
	tcp_err(pcb, _cb_tcp_cilent_conn_err);
	
}


