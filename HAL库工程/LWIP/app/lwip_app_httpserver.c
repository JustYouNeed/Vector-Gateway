# include "lwip_app_httpserver.h"
# include "ff.h"

const uint8_t html[] = 
	"<html>\
	 <head><title> Vector Gateway Control Web </title></head>\
	 <center><p> Version V1.0.0 </center>\
	 </html>";


static err_t http_server_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	char *data = NULL;
	uint8_t res;
	FIL file;
	UINT br;
	uint8_t *buff = (uint8_t*)bsp_mem_Malloc(SRAMIN, sizeof(uint8_t)*1024);
	
	if(p != NULL)
	{
		tcp_recved(pcb, p->tot_len);
		data = p->payload;
		if((p->len >= 3) && data[0] == 'G' && data[1] == 'E' && data[2] == 'T')  /* HTTP GET���󣬷�����ҳ */
		{
			printf("GET Request\r\n");
			res = f_open(&file, "1:/WEB/index.html", FA_OPEN_EXISTING | FA_READ);
			if(res == FR_OK)
			{
				while(1)
				{
					res = f_read(&file, buff, 1024, &br);
					if(br == 0 || res != FR_OK) break;
					tcp_write(pcb, buff, br, 1);
				}
			}
			else 
			{
				bsp_mem_Free(SRAMIN, buff);
				return tcp_close(pcb);
			}
		}
		else
		{
			printf("Request Error\r\n");
		}
		pbuf_free(p);  /* ���ݴ�������ͷ��ڴ� */
		tcp_close(pcb);
	}
	else if(err == ERR_OK)
	{
		return tcp_close(pcb);
	}
	return ERR_OK;
}

static err_t http_server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
	tcp_recv(pcb, http_server_recv);  /* ����ƿ���ע����ջص����� */
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

