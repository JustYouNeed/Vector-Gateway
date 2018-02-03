# include "lan_eth.h"


err_t lan_eth_init(struct netif *netif)
{
	#ifdef CHECKSUM_BY_HARDWARE
	int i; 
#endif
  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  netif->hwaddr[0] = lwip_info.mac[0];
	netif->hwaddr[1] = lwip_info.mac[1];
	netif->hwaddr[2] = lwip_info.mac[2];
	netif->hwaddr[3] = lwip_info.mac[3];
	netif->hwaddr[4] = lwip_info.mac[4];
	netif->hwaddr[5] = lwip_info.mac[5];

  /* maximum transfer unit */
  netif->mtu = 1500;
  
  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
 
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr); //向STM32F4的MAC地址寄存器中写入MAC地址
	ETH_DMATxDescChainInit(DMATxDescTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDescTab, Rx_Buff, ETH_RXBUFNB);
#ifdef CHECKSUM_BY_HARDWARE 	//使用硬件帧校验
	for(i=0;i<ETH_TXBUFNB;i++)	//使能TCP,UDP和ICMP的发送帧校验,TCP,UDP和ICMP的接收帧校验在DMA中配置了
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDescTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}
#endif
  /* Do whatever else is needed to initialize interface. */  
	ETH_Start();
	return ERR_OK;
}


err_t lan_eth_output(struct netif *netif, struct pbuf *p)
{
	err_t res;
	int i = 0;
	struct pbuf *q = NULL;
	uint8_t *buff = (u8_t *)ETH_GetCurTxBuff();
	
	for(q = p; q != NULL; q = q->next)
	{
		memcpy((u8_t *)&buff[i], q->payload, q->len);
		i += q->len;
	}
	res = ETH_Tx_Packet(i);
	if(ETH_ERROR == res) return ERR_MEM;
  return ERR_OK;
}


struct pbuf *lan_eth_input(struct netif *netif)
{
	struct pbuf *p = NULL, *q = NULL;
	u16_t len;
	u8 * buff;
	int i = 0;
	
	FrameTypeDef frame;
	frame = ETH_Rx_Packet();
	len = frame.length;
	buff = (u8 *)frame.buffer;
	
	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	if(NULL != p)
	{
		for(q = p; q != NULL; q = q->next)
		{
			memcpy((u8_t *)q->payload, (u8_t *)&buff[i], q->len);
			i += q->len;
		}
	}
	frame.descriptor->Status = ETH_DMARxDesc_OWN;
	if((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
	{
		ETH->DMASR = ETH_DMASR_RBUS;
		ETH->DMARPDR = 0;
	}
  return p;  
}