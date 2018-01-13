# ifndef __LWIP_APP_TELNET_H
# define __LWIP_APP_TELNET_H


# include "includes.h"
# include "lwip/ip_addr.h"
# include "lwip/netif.h"
# include "lwip/raw.h"
# include "lwip/icmp.h"

enum TELNET_STATE
{
	TELNET_SETUP,
	TELNET_CONNECTED,
};

typedef struct
{
	uint8_t state;
	u16_t cilent_port;
	u16_t bytes_len;
	char cmd[30];
}telnet_conn;


# define TELNET_SERVER_PORT	23

void lwip_app_telnetConfig(void);

# endif

