/**
  ******************************************************************************
  * File Name:
  * Author:   
  * Version:  
  * Date:
  * Brief:
  ******************************************************************************
  * History
  *
  *
  *
  *
  *
  *
  *
  *
  ******************************************************************************
  */


/*
  ******************************************************************************
  *                              INCLUDE FILES
  ******************************************************************************
*/
# include "app_w25qxx.h"
# include "bsp_spi.h"

uint16_t W25QXX_TYPE = W25Q128;
uint8_t FLASH_RX_BUFF[4096];
uint8_t FLASH_TX_BUFF[4096];

/*
*********************************************************************************************************
*                                          app_flash_Config
*
* Description: W25Q128 Flash芯片初始化
*             
* Arguments  : 无
*
* Reutrn     : 无
*
* Note(s)    : 无
*********************************************************************************************************
*/
void app_flash_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
//	FLASH_RX_BUFF = (uint8_t *)bsp_mem_Malloc(SRAMIN, sizeof(uint8_t) * 4096);
//	FLASH_TX_BUFF = (uint8_t *)bsp_mem_Malloc(SRAMIN, sizeof(uint8_t) * 4096);
	
	FLASH_CS = 1;
	
	bsp_spi_Config(SPI3);
	bsp_spi_SetSpeed(SPI3, SPI_BAUDRATEPRESCALER_2);
	W25QXX_TYPE = app_flash_ReadID();
	os_printf("Device ID:%d\r\n", W25QXX_TYPE);
}


/*
*********************************************************************************************************
*                                          app_flash_ReadID
*
* Description: 读取Flash芯片ID，可通过读取ID值判断是否初始化成功
*             
* Arguments  : 无
*
* Reutrn     : 芯片ID值
*
* Note(s)    : 无
*********************************************************************************************************
*/
uint16_t app_flash_ReadID(void)
{
	uint16_t id = 0x00;
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = 0x90;
	FLASH_TX_BUFF[cnt++] = 0x00;
	FLASH_TX_BUFF[cnt++] = 0x00;
	FLASH_TX_BUFF[cnt++] = 0x00;
	FLASH_TX_BUFF[cnt++] = 0xff;
	FLASH_TX_BUFF[cnt++] = 0xff;
	
	FLASH_CS = 0;
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	FLASH_CS = 1;
	
	os_printf("%x%x\r\n",FLASH_RX_BUFF[4], FLASH_RX_BUFF[5]);
	id = FLASH_RX_BUFF[4] << 8 | FLASH_RX_BUFF[5];
	return id;
}



/*
*********************************************************************************************************
*                                          app_flash_ReadSR
*
* Description: 读取Flash芯片寄存器
*             
* Arguments  : 无
*
* Reutrn     : 寄存器值
*
* Note(s)    : 无
*********************************************************************************************************
*/
uint8_t app_flash_ReadSR(void)
{
	uint8_t byte = 0x0;
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_ReadStatusReg;
	FLASH_TX_BUFF[cnt++] = 0xff;
	
	FLASH_CS = 0;
	bsp_spi_Transmit(SPI3, &byte, FLASH_TX_BUFF, cnt);
	FLASH_CS = 1;

	return byte;
}


/*
*********************************************************************************************************
*                                       app_flash_WriteSR   
*
* Description: 写Flash寄存器值
*             
* Arguments  : sr:要写入的值
*
* Reutrn     : 无
*
* Note(s)    : 无
*********************************************************************************************************
*/
void app_flash_WriteSR(uint8_t sr)
{
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_WriteStatusReg;
	FLASH_TX_BUFF[cnt++] = sr;
	FLASH_CS = 0;
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	FLASH_CS = 1;
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

void app_flash_WriteCmd(FunctionalState state)
{
	uint16_t cnt = 0;
	
	if(state == DISABLE)
		FLASH_TX_BUFF[cnt++] = W25X_WriteDisable;
	else if(state == ENABLE)
		FLASH_TX_BUFF[cnt++] = W25X_WriteEnable;
	
	FLASH_CS = 0;
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	FLASH_CS = 1;
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
void app_flash_WritePage(uint8_t *buff, uint32_t addr, uint16_t len)
{
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_PageProgram;	//发送写页命令   
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 16);	//发送24bit地址 
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 8);
	FLASH_TX_BUFF[cnt++] = (uint8_t)addr;
	
	app_flash_WriteCmd(ENABLE);                  //SET WEL 
	FLASH_CS = 0;                            //使能器件   
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, buff, len);
	FLASH_CS=1;                            //取消片选 
	app_flash_WaitBusy();					   //等待写入结束
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
void app_flash_WriteNoCheck(uint8_t *buff, uint32_t addr, uint16_t len)
{
	uint16_t pageremain;	   
	pageremain = 256 - addr %256; //单页剩余的字节数		 	    
	if(len <= pageremain)pageremain=len;//不大于256个字节
	while(1)
	{	   
		app_flash_WritePage(buff, addr, pageremain);
		if(len == pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			buff += pageremain;
			addr += pageremain;	

			len -= pageremain;			  //减去已经写入了的字节数
			if(len > 256) pageremain = 256; //一次可以写入256个字节
			else pageremain = len; 	  //不够256个字节了
		}
	}
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
uint8_t app_flash_Read(uint8_t *buff, uint32_t addr, uint16_t len)
{
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_ReadData;	
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 16);	//发送24bit地址 
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 8);
	FLASH_TX_BUFF[cnt++] = (uint8_t)addr;
	
	FLASH_CS = 0;                            //使能器件   
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	
	bsp_spi_Transmit(SPI3, buff, FLASH_TX_BUFF, len);

	FLASH_CS=1;  
	
	return RES_OK;
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
uint8_t W25QXX_BUF[4096];
uint8_t app_flash_Write(uint8_t *buff, uint32_t addr, uint16_t len)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;  
	
 	secpos = addr/4096;//扇区地址  
	secoff = addr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
	
 	if(len <= secremain) secremain = len;//不大于4096个字节
	while(1) 
	{	
		app_flash_Read(W25QXX_BUF, secpos*4096, 4096);//读出整个扇区的内容
		for(i = 0; i < secremain; i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i] != 0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			app_flash_EraseSector(secpos);//擦除这个扇区
			for(i = 0; i < secremain; i++)	   //复制
			{
				W25QXX_BUF[i+secoff] = buff[i];	  
			}
			app_flash_WriteNoCheck(W25QXX_BUF, secpos*4096,4096);//写入整个扇区  

		}else app_flash_WriteNoCheck(buff, addr, secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(len == secremain)break;//写入结束了
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

			buff += secremain;  //指针偏移
			addr += secremain;//写地址偏移	   
			len -= secremain;				//字节数递减
			if(len > 4096)secremain = 4096;	//下一个扇区还是写不完
			else secremain = len;			//下一个扇区可以写完了
		}	 
	}
	return RES_OK;
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
void app_flash_EraseChip(void)
{
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_ChipErase;
	
	app_flash_WriteCmd(ENABLE);                  //SET WEL 
	app_flash_WaitBusy();   
	
	FLASH_CS = 0;                            //使能器件   
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);        //发送片擦除命令  
	FLASH_CS=1;                            //取消片选     	      
	
	app_flash_WaitBusy();   				   //等待芯片擦除结束
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
void app_flash_EraseSector(uint32_t dstAddr)
{
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_SectorErase;
	FLASH_TX_BUFF[cnt++] = (uint8_t)((dstAddr)>>16);
	FLASH_TX_BUFF[cnt++] = (uint8_t)((dstAddr)>>8);
	FLASH_TX_BUFF[cnt++] = (uint8_t)dstAddr;
	
	dstAddr*=4096;
	app_flash_WriteCmd(ENABLE);                  //SET WEL 	 
	app_flash_WaitBusy();   
	
	FLASH_CS=0;                            //使能器件  
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	FLASH_CS=1;                            //取消片选     	      
	app_flash_WaitBusy();   				   //等待擦除完成
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
void app_flash_WaitBusy(void)
{
	while((app_flash_ReadSR() & 0x01)==0x01);   // 等待BUSY位清空
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
void app_flash_PowerCmd(FunctionalState state)
{
	uint16_t cnt = 0;
	
	if(state == DISABLE)
		FLASH_TX_BUFF[cnt++] = W25X_PowerDown;
	else if(state == ENABLE)
		FLASH_TX_BUFF[cnt++] = W25X_ReleasePowerDown;
	
	FLASH_CS = 0;
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	FLASH_CS = 1;
	bsp_tim_DelayUs(3);
}
