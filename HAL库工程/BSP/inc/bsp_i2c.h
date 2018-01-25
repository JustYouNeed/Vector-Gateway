# ifndef __BSP_IIC_H
# define __BSP_IIC_H

# include "bsp.h"

typedef uint8_t (*pFun)(void);
typedef void (*pFunction)(void);

/*  I2C功能结构体,可以让不同的I2C设备共用一套驱动代码  */
typedef struct{
	pFunction set_sda_in;  /*  设置SDA为输入函数指针  */
	pFunction set_sda_out;	/*  设置SDA输出函数指针  */
	pFunction set_sda_high;	/*  设置SDA输出高函数指针  */
	pFunction set_sda_low;  /*  SDA输出低函数指针  */
	pFun read_sda;    /*  读取SDA函数指针  */
	pFunction set_scl_high;  /*  SCL输出高指针  */
	pFunction set_scl_low;  /*  SCL输出低指针  */
}I2C_Str, *pI2C_Str;

void bsp_i2c_Start(I2C_Str i2c_structure);
void bsp_i2c_Stop(I2C_Str i2c_structure);
void bsp_i2c_Ack(I2C_Str i2c_structure);
void bsp_i2c_NoAck(I2C_Str i2c_structure);
uint8_t bsp_i2c_WaitAck(I2C_Str i2c_structure);
void bsp_i2c_SendByte(I2C_Str i2c_structure, uint8_t byte);
uint8_t bsp_i2c_ReadByte(I2C_Str i2c_structure, uint8_t ack);


# endif




