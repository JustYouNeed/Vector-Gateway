# ifndef __LWIP_USART_H
# define __LWIP_USART_H

# include "bsp.h"
# include "lwip/opt.h"
# include "netif/ethernetif.h" 
# include "lwip_comm.h"
# include "includes.h"
# include "bsp_lan.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"

typedef struct 
{
	uint8_t frame[2];
	uint8_t funcode;
}Header_str;

typedef struct
{
	Header_str header;
	struct pbuf p_buf;
}usart_eth;

err_t usart_eth_init(struct netif *netif);
err_t usart_eth_output(struct netif *netif, struct pbuf *p);
struct pbuf * usart_eth_input(struct netif *netif);

# endif

