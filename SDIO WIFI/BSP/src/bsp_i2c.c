# include "bsp_i2c.h"



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
void bsp_i2cStart(I2C_Str i2c_structure)
{
	i2c_structure.set_sda_out();
	i2c_structure.set_sda_high();
	i2c_structure.set_scl_high();
	
	bsp_tim_DelayUs(4);
	i2c_structure.set_sda_low();
	bsp_tim_DelayUs(4);
	i2c_structure.set_scl_low();
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
void bsp_i2cStop(I2C_Str i2c_structure)
{
	i2c_structure.set_sda_out();
	i2c_structure.set_scl_low();
	i2c_structure.set_sda_low();
	
	bsp_tim_DelayUs(4);
	
	i2c_structure.set_scl_high();
	i2c_structure.set_sda_high();
	
	bsp_tim_DelayUs(4);
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
void bsp_i2cAck(I2C_Str i2c_structure)
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
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void bsp_i2cNoAck(I2C_Str i2c_structure)
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
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t bsp_i2cWaitAck(I2C_Str i2c_structure)
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
			bsp_i2cStop(i2c_structure);
			return 1;
		}
	}
	i2c_structure.set_scl_low();
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
void bsp_i2cSendByte(I2C_Str i2c_structure, uint8_t byte)
{
	uint8_t cnt = 0x0;
	
	i2c_structure.set_sda_out();
	i2c_structure.set_scl_low();
	
	for(; cnt < 8; cnt ++)
	{
		if((byte&0x80)>>7 == 1) i2c_structure.set_sda_high();
		else if((byte&0x80)>>7 == 0) i2c_structure.set_sda_low();
		byte <<= 1;
		
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_high();
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_low();
		bsp_tim_DelayUs(2);
	}
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
uint8_t bsp_i2cReadByte(I2C_Str i2c_structure, uint8_t ack)
{
	uint8_t cnt = 0x0, rec = 0x0;
	
	i2c_structure.set_sda_in();
	
	for(; cnt < 8; cnt ++)
	{
		i2c_structure.set_scl_low();
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_high();
		
		rec <<= 1;
		if(i2c_structure.read_sda()) rec ++;
		
		bsp_tim_DelayUs(1);
	}
	if(!ack)
		bsp_i2cNoAck(i2c_structure);
	else
		bsp_i2cAck(i2c_structure);
	return rec;
}

