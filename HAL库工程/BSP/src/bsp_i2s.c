# include "bsp_i2s.h"
# include "includes.h"



//�����ʼ��㹫ʽ:Fs=I2SxCLK/[256*(2*I2SDIV+ODD)]
//I2SxCLK=(HSE/pllm)*PLLI2SN/PLLI2SR
//һ��HSE=8Mhz 
//pllm:��Sys_Clock_Set���õ�ʱ��ȷ����һ����8
//PLLI2SN:һ����192~432 
//PLLI2SR:2~7
//I2SDIV:2~255
//ODD:0/1
//I2S��Ƶϵ����@pllm=8,HSE=8Mhz,��vco����Ƶ��Ϊ1Mhz
//���ʽ:������/10,PLLI2SN,PLLI2SR,I2SDIV,ODD
const uint16_t I2S_PSC_TBL[][5]=
{
	{800 ,256,5,12,1},		//8Khz������
	{1102,429,4,19,0},		//11.025Khz������ 
	{1600,213,2,13,0},		//16Khz������
	{2205,429,4, 9,1},		//22.05Khz������
	{3200,213,2, 6,1},		//32Khz������
	{4410,271,2, 6,0},		//44.1Khz������
	{4800,258,3, 3,1},		//48Khz������
	{8820,316,2, 3,1},		//88.2Khz������
	{9600,344,2, 3,1},  	//96Khz������
	{17640,361,2,2,0},  	//176.4Khz������ 
	{19200,393,2,2,0},  	//192Khz������
};  


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
void bsp_i2s_Config(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
	RCC->APB1ENR |= 1 << 14;  /*  ʹ��SPI2ʱ��  */
	
	RCC->APB1RSTR |= 1 << 14;		/*  ��λSPI2  */
	RCC->APB1RSTR &= ~(1 << 14);		/*  ������λ  */
	
	SPI2->I2SCFGR = 0;
	SPI2->I2SPR = 0x02;		/*  ��Ƶ������Ĭ��ֵ  */
	SPI2->I2SCFGR |= 1 << 11;		/*  ѡ��ΪI2Sģʽ  */
	SPI2->I2SCFGR |= (uint16_t)I2S_Mode << 8;		/*  ����I2S����ģʽ  */
	SPI1->I2SCFGR |= I2S_Standard << 4;		/*  I2S��׼����  */
	SPI2->I2SCFGR |= I2S_Clock_Polarity;		/*  ����ʱ�ӵ�ƽ����  */
	
	if(I2S_DataFormat)		/*  ������Ǳ�׼��16λ����  */
	{
		SPI2->I2SCFGR |= 1 << 0;		/*  ͨ����������Ϊ32λ  */
		I2S_DataFormat -= 1;
	}
	else
		SPI2->I2SCFGR |= 0 << 0;	/*  ͨ����������Ϊ16λ  */
	
	SPI2->I2SCFGR |= I2S_DataFormat << 1;	/*  I2S��׼����  */
	SPI2->CR2 |= 1 << 1;		/*  ʹ��SPI2����DMA  */
	SPI2->I2SCFGR |= 1 << 10;		/*  SPI2 I2Sʹ��  */
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
void bsp_i2s_ExtConfig(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat)
{
	I2S2ext->I2SCFGR=0;				//ȫ������Ϊ0
	I2S2ext->I2SPR=0X02;			//��Ƶ�Ĵ���ΪĬ��ֵ
	I2S2ext->I2SCFGR|=1<<11;		//ѡ��:I2Sģʽ
	I2S2ext->I2SCFGR|=(uint16_t)I2S_Mode<<8;	//I2S����ģʽ����
	I2S2ext->I2SCFGR|=I2S_Standard<<4;		//I2S��׼���� 
	I2S2ext->I2SCFGR|=I2S_Clock_Polarity<<3;		//����ʱ�ӵ�ƽ����
	if(I2S_DataFormat)						//�Ǳ�׼16λ����
	{
		I2S2ext->I2SCFGR|=1<<0;		//Channel����Ϊ32λ 
		I2S_DataFormat-=1;		
	}else I2S2ext->I2SCFGR|=0<<0;	//Channel����Ϊ16λ  
	I2S2ext->I2SCFGR|=I2S_DataFormat<<1;	//I2S��׼���� 
	I2S2ext->CR2|=1<<0;				//I2S2ext RX DMA����ʹ��.
	I2S2ext->I2SCFGR|=1<<10;		//I2S2ext I2S ENʹ��.	
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
uint8_t bsp_i2s_SampleRateSet(uint32_t samplerate)
{
	uint8_t i=0; 
	uint32_t tempreg=0;
	samplerate/=10;//��С10��   
	
	for(i=0;i<(sizeof(I2S_PSC_TBL)/10);i++)//�����Ĳ������Ƿ����֧��
	{
		if(samplerate==I2S_PSC_TBL[i][0])break;
	}
 
	RCC->CR &= ~(1 << 26);  /*  ����֮ǰ��Ҫ�ر�I2S  */
	
	if(i==(sizeof(I2S_PSC_TBL)/10))return 1;//�ѱ���Ҳ�Ҳ���
	
	tempreg |= (uint32_t)I2S_PSC_TBL[i][1] << 6;  /*  ����PLLI2SN  */
	tempreg |= (uint32_t)I2S_PSC_TBL[i][2] << 28;  /*  ����PLLI2SR  */
	RCC->PLLI2SCFGR = tempreg;   /*  ����I2S2CLK��ʱ��Ƶ��  */
	RCC->CR |= 1 << 26;					//����I2Sʱ��
	while((RCC->CR & 1 << 27) == 0);		//�ȴ�I2Sʱ�ӿ����ɹ�. 
	tempreg = I2S_PSC_TBL[i][3] << 0;	//����I2SDIV
	tempreg |= I2S_PSC_TBL[i][4] << 8;	//����ODDλ
	tempreg |= 1 << 9;					//ʹ��MCKOEλ,���MCK
	SPI2->I2SPR = tempreg;			//����I2SPR�Ĵ��� 
	return 0;
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
void bsp_i2s_TX_DMAConfig(uint8_t* buf0,uint8_t *buf1,uint16_t num)
{
	RCC->AHB1ENR |= 1 << 21;				/*  ʹ��DMA  */
	
	while(DMA1_Stream4->CR & 0x01);		/*  �ȴ�DMA Stream������  */
	
	DMA1->HIFCR |= 0x3d << (6 * 0);			/*  ���ͨ��4�����е��жϱ�־  */
	DMA1_Stream4->FCR = 0x00000021;		/*  ����ΪĬ��ֵ  */
	DMA1_Stream4->PAR = (uint32_t)&SPI2->DR;		/*  ���������ַΪSPI2->DR  */
	DMA1_Stream4->M0AR = (uint32_t)buf0;		/*  �ڴ��ַ1  */
	DMA1_Stream4->M1AR = (uint32_t)buf1;		/*  �ڴ��ַ2  */
	DMA1_Stream4->NDTR = num;			/*  �������ݳ���  */
	
	DMA1_Stream4->CR = 0;		/*  ��λ  */
	DMA1_Stream4->CR |= 1 << 6;		/*  ģʽΪ�洢��������  */
	DMA1_Stream4->CR |= 1 << 8;		/*  ѭ��ģʽ  */
	DMA1_Stream4->CR |= 0 << 9;		/*  ���������ģʽ  */
	DMA1_Stream4->CR |= 1 << 10;	/*  �洢������ģʽ  */
	DMA1_Stream4->CR |= 1 << 11;	/*  �������ݳ���Ϊ16λ  */
	DMA1_Stream4->CR |= 1 << 13;	/*  �洢������16λ  */
	DMA1_Stream4->CR |= 2 << 16;	/*  �����ȼ�  */
	DMA1_Stream4->CR |= 1 << 18;	/*  ˫����ģʽ  */
	DMA1_Stream4->CR |= 0 << 21;	/*  ����ͻ�����δ���  */
	DMA1_Stream4->CR |= 0 << 23;	/*  �洢��ͻ�����δ���  */
	DMA1_Stream4->CR |= 0 << 25;	/*  ѡ��ͨ��0 SPI2_TXͨ��  */
	DMA1_Stream4->FCR &= ~(1 << 2);		/*  ��ʹ��FIFOģʽ */
	DMA1_Stream4->FCR &= ~(3 << 0);		/*  ��FIFO����  */
	DMA1_Stream4->CR |= 1 << 4;		/*  ������������ж�  */
	
	bsp_nvic_Config(0, 0, DMA1_Stream4_IRQn, 2);
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
void bsp_i2s_PlayStart(void)
{
	DMA1_Stream4->CR |= 1 << 0;	/*  ����DMA����  */
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
void bsp_i2s_PlayStop(void)
{
	DMA1_Stream4->CR &= ~(1 << 0); 	/*  �ر�DMA����  */
}


void (*i2s_tx_callback)(void);	
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
//void DMA1_Stream4_IRQHandler(void)
//{    
//OSIntEnter();    
//if(DMA1->HISR & (1 << 5))   /*  DMA1_Stream4,������ɱ�־  */
//{ 
//	DMA1->HIFCR |= 1 << 5;
//			i2s_tx_callback();	//ִ�лص�����,��ȡ���ݵȲ����������洦��  
//}   
//OSIntExit(); 	
//} 

