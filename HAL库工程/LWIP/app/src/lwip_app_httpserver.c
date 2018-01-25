# include "lwip_app_httpserver.h"

const uint8_t html[] = 
	"<html>\
	 <head><title> Vector Gateway Control Web </title></head>\
	 <center><p> Version V1.0.0 </center>\
	 </html>";


static err_t http_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	char *data = NULL;
	if(p != NULL)
	{
		tcp_recved(pcb, p->tot_len);
		data = p->payload;
		if((p->len >= 3) && data[0] == 'G' && data[1] == 'E' && data[2] == 'T')  /* HTTP GET请求，返回网页 */
		{
			printf("GET Request\r\n");
			tcp_write(pcb, html, sizeof(html), 1);
		}
		else
		{
			printf("Request Error\r\n");
		}
		pbuf_free(p);  /* 数据处理完后释放内存 */
	}
	else if(err == ERR_OK)
	{
		return tcp_close(pcb);
	}
	return ERR_OK;
}

static err_t http_server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	tcp_recv(pcb, http_server_recv);  /* 向控制块中注册接收回调函数 */
	return ERR_OK;
}


void lwip_app_HTTPServerConfig(void)
{
	struct tcp_pcb *pcb;
	pcb = tcp_new();
	tcp_bind(pcb, IP_ADDR_ANY, 80);
	pcb = tcp_listen(pcb);
	tcp_accept(pcb, http_server_accept);
}

