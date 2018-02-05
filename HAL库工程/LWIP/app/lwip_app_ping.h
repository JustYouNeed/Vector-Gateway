# ifndef __LWIP_APP_PING_H
# define __LWIP_APP_PING_H

# include "includes.h"
# include "lwip/ip_addr.h"
# include "lwip/netif.h"
# include "lwip/raw.h"
# include "lwip/icmp.h"

# define PING_DATA_SIZE		32
# define PING_ID 			0xafaf

typedef struct
{
	ip_addr_t ping_ip;
	struct raw_pcb *ping_pcb;
	
	u32_t ping_time;
	u32_t ping_send_time;
	u32_t ping_recv_time;
}_ping_dev;

void lwip_app_pingConfig(_ping_dev *ping);
void lwip_app_ping(_ping_dev *ping);
# endif

