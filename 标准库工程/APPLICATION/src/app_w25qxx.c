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
* Description: W25Q128 FlashоƬ��ʼ��
*             
* Arguments  : ��
*
* Reutrn     : ��
*
* Note(s)    : ��
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
* Description: ��ȡFlashоƬID����ͨ����ȡIDֵ�ж��Ƿ��ʼ���ɹ�
*             
* Arguments  : ��
*
* Reutrn     : оƬIDֵ
*
* Note(s)    : ��
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
* Description: ��ȡFlashоƬ�Ĵ���
*             
* Arguments  : ��
*
* Reutrn     : �Ĵ���ֵ
*
* Note(s)    : ��
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
* Description: дFlash�Ĵ���ֵ
*             
* Arguments  : sr:Ҫд���ֵ
*
* Reutrn     : ��
*
* Note(s)    : ��
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
	FLASH_CS = 0;                            //ʹ������   
	bsp_spiReadWriteByte(SPI3, W25X_PageProgram);      //����дҳ����   
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>16)); //����24bit��ַ    
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>8));   
	bsp_spiReadWriteByte(SPI3, (u8)addr);   
	for(i=0;i<len;i++)bsp_spiReadWriteByte(SPI3,buff[i]);//ѭ��д��  
	FLASH_CS=1;                            //ȡ��Ƭѡ 
	app_flash_WaitBusy();					   //�ȴ�д�����
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
	pageremain=256-addr%256; //��ҳʣ����ֽ���		 	    
	if(len<=pageremain)pageremain=len;//������256���ֽ�
	while(1)
	{	   
		app_flash_WritePage(buff,addr,pageremain);
		if(len==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			buff+=pageremain;
			addr+=pageremain;	

			len-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(len>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=len; 	  //����256���ֽ���
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
	FLASH_CS = 0;                            //ʹ������   
	bsp_spiReadWriteByte(SPI3, W25X_ReadData);         //���Ͷ�ȡ����   
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>16));  //����24bit��ַ    
	bsp_spiReadWriteByte(SPI3, (u8)((addr)>>8));   
	bsp_spiReadWriteByte(SPI3, (u8)addr);   
	for(i=0;i<len;i++)
	{ 
			buff[i]=bsp_spiReadWriteByte(SPI3, 0XFF);   //ѭ������  
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
	
 	secpos = addr/4096;//������ַ  
	secoff = addr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
	
 	if(len <= secremain) secremain = len;//������4096���ֽ�
	while(1) 
	{	
		app_flash_Read(W25QXX_BUF, secpos*4096, 4096);//������������������
		for(i = 0; i < secremain; i++)//У������
		{
			if(W25QXX_BUF[secoff+i] != 0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			app_flash_EraseSector(secpos);//�����������
			for(i = 0; i < secremain; i++)	   //����
			{
				W25QXX_BUF[i+secoff] = buff[i];	  
			}
			app_flash_WriteNoCheck(W25QXX_BUF, secpos*4096,4096);//д����������  

		}else app_flash_WriteNoCheck(buff, addr, secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(len == secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

			buff += secremain;  //ָ��ƫ��
			addr += secremain;//д��ַƫ��	   
			len -= secremain;				//�ֽ����ݼ�
			if(len > 4096)secremain = 4096;	//��һ����������д����
			else secremain = len;			//��һ����������д����
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
	FLASH_CS = 0;                            //ʹ������   
	bsp_spiReadWriteByte(SPI3, W25X_ChipErase);        //����Ƭ��������  
	FLASH_CS=1;                            //ȡ��Ƭѡ     	      
	app_flash_WaitBusy();   				   //�ȴ�оƬ��������
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
	FLASH_CS=0;                            //ʹ������   
	bsp_spiReadWriteByte(SPI3, W25X_SectorErase);      //������������ָ�� 
	bsp_spiReadWriteByte(SPI3, (u8)((dstAddr)>>16));  //����24bit��ַ    
	bsp_spiReadWriteByte(SPI3, (u8)((dstAddr)>>8));   
	bsp_spiReadWriteByte(SPI3, (u8)dstAddr);  
	FLASH_CS=1;                            //ȡ��Ƭѡ     	      
	app_flash_WaitBusy();   				   //�ȴ��������
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
		while((app_flash_ReadSR() & 0x01)==0x01);   // �ȴ�BUSYλ���
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
