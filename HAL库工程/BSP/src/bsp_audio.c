# include "bsp_audio.h"

I2C_Str audio_i2c;

//WM8978�Ĵ���ֵ������(�ܹ�58���Ĵ���,0~57),ռ��116�ֽ��ڴ�
//��ΪWM8978��IIC������֧�ֶ�����,�����ڱ��ر������мĴ���ֵ
//дWM8978�Ĵ���ʱ,ͬ�����µ����ؼĴ���ֵ,���Ĵ���ʱ,ֱ�ӷ��ر��ر���ļĴ���ֵ.
//ע��:WM8978�ļĴ���ֵ��9λ��,����Ҫ��uint16_t���洢. 
static uint16_t WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t bsp_audio_Config(void)
{
	uint8_t res = 0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF5_SPI2;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	bsp_audio_IICConfig();
	
	res = bsp_audio_WriteReg(0, 0);
	if(res) return 1;
	
	bsp_audio_WriteReg(1, 0X1B);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	bsp_audio_WriteReg(2, 0X1B0);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	bsp_audio_WriteReg(3, 0X6C);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	
	bsp_audio_WriteReg(6, 0);		//R6,MCLK���ⲿ�ṩ
	bsp_audio_WriteReg(43, 1<<4);	//R43,INVROUT2����,��������
	bsp_audio_WriteReg(47, 1<<8);	//R47����,PGABOOSTL,��ͨ��MIC���20������
	bsp_audio_WriteReg(48, 1<<8);	//R48����,PGABOOSTR,��ͨ��MIC���20������
	bsp_audio_WriteReg(49, 1<<1);	//R49,TSDEN,�������ȱ��� 
	bsp_audio_WriteReg(10, 1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
	bsp_audio_WriteReg(14, 1<<3);	//R14,ADC 128x������
	return 0;
}


/*
*********************************************************************************************************
*                                          bsp_audio_IICConfig
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_IICConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	GPIO_InitStructure.Pin = AUDIO_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(AUDIO_SDA_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = AUDIO_SCL_PIN;
	HAL_GPIO_Init(AUDIO_SCL_PORT, &GPIO_InitStructure);
	
	audio_i2c.read_sda = AUDIO_SDA_READ;
	audio_i2c.set_scl_high = AUDIO_SCL_HIGH;
	audio_i2c.set_scl_low = AUDIO_SCL_LOW;
	audio_i2c.set_sda_high = AUDIO_SDA_HIGH;
	audio_i2c.set_sda_in = AUDIO_SDA_IN;
	audio_i2c.set_sda_low = AUDIO_SDA_LOW;
	audio_i2c.set_sda_out = AUDIO_SDA_OUT;
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_ADDAConfig(uint8_t dacEn, uint8_t adcEn)
{
	uint16_t regval;
	regval=bsp_audio_ReadReg(3);	//��ȡR3
	if(dacEn)regval|=3<<0;		//R3���2��λ����Ϊ1,����DACR&DACL
	else regval&=~(3<<0);		//R3���2��λ����,�ر�DACR&DACL.
	bsp_audio_WriteReg(3,regval);	//����R3
	regval=bsp_audio_ReadReg(2);	//��ȡR2
	if(adcEn)regval|=3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
	else regval&=~(3<<0);		//R2���2��λ����,�ر�ADCR&ADCL.
	bsp_audio_WriteReg(2,regval);	//����R2	
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_InputConfig(uint8_t micEn, uint8_t lineInEn, uint8_t auxEn)
{
	uint16_t regval;  
	regval=bsp_audio_ReadReg(2);	//��ȡR2
	if(micEn)regval|=3<<2;		//����INPPGAENR,INPPGAENL(MIC��PGA�Ŵ�)
	else regval&=~(3<<2);		//�ر�INPPGAENR,INPPGAENL.
 	bsp_audio_WriteReg(2,regval);	//����R2 
	
	regval=bsp_audio_ReadReg(44);	//��ȡR44
	if(micEn)regval|=3<<4|3<<0;	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else regval&=~(3<<4|3<<0);	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	bsp_audio_WriteReg(44,regval);//����R44
	
	if(lineInEn)bsp_audio_LineInGain(5);//LINE IN 0dB����
	else bsp_audio_LineInGain(0);	//�ر�LINE IN
	if(auxEn)bsp_audio_LineInGain(7);//AUX 6dB����
	else bsp_audio_LineInGain(0);	//�ر�AUX���� 
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_OutputConfig(uint8_t dacEn, uint8_t bpsEn)
{
	uint16_t regval=0;
	if(dacEn)regval|=1<<0;	//DAC���ʹ��
	if(bpsEn)
	{
		regval|=1<<1;		//BYPASSʹ��
		regval|=5<<2;		//0dB����
	} 
	bsp_audio_WriteReg(50,regval);//R50����
	bsp_audio_WriteReg(51,regval);//R51���� 
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_MICGain(uint8_t gain)
{
	gain&=0X3F;
	bsp_audio_WriteReg(45,gain);		//R45,��ͨ��PGA���� 
	bsp_audio_WriteReg(46,gain|1<<8);	//R46,��ͨ��PGA����
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_LineInGain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=bsp_audio_ReadReg(47);	//��ȡR47
	regval&=~(7<<4);			//���ԭ�������� 
 	bsp_audio_WriteReg(47,regval|gain<<4);//����R47
	regval=bsp_audio_ReadReg(48);	//��ȡR48
	regval&=~(7<<4);			//���ԭ�������� 
 	bsp_audio_WriteReg(48,regval|gain<<4);//����R48
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_AUXGain(uint8_t gain)
{
		uint16_t regval;
	gain&=0X07;
	regval=bsp_audio_ReadReg(47);	//��ȡR47
	regval&=~(7<<0);			//���ԭ�������� 
 	bsp_audio_WriteReg(47,regval|gain<<0);//����R47
	regval=bsp_audio_ReadReg(48);	//��ȡR48
	regval&=~(7<<0);			//���ԭ�������� 
 	bsp_audio_WriteReg(48,regval|gain<<0);//����R48
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t bsp_audio_WriteReg(uint8_t reg, uint16_t val)
{
	bsp_i2c_Start(audio_i2c);
	bsp_i2c_SendByte(audio_i2c, (AUDIO_ADDR << 1) | AUDIO_WRITE);
	if(bsp_i2c_WaitAck(audio_i2c)) return 1;
	bsp_i2c_SendByte(audio_i2c, ((reg << 1) | (val >> 8)) & 0x01 );
	if(bsp_i2c_WaitAck(audio_i2c)) return 2;
	bsp_i2c_SendByte(audio_i2c, val & 0xff);
	if(bsp_i2c_WaitAck(audio_i2c)) return 3;
	bsp_i2c_Stop(audio_i2c);
	WM8978_REGVAL_TBL[reg]=val;
	return 0;
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint16_t bsp_audio_ReadReg(uint8_t reg)
{
	return WM8978_REGVAL_TBL[reg];
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_HPVolSet(uint8_t voll, uint8_t volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	if(voll==0)voll|=1<<6;//����Ϊ0ʱ,ֱ��mute
	if(volr==0)volr|=1<<6;//����Ϊ0ʱ,ֱ��mute 
	bsp_audio_WriteReg(52,voll);			//R52,������������������
	bsp_audio_WriteReg(53,volr|(1<<8));	//R53,������������������,ͬ������(HPVU=1)
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_SPKVolSet(uint8_t vol)
{
	vol&=0X3F;//�޶���Χ
	if(vol==0)vol|=1<<6;//����Ϊ0ʱ,ֱ��mute 
 	bsp_audio_WriteReg(54,vol);			//R54,������������������
	bsp_audio_WriteReg(55,vol|(1<<8));	//R55,������������������,ͬ������(SPKVU=1)	
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_I2SConfig(uint8_t fmt, uint8_t len)
{
	fmt&=0X03;
	len&=0X03;//�޶���Χ
	bsp_audio_WriteReg(4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_3DSet(uint8_t deepth)
{
	deepth&=0XF;//�޶���Χ 
 	bsp_audio_WriteReg(41,deepth);	//R41,3D�������� 	
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_EQ3DDir(uint8_t dir)
{
	uint16_t regval; 
	regval=bsp_audio_ReadReg(0X12);
	if(dir)regval|=1<<8;
	else regval&=~(1<<8); 
 	bsp_audio_WriteReg(18,regval);//R18,EQ1�ĵ�9λ����EQ/3D����
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_EQ1Set(uint8_t cfreq, uint8_t gain)
{
	uint16_t regval;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain;
	regval=bsp_audio_ReadReg(18);
	regval&=0X100;
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	bsp_audio_WriteReg(18,regval);//R18,EQ1���� 
}




/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_EQ2Set(uint8_t cfreq, uint8_t gain)
{
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	bsp_audio_WriteReg(19,regval);//R19,EQ2���� 	
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_EQ3Set(uint8_t cfreq, uint8_t gain)
{
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	bsp_audio_WriteReg(20,regval);//R20,EQ3���� 
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_EQ4Set(uint8_t cfreq, uint8_t gain)
{
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	bsp_audio_WriteReg(21,regval);//R21,EQ4���� 	
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_audio_EQ5Set(uint8_t cfreq, uint8_t gain)
{
	uint16_t regval=0;
	cfreq&=0X3;//�޶���Χ 
	if(gain>24)gain=24;
	gain=24-gain; 
	regval|=cfreq<<5;	//���ý�ֹƵ�� 
	regval|=gain;		//��������	
 	bsp_audio_WriteReg(22,regval);//R22,EQ5���� 	
}

/*************************************************************************************************************************/




/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void AUDIO_SDA_HIGH(void)
{
	AUDIO_SDA_PORT->ODR |= AUDIO_SDA_PIN;
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void AUDIO_SDA_LOW(void)
{
	AUDIO_SDA_PORT->ODR &= ~AUDIO_SDA_PIN;
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void AUDIO_SDA_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Pin = AUDIO_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(AUDIO_SDA_PORT, &GPIO_InitStructure);
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void AUDIO_SDA_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Pin = AUDIO_SDA_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(AUDIO_SDA_PORT, &GPIO_InitStructure);
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t AUDIO_SDA_READ(void)
{
	return ((AUDIO_SDA_PORT->ODR & AUDIO_SDA_PIN) == 0) ? 0 :1;
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void AUDIO_SCL_HIGH(void)
{
	AUDIO_SCL_PORT->BSRR = AUDIO_SCL_PIN;
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void AUDIO_SCL_LOW(void)
{
	
	AUDIO_SCL_PORT->ODR &= ~AUDIO_SCL_PIN;
}

