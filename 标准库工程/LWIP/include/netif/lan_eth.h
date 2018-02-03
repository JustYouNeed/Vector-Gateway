# ifndef __LAN_ETH_H
# define __LAN_ETH_H

# include "bsp.h"
# include "lwip/opt.h"
# include "bsp_lan8720.h"
# include "netif/ethernetif.h" 
# include "lwip_comm.h"
# include "includes.h"

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"

err_t lan_eth_init(struct netif *netif);
err_t lan_eth_output(struct netif *netif, struct pbuf *p);
struct pbuf *lan_eth_input(struct netif *netif);


# endif

