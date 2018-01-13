# ifndef __BSP_IIC_H
# define __BSP_IIC_H

# include "bsp.h"

typedef uint8_t (*pFun)(void);
typedef void (*pFunction)(void);


typedef struct{
	pFunction set_sda_in;
	pFunction set_sda_out;
	pFunction set_sda_high;
	pFunction set_sda_low;
	pFun read_sda;
	pFunction set_scl_high;
	pFunction set_scl_low;
}I2C_Str, *pI2C_Str;

void bsp_i2cStart(I2C_Str i2c_structure);
void bsp_i2cStop(I2C_Str i2c_structure);
void bsp_i2cAck(I2C_Str i2c_structure);
void bsp_i2cNoAck(I2C_Str i2c_structure);
uint8_t bsp_i2cWaitAck(I2C_Str i2c_structure);
void bsp_i2cSendByte(I2C_Str i2c_structure, uint8_t byte);
uint8_t bsp_i2cReadByte(I2C_Str i2c_structure, uint8_t ack);


# endif




