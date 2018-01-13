/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

# include "lwip/opt.h"
# include "netif/ethernetif.h" 
# include "lwip_comm.h"
# include "includes.h"
# include "bsp_lan8720.h"
#if 1 /* don't build, this is only a skeleton, see previous comment */

#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#include "netif/ppp_oe.h"


extern OS_MUTEX LWIP_Mutex;
extern OS_SEM		LWIP_Input;


/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static err_t low_level_init(struct netif *netif)
{
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	
	netif->hwaddr[0] = lwip_info.mac[0];
	netif->hwaddr[1] = lwip_info.mac[1];
	netif->hwaddr[2] = lwip_info.mac[2];
	netif->hwaddr[3] = lwip_info.mac[3];
	netif->hwaddr[4] = lwip_info.mac[4];
	netif->hwaddr[5] = lwip_info.mac[5];
	
	netif->mtu = 1500;
	
	netif->flags = NETIF_FLAG_BROADCAST | 
								 NETIF_FLAG_ETHARP | 
								 NETIF_FLAG_LINK_UP;
	
	HAL_ETH_DMATxDescListInit(&ETH_Handler,DMATxDescTab,Tx_Buff,ETH_TXBUFNB);//初始化发送描述符
	HAL_ETH_DMARxDescListInit(&ETH_Handler,DMARxDescTab,Rx_Buff,ETH_RXBUFNB);//初始化接收描述符
	HAL_ETH_Start(&ETH_Handler); //开启MAC和DMA				
	return ERR_OK;
	
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
	err_t errval;
	struct pbuf *q;
	uint8_t *buffer=(uint8_t *)(ETH_Handler.TxDesc->Buffer1Addr);
	__IO ETH_DMADescTypeDef *DmaTxDesc;
	uint32_t framelength = 0;
	uint32_t bufferoffset = 0;
	uint32_t byteslefttocopy = 0;
	uint32_t payloadoffset = 0;

	DmaTxDesc = ETH_Handler.TxDesc;
	bufferoffset = 0;

	DISABLE_INT();
	//从pbuf中拷贝要发送的数据
	for(q=p;q!=NULL;q=q->next)
	{
			//判断此发送描述符是否有效，即判断此发送描述符是否归以太网DMA所有
			if((DmaTxDesc->Status&ETH_DMATXDESC_OWN)!=(uint32_t)RESET)
			{
					errval=ERR_USE;
					goto error;             //发送描述符无效，不可用
			}
			byteslefttocopy=q->len;     //要发送的数据长度
			payloadoffset=0;
 
			//将pbuf中要发送的数据写入到以太网发送描述符中，有时候我们要发送的数据可能大于一个以太网
			//描述符的Tx Buffer，因此我们需要分多次将数据拷贝到多个发送描述符中
			while((byteslefttocopy+bufferoffset)>ETH_TX_BUF_SIZE )
			{
					//将数据拷贝到以太网发送描述符的Tx Buffer中
					memcpy((uint8_t*)((uint8_t*)buffer+bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),(ETH_TX_BUF_SIZE-bufferoffset));
					//DmaTxDsc指向下一个发送描述符
					DmaTxDesc=(ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);
					//检查新的发送描述符是否有效
					if((DmaTxDesc->Status&ETH_DMATXDESC_OWN)!=(uint32_t)RESET)
					{
							errval = ERR_USE;
							goto error;     //发送描述符无效，不可用
					}
					buffer=(uint8_t *)(DmaTxDesc->Buffer1Addr);   //更新buffer地址，指向新的发送描述符的Tx Buffer
					byteslefttocopy=byteslefttocopy-(ETH_TX_BUF_SIZE-bufferoffset);
					payloadoffset=payloadoffset+(ETH_TX_BUF_SIZE-bufferoffset);
					framelength=framelength+(ETH_TX_BUF_SIZE-bufferoffset);
					bufferoffset=0;
			}
			//拷贝剩余的数据
			memcpy( (uint8_t*)((uint8_t*)buffer+bufferoffset),(uint8_t*)((uint8_t*)q->payload+payloadoffset),byteslefttocopy );
			bufferoffset=bufferoffset+byteslefttocopy;
			framelength=framelength+byteslefttocopy;
	}
	//当所有要发送的数据都放进发送描述符的Tx Buffer以后就可发送此帧了
	HAL_ETH_TransmitFrame(&ETH_Handler,framelength);
	errval = ERR_OK;
error:    
	//发送缓冲区发生下溢，一旦发送缓冲区发生下溢TxDMA会进入挂起状态
	if((ETH_Handler.Instance->DMASR&ETH_DMASR_TUS)!=(uint32_t)RESET)
	{
			//清除下溢标志
			ETH_Handler.Instance->DMASR = ETH_DMASR_TUS;
			//当发送帧中出现下溢错误的时候TxDMA会挂起，这时候需要向DMATPDR寄存器
			//随便写入一个值来将其唤醒，此处我们写0
			ETH_Handler.Instance->DMATPDR=0;
	}
	ENABLE_INT();
	return errval;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
	struct pbuf *p = NULL;
    struct pbuf *q;
    uint16_t len;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset=0;
    uint32_t payloadoffset=0;
    uint32_t byteslefttocopy=0;
    uint32_t i=0;
  
    if(HAL_ETH_GetReceivedFrame(&ETH_Handler)!=HAL_OK)  //判断是否接收到数据
    return NULL;
    
    DISABLE_INT();
    len=ETH_Handler.RxFrameInfos.length;                //获取接收到的以太网帧长度
    buffer=(uint8_t *)ETH_Handler.RxFrameInfos.buffer;  //获取接收到的以太网帧的数据buffer
  
    if(len>0) p=pbuf_alloc(PBUF_RAW,len,PBUF_POOL);     //申请pbuf
    if(p!=NULL)                                        //pbuf申请成功
    {
        dmarxdesc=ETH_Handler.RxFrameInfos.FSRxDesc;    //获取接收描述符链表中的第一个描述符
        bufferoffset = 0;
        for(q=p;q!=NULL;q=q->next)                      
        {
            byteslefttocopy=q->len;                  
            payloadoffset=0;
            //将接收描述符中Rx Buffer的数据拷贝到pbuf中
            while((byteslefttocopy+bufferoffset)>ETH_RX_BUF_SIZE )
            {
                //将数据拷贝到pbuf中
                memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer+bufferoffset),(ETH_RX_BUF_SIZE-bufferoffset));
                 //dmarxdesc向下一个接收描述符
                dmarxdesc=(ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                //更新buffer地址，指向新的接收描述符的Rx Buffer
                buffer=(uint8_t *)(dmarxdesc->Buffer1Addr);
 
                byteslefttocopy=byteslefttocopy-(ETH_RX_BUF_SIZE-bufferoffset);
                payloadoffset=payloadoffset+(ETH_RX_BUF_SIZE-bufferoffset);
                bufferoffset=0;
            }
            //拷贝剩余的数据
            memcpy((uint8_t*)((uint8_t*)q->payload+payloadoffset),(uint8_t*)((uint8_t*)buffer+bufferoffset),byteslefttocopy);
            bufferoffset=bufferoffset+byteslefttocopy;
        }
    }    
    //释放DMA描述符
    dmarxdesc=ETH_Handler.RxFrameInfos.FSRxDesc;
    for(i=0;i<ETH_Handler.RxFrameInfos.SegCount; i++)
    {  
        dmarxdesc->Status|=ETH_DMARXDESC_OWN;       //标记描述符归DMA所有
        dmarxdesc=(ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }
    ETH_Handler.RxFrameInfos.SegCount =0;           //清除段计数器
    if((ETH_Handler.Instance->DMASR&ETH_DMASR_RBUS)!=(uint32_t)RESET)  //接收缓冲区不可用
    {
        //清除接收缓冲区不可用标志
        ETH_Handler.Instance->DMASR = ETH_DMASR_RBUS;
        //当接收缓冲区不可用的时候RxDMA会进去挂起状态，通过向DMARPDR写入任意一个值来唤醒Rx DMA
        ETH_Handler.Instance->DMARPDR=0;
    }
    ENABLE_INT();
    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
#endif /* 0 */
err_t ethernetif_input(struct netif *netif)
{
	err_t err;
  struct pbuf *p;

  /* move received packet into a new pbuf */
  p = low_level_input(netif);
    
  /* no packet could be read, silently ignore this */
  if (p == NULL) return 0;
    
  /* entry point to the LwIP stack */
  err = netif->input(p, netif);
    
  if (err != ERR_OK)
  {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
    pbuf_free(p);
    p = NULL;    
  }
	return err;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
#if LWIP_NETIF_HOSTNAME			//LWIP_NETIF_HOSTNAME 
	netif->hostname="VectorGateway";  	//初始化名称
#endif 
	netif->name[0]=IFNAME0; 	//初始化变量netif的name字段
	netif->name[1]=IFNAME1; 	//在文件外定义这里不用关心具体值
	netif->output=etharp_output;//IP层发送数据包函数
	netif->linkoutput=low_level_output;//ARP模块发送数据包函数
	low_level_init(netif); 		//底层硬件初始化函数
	return ERR_OK;
}


