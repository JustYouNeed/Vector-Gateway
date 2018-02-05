# include "lwip_app_telnet.h"
# include "lwip/tcp.h"

# define LOGIN_INFO "Please input Password to login:"
# define PASSWORD	"19960114"


char * cmd[] = 
{
	"greeting",
	"date",
	"systick",
	"version",
	"quit",
	"help",
};


int telnet_process_cmd(struct tcp_pcb *pcb, char *req)
{
	char res_buff[100] = {0};
	int slen = 0;
	int close_flag = 0;
	
	if(strcmp(req, cmd[0]) == 0)
	{
		slen = sprintf(res_buff, "Gateway Telnet Server\r\n");
	}else 	if(strcmp(req, cmd[1]) == 0)
	{
		slen = sprintf(res_buff, "Gateway Telnet Server\r\n");
	}else 	if(strcmp(req, cmd[2]) == 0)
	{
//	slen = sprintf(res_buff, "Current systick is [%u]..\r\n", sys_now());
	}else 	if(strcmp(req, cmd[3]) == 0)
	{
		slen = sprintf(res_buff, "Version:1.0.0V\r\n");
	}else 	if(strcmp(req, cmd[4]) == 0)
	{
		slen = sprintf(res_buff, "The Connection will shutdown..\r\n");
		close_flag = 1;
	}else 	if(strcmp(req, cmd[5]) == 0)
	{
		slen = sprintf(res_buff, "Gateway Telnet Server\r\n");
	}else 
	{
		slen = sprintf(res_buff, "Command not support..\r\n");
	}
	
	tcp_write(pcb, res_buff, slen, TCP_WRITE_FLAG_COPY);
	
	slen = sprintf(res_buff, "Gateway Telnet->");
	tcp_write(pcb, res_buff, slen, TCP_WRITE_FLAG_COPY);
	
	return close_flag;
}

int telnet_input(struct tcp_pcb *pcb, telnet_conn *conn_arg, struct pbuf *p)
{
	int slen = 0;
	char buff[20];
//	u8_t i = 0;
	u16_t len = p->len;
//	u8_t *dtab = (u8_t *)bsp_mem_Malloc(SRAMIN, sizeof(u8_t)*20);
//	memcpy(dtab, p->payload, len);
	u8_t *dtab = (unsigned char *)p->payload;
//	printf("payload:%s,len:%d\r\n",dtab, len);
	if((len == 2) && (*dtab == 0x0d) && (*(dtab + 1) == 0x0a))
	{
		conn_arg->cmd[conn_arg->bytes_len] = 0x00;
		return 1;
	}
	else if((len == 1) && (*dtab >= 0x20) && (*dtab <= 0x7e))
	{
		conn_arg->cmd[conn_arg->bytes_len] = *dtab;
		if(conn_arg->bytes_len < (30 - 1))
		{
			conn_arg->bytes_len ++;
		}
	}
	else if((len == 1) && (*dtab == 0x08) && (conn_arg->bytes_len > 0))
	{
		conn_arg->bytes_len --;
		slen = sprintf(buff, " \b \b");
		tcp_write(pcb, buff, slen, TCP_WRITE_FLAG_COPY);
	}
	else if((len == 1) && *dtab == 0x08)
	{
		conn_arg->bytes_len = 0;
		slen = sprintf(buff, ">");
		tcp_write(pcb, buff, slen, TCP_WRITE_FLAG_COPY);
	}
//	bsp_mem_Free(SRAMIN, dtab);
	return 0;
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
void telnet_server_close_conn(struct tcp_pcb *pcb)
{
	if(NULL == (void *)pcb->callback_arg)
	{
		mem_free((void *)pcb->callback_arg);
		tcp_arg(pcb, NULL);
	}
	
	tcp_close(pcb);
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
static err_t telnet_server_recv(void *p_arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	telnet_conn *conn_arg = (telnet_conn *)p_arg;
	char sndbuff[50];
	int slen = 0;
	int ret = 0;
	if(NULL == conn_arg || (pcb->remote_port != conn_arg->cilent_port))
	{
		if(p) pbuf_free(p);
		return ERR_ARG;
	}
	
	if(p != NULL)
	{
		tcp_recved(pcb, p->tot_len);
		ret = telnet_input(pcb, conn_arg, p);
		if(ret == 1)
		{
			switch(conn_arg->state)
			{
				case TELNET_SETUP:
					//printf("cmd:%c%c%c%c%c\r\n",conn_arg->cmd[0], conn_arg->cmd[1], conn_arg->cmd[2], conn_arg->cmd[3]);
					if(strcmp(conn_arg->cmd, PASSWORD) == 0)
					{
						slen = sprintf(sndbuff, "##Welcome to Gateway Telnet##\r\n");
						tcp_write(pcb, sndbuff, slen, TCP_WRITE_FLAG_COPY);
						slen = sprintf(sndbuff, "##Create by Vector...      ##\r\n");
						tcp_write(pcb, sndbuff, slen, TCP_WRITE_FLAG_COPY);
						slen = sprintf(sndbuff, "##quit:退出    help:帮助   ##\r\n");
						tcp_write(pcb, sndbuff, slen, TCP_WRITE_FLAG_COPY);
						slen = sprintf(sndbuff, "##Gateway Telnet->");
						tcp_write(pcb, sndbuff, slen, 1);
						
						conn_arg->state = TELNET_CONNECTED;
					}
					else
					{
						slen = sprintf(sndbuff, "##PASSWORD ERROR,Try again:##\r\n");
						tcp_write(pcb, sndbuff, slen, TCP_WRITE_FLAG_COPY);
					}
					
					memset(conn_arg->cmd, 0, 30);
					conn_arg->bytes_len = 0;
					break;
				case TELNET_CONNECTED:
					if(telnet_process_cmd(pcb, conn_arg->cmd) == 0)
					{
						memset(conn_arg->cmd, 0, 30);
						conn_arg->bytes_len = 0;
					}
					else
					{
						telnet_server_close_conn(pcb);
					}
					break;
				default:break;
			}
		}  /* 数据处理结束,释放数据包 */
		pbuf_free(p);	
	}
	else if(err == ERR_OK)
	{
		telnet_server_close_conn(pcb);
	}
	return ERR_OK;
}

static void telnet_server_conn_err(void *p_arg, err_t err)
{
	telnet_conn *conn_arg = (telnet_conn *)p_arg;
	
	if(!conn_arg) return ;
	printf("connection error\r\n");
	mem_free(p_arg);
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
static err_t telnet_server_accept(void *p_arg, struct tcp_pcb *pcb, err_t err)
{
	u32_t remote_ip;
	char greet[100];
	u8_t iptab[4];
	telnet_conn *conn_arg = NULL;
	remote_ip = pcb->remote_ip.addr;
	
	iptab[0] = (u8_t)(remote_ip >> 24);
	iptab[1] = (u8_t)(remote_ip >> 16);
	iptab[2] = (u8_t)(remote_ip >> 8);
	iptab[3] = (u8_t)(remote_ip >> 0);
	
	sprintf(greet, "Welcome to Telnet!you info -->[%d.%d.%d.%d:%d]\r\n", iptab[3], iptab[2], iptab[1], iptab[0], pcb->remote_port);
	
	conn_arg = mem_calloc(sizeof(telnet_conn), 1);
	if(!conn_arg) return ERR_MEM;
	
	conn_arg->state = TELNET_SETUP;
	conn_arg->cilent_port = pcb->remote_port;
	conn_arg->bytes_len = 0;
	memset(conn_arg->cmd, 0, 30);
	
	tcp_arg(pcb, conn_arg);
	
	tcp_err(pcb, telnet_server_conn_err);
	tcp_recv(pcb, telnet_server_recv);
	
	tcp_write(pcb, greet, strlen(greet), 1);
	tcp_write(pcb, LOGIN_INFO, strlen(LOGIN_INFO), 1);
	
	return ERR_OK;	
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
void lwip_app_telnetConfig(void)
{
	struct tcp_pcb *pcb;
	pcb = tcp_new();
	tcp_bind(pcb, IP_ADDR_ANY, TELNET_SERVER_PORT);
	pcb = tcp_listen(pcb);
	
	tcp_accept(pcb, telnet_server_accept);
}


