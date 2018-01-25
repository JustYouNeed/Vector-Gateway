# include "bsp_i2c.h"



/*
*********************************************************************************************************
*                                          bsp_i2c_Start
*
* Description: I2C启动通信函数
*              启动时序: SCL为高电平期间,SDA提供一个下降沿
*             
* Arguments  : i2c_structure: I2C_Str,提供不同I2C设备的底层驱动
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_i2c_Start(I2C_Str i2c_structure)
{
	i2c_structure.set_sda_out();  /*  先设置SDA线为输出模式  */
	i2c_structure.set_sda_high(); /*  拉高SDA线与SCL线  */
	i2c_structure.set_scl_high();
	
	/*  提供SDA的下降沿  */
	bsp_tim_DelayUs(4);
	i2c_structure.set_sda_low();
	bsp_tim_DelayUs(4);
	
	i2c_structure.set_scl_low(); /*  SCL空闲保持低电平  */
}


/*
*********************************************************************************************************
*                                          bsp_i2c_Stop
*
* Description: I2C停止通信函数
*              停止时序:SCL为高电平期间,SDA提供一个上升沿
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
	i2c_structure.set_sda_out();  /*  先设置SDA线为输出模式  */
	i2c_structure.set_scl_high(); /*  SCL拉高,SDA拉低  */
	i2c_structure.set_sda_low();
	
	/*  SDA提供上升沿  */
	bsp_tim_DelayUs(4);
	i2c_structure.set_sda_high();
	bsp_tim_DelayUs(4);
	
	i2c_structure.set_scl_low(); /*  SCL回归空闲  */
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
* Description: 发送一个字节数据到I2C设备
*              
* Arguments  : 1> i2c_structure: I2C_Str结构体
*              2> byte: unsigned char型,要发送的数据
*
* Reutrn     : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/
void bsp_i2c_SendByte(I2C_Str i2c_structure, uint8_t byte)
{
	uint8_t cnt = 0x0;
	
	i2c_structure.set_sda_out(); /*  设置SDA为输出  */
	i2c_structure.set_scl_low(); /*  为发送数据做准备,不是必须  */
	
	for(; cnt < 8; cnt ++)
	{
		/*  发送数据,高位在前  */
		if((byte&0x80)>>7 == 1) i2c_structure.set_sda_high();
		else if((byte&0x80)>>7 == 0) i2c_structure.set_sda_low();
		byte <<= 1;
		
		/*  数据在SCL低电平期间变化,高电平期间被设备读取  */
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
* Description: 通过I2C协议读取一个字节的数据
*             
* Arguments  : 1> i2c_structure: I2C_Str结构体
*              2> ack:是否需要ack, 0:不需要, 1:需要
*
* Reutrn     : 读取回来的数据
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t bsp_i2c_ReadByte(I2C_Str i2c_structure, uint8_t ack)
{
	uint8_t cnt = 0x0, rec = 0x0;
	
	i2c_structure.set_sda_in();  /*  设置SDA为输出  */
	
	for(; cnt < 8; cnt ++)  
	{
		/*  数据在SCL的上升沿期间被设备输出,高电平期间保持不变  */
		i2c_structure.set_scl_low();  
		bsp_tim_DelayUs(2);
		i2c_structure.set_scl_high();
		
		/*  读取数据,高位在前  */
		rec <<= 1;
		if(i2c_structure.read_sda()) rec ++;
		
		/*  为下次bit准备  */
		bsp_tim_DelayUs(1);
	} /*  end for  */
	/*  是否需要ACK  */
	if(!ack)
		bsp_i2c_NoAck(i2c_structure);
	else
		bsp_i2c_Ack(i2c_structure);
	return rec;
}

