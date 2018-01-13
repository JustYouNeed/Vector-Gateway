# ifndef __LWIP_COMM_H__
# define __LWIP_COMM_H__

# include "bsp.h"
# include "bsp_malloc.h"
# include "bsp_lan8720.h"
# include "netif.h"

#define LWIP_DHCP_MAX_TRIES		4

typedef struct 
{
	uint8_t mac[6];
	uint8_t remoteip[4];
	uint8_t localip[4];
	uint8_t netmask[4];
	uint8_t gateway[4];
	
	__IO uint8_t dhcpStatus;
}lwip_dev;

extern lwip_dev lwip_info;

void lwip_DHCPTask_Create(void);

uint8_t lwip_comm_mem_malloc(void);
void lwip_comm_mem_free(void);

void lwip_default_info_set(lwip_dev *ip_info);
uint8_t lwip_config(void);

void lwip_pkt_handle(void);

void lwip_periodic_handle(void);
# endif


