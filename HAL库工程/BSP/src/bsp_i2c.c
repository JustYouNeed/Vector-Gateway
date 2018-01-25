# include "bsp_i2c.h"



/*
*********************************************************************************************************
*                                          bsp_i2c_Start
*
* Description: I2C����ͨ�ź���
*              ����ʱ��: SCLΪ�ߵ�ƽ�ڼ�,SDA�ṩһ���½���
*             
* Arguments  : i2c_structure: I2C_Str,�ṩ��ͬI2C�豸�ĵײ�����
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_i2c_Start(I2C_Str i2c_structure)
{
	i2c_structure.set_sda_out();  /*  ������SDA��Ϊ���ģʽ  */
	i2c_structure.set_sda_high(); /*  ����SDA����SCL��  */
	i2c_structure.set_scl_high();
	
	/*  �ṩSDA���½���  */
	bsp_tim_DelayUs(4);
	i2c_structure.set_sda_low();
	bsp_tim_DelayUs(4);
	
	i2c_structure.set_scl_low(); /*  SCL���б��ֵ͵�ƽ  */
}


/*
*********************************************************************************************************
*                                          bsp_i2c_Stop
*
* Description: I2Cֹͣͨ�ź���
*              ֹͣʱ��:SCLΪ�ߵ�ƽ�ڼ�,SDA�ṩһ��������
*             
* Arguments  : i2c_structure
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_i2c_Stop(I2C_Str i2c_structure)
{
	i2c_structure.set_sda_out();  /*  ������SDA��Ϊ���ģʽ  */
	i2c_structure.set_scl_high(); /*  SCL����,SDA����  */
	i2c_structure.set_sda_low();
	
	/*  SDA�ṩ������  */
	bsp_tim_DelayUs(4);
	i2c_structure.set_sda_high();
	bsp_tim_DelayUs(4);
	
	i2c_structure.set_scl_low(); /*  SCL�ع����  */
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
void bsp_i2c_Ack(I2C_Str i2c_structure)
{
	i2c_structure.set_scl_low();
	
	i2c_structure.set_sda_out();
	
	i2c_structure.set_sda_low();
	
	bsp_tim_DelayUs(2);
	
	i2c_structure.set_scl_high();
	
	bsp_tim_DelayUs(2);
	
	i2c_structure.set_scl_low();
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
void bsp_i2c_NoAck(I2C_Str i2c_structure)
{
	i2c_structure.set_scl_low();
	i2c_structure.set_sda_out();
	i2c_structure.set_sda_high();
	
	bsp_tim_DelayUs(2);
	
	i2c_structure.set_scl_high();
	
	bsp_tim_DelayUs(2);
	
	i2c_structure.set_scl_low();
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
uint8_t bsp_i2c_WaitAck(I2C_Str i2c_structure)
{
	uint8_t ErrTime = 0;
	
	i2c_structure.set_sda_in();
	i2c_structure.set_sda_high();
	bsp_tim_DelayUs(1);
	i2c_structure.set_scl_high();
	bsp_tim_DelayUs(1);
	
	while(i2c_structure.read_sda())
	{
		ErrTime ++;
		if(ErrTime > 250)
		{
			bsp_i2c_Stop(i2c_structure);
			return 1;
		}
	}
	i2c_structure.set_scl_low();
	return 0;
}


/*
*********************************************************************************************************
*                                        bsp_i2c_SendByte  
*
* Description: ����һ���ֽ����ݵ�I2C�豸
*              
* Arguments  : 1> i2c_structure: I2C_Str�ṹ��
*              2> byte: unsigned char��,Ҫ���͵�����
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_i2c_SendByte(I2C_Str i2c_structure, uint8_t byte)
{
	uint8_t cnt = 0x0;
	
	i2c_structure.set_sda_out(); /*  ����SDAΪ���  */
	i2c_structure.set_scl_low(); /*  Ϊ����������׼��,���Ǳ���  */
	
	for(; cnt < 8; cnt ++)
	{
		/*  ��������,��λ��ǰ  */
		if((byte&0x80)>>7 == 1) i2c_structure.set_sda_high();
		else if((byte&0x80)>>7 == 0) i2c_structure.set_sda_low();
		byte <<= 1;
		
		/*  ������SCL�͵�ƽ�ڼ�仯,�ߵ�ƽ�ڼ䱻�豸��ȡ  */
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_high();
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_low();
		bsp_tim_DelayUs(2);
	}
}


/*
*********************************************************************************************************
*                                  bsp_i2c_ReadByte        
*
* Description: ͨ��I2CЭ���ȡһ���ֽڵ�����
*             
* Arguments  : 1> i2c_structure: I2C_Str�ṹ��
*              2> ack:�Ƿ���Ҫack, 0:����Ҫ, 1:��Ҫ
*
* Reutrn     : ��ȡ����������
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t bsp_i2c_ReadByte(I2C_Str i2c_structure, uint8_t ack)
{
	uint8_t cnt = 0x0, rec = 0x0;
	
	i2c_structure.set_sda_in();  /*  ����SDAΪ���  */
	
	for(; cnt < 8; cnt ++)  
	{
		/*  ������SCL���������ڼ䱻�豸���,�ߵ�ƽ�ڼ䱣�ֲ���  */
		i2c_structure.set_scl_low();  
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_high();
		
		/*  ��ȡ����,��λ��ǰ  */
		rec <<= 1;
		if(i2c_structure.read_sda()) rec ++;
		
		/*  Ϊ�´�bit׼��  */
		bsp_tim_DelayUs(1);
	} /*  end for  */
	/*  �Ƿ���ҪACK  */
	if(!ack)
		bsp_i2c_NoAck(i2c_structure);
	else
		bsp_i2c_Ack(i2c_structure);
	return rec;
}

