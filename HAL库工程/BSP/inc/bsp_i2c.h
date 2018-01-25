# ifndef __BSP_IIC_H
# define __BSP_IIC_H

# include "bsp.h"

typedef uint8_t (*pFun)(void);
typedef void (*pFunction)(void);

/*  I2C���ܽṹ��,�����ò�ͬ��I2C�豸����һ����������  */
typedef struct{
	pFunction set_sda_in;  /*  ����SDAΪ���뺯��ָ��  */
	pFunction set_sda_out;	/*  ����SDA�������ָ��  */
	pFunction set_sda_high;	/*  ����SDA����ߺ���ָ��  */
	pFunction set_sda_low;  /*  SDA����ͺ���ָ��  */
	pFun read_sda;    /*  ��ȡSDA����ָ��  */
	pFunction set_scl_high;  /*  SCL�����ָ��  */
	pFunction set_scl_low;  /*  SCL�����ָ��  */
}I2C_Str, *pI2C_Str;

void bsp_i2c_Start(I2C_Str i2c_structure);
void bsp_i2c_Stop(I2C_Str i2c_structure);
void bsp_i2c_Ack(I2C_Str i2c_structure);
void bsp_i2c_NoAck(I2C_Str i2c_structure);
uint8_t bsp_i2c_WaitAck(I2C_Str i2c_structure);
void bsp_i2c_SendByte(I2C_Str i2c_structure, uint8_t byte);
uint8_t bsp_i2c_ReadByte(I2C_Str i2c_structure, uint8_t ack);


# endif




