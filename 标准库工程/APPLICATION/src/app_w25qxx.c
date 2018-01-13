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
	
	bsp_GPIOClcokCmd(GPIOA);
	bsp_GPIOClcokCmd(GPIOB);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	FLASH_CS = 1;
	
	bsp_spiConfig(SPI3);
	bsp_spiSetSpeed(SPI3, SPI_BaudRatePrescaler_2);
	
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
	FLASH_CS = 0;
	bsp_spiReadWriteByte(SPI3, 0x90);
	bsp_spiReadWriteByte(SPI3, 0x00);
	bsp_spiReadWriteByte(SPI3, 0x00);
	bsp_spiReadWriteByte(SPI3, 0x00);
	
	id |= bsp_spiReadWriteByte(SPI3, 0xff)<<8;
	id |= bsp_spiReadWriteByte(SPI3, 0xff);
	
	FLASH_CS = 1;
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
	
	FLASH_CS = 0;
	bsp_spiReadWriteByte(SPI3, W25X_ReadStatusReg);
	byte = bsp_spiReadWriteByte(SPI3, 0xff);
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
	FLASH_CS = 0;
	bsp_spiReadWriteByte(SPI3, W25X_WriteStatusReg);
	bsp_spiReadWriteByte(SPI3, sr);
	FLASH_CS = 1;
}



/*
*********************************************************************************************************
*                                        app_flash_WriteCmd  
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
	FLASH_CS = 0;
	if(state == DISABLE)
		bsp_spiReadWriteByte(SPI3, W25X_WriteDisable);
	else if(state == ENABLE)
		bsp_spiReadWriteByte(SPI3, W25X_WriteEnable);
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
	u16 i;  
	app_flash_WriteCmd(ENABLE);                  //SET WEL 
	FLASH_CS = 0;                            //使能器件   
	bsp_spiReadWriteByte(SPI3, W25X_PageProgram);      //发送写页命令   
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>16)); //发送24bit地址    
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>8));   
	bsp_spiReadWriteByte(SPI3, (u8)addr);   
	for(i=0;i<len;i++)bsp_spiReadWriteByte(SPI3,buff[i]);//循环写数  
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
	u16 pageremain;	   
	pageremain=256-addr%256; //单页剩余的字节数		 	    
	if(len<=pageremain)pageremain=len;//不大于256个字节
	while(1)
	{	   
		app_flash_WritePage(buff,addr,pageremain);
		if(len==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			buff+=pageremain;
			addr+=pageremain;	

			len-=pageremain;			  //减去已经写入了的字节数
			if(len>256)pageremain=256; //一次可以写入256个字节
			else pageremain=len; 	  //不够256个字节了
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
void app_flash_Read(uint8_t *buff, uint32_t addr, uint16_t len)
{
	u16 i;   										    
	FLASH_CS = 0;                            //使能器件   
	bsp_spiReadWriteByte(SPI3, W25X_ReadData);         //发送读取命令   
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>16));  //发送24bit地址    
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>8));   
	bsp_spiReadWriteByte(SPI3, (u8)addr);   
	for(i=0;i<len;i++)
	{ 
			buff[i]=bsp_spiReadWriteByte(SPI3, 0XFF);   //循环读数  
	}
	FLASH_CS=1;  
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
void app_flash_Write(uint8_t *buff, uint32_t addr, uint16_t len)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
	W25QXX_BUF = (uint8_t *)bsp_mem_Malloc(SRAMIN, 4096);
	
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

	bsp_mem_Free(SRAMIN, W25QXX_BUF);
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
	app_flash_WriteCmd(ENABLE);                  //SET WEL 
	app_flash_WaitBusy();   
	FLASH_CS = 0;                            //使能器件   
	bsp_spiReadWriteByte(SPI3, W25X_ChipErase);        //发送片擦除命令  
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
	dstAddr*=4096;
	app_flash_WriteCmd(ENABLE);                  //SET WEL 	 
	app_flash_WaitBusy();   
	FLASH_CS=0;                            //使能器件   
	bsp_spiReadWriteByte(SPI3, W25X_SectorErase);      //发送扇区擦除指令 
	bsp_spiReadWriteByte(SPI3, (u8)((dstAddr)>>16));  //发送24bit地址    
	bsp_spiReadWriteByte(SPI3, (u8)((dstAddr)>>8));   
	bsp_spiReadWriteByte(SPI3, (u8)dstAddr);  
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
	FLASH_CS = 0;
	if(state == DISABLE)
		bsp_spiReadWriteByte(SPI3, W25X_PowerDown);
	else if(state == ENABLE)
		bsp_spiReadWriteByte(SPI3, W25X_ReleasePowerDown);
	FLASH_CS = 1;
	bsp_tim_DelayUs(3);
}
