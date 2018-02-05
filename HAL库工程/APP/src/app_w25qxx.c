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
	
	FLASH_TX_BUFF[cnt++] = W25X_PageProgram;	//����дҳ����   
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 16);	//����24bit��ַ 
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 8);
	FLASH_TX_BUFF[cnt++] = (uint8_t)addr;
	
	app_flash_WriteCmd(ENABLE);                  //SET WEL 
	FLASH_CS = 0;                            //ʹ������   
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
	
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, buff, len);
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
	uint16_t pageremain;	   
	pageremain = 256 - addr %256; //��ҳʣ����ֽ���		 	    
	if(len <= pageremain)pageremain=len;//������256���ֽ�
	while(1)
	{	   
		app_flash_WritePage(buff, addr, pageremain);
		if(len == pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			buff += pageremain;
			addr += pageremain;	

			len -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(len > 256) pageremain = 256; //һ�ο���д��256���ֽ�
			else pageremain = len; 	  //����256���ֽ���
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
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 16);	//����24bit��ַ 
	FLASH_TX_BUFF[cnt++] = (uint8_t)((addr) >> 8);
	FLASH_TX_BUFF[cnt++] = (uint8_t)addr;
	
	FLASH_CS = 0;                            //ʹ������   
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
	
	FLASH_CS = 0;                            //ʹ������   
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);        //����Ƭ��������  
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
	uint16_t cnt = 0;
	
	FLASH_TX_BUFF[cnt++] = W25X_SectorErase;
	FLASH_TX_BUFF[cnt++] = (uint8_t)((dstAddr)>>16);
	FLASH_TX_BUFF[cnt++] = (uint8_t)((dstAddr)>>8);
	FLASH_TX_BUFF[cnt++] = (uint8_t)dstAddr;
	
	dstAddr*=4096;
	app_flash_WriteCmd(ENABLE);                  //SET WEL 	 
	app_flash_WaitBusy();   
	
	FLASH_CS=0;                            //ʹ������  
	bsp_spi_Transmit(SPI3, FLASH_RX_BUFF, FLASH_TX_BUFF, cnt);
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
