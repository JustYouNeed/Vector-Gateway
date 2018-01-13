# ifndef __BSP_I2S_H
# define __BSP_I2S_H

# include "bsp.h"


extern void (*i2s_tx_callback)(void);		//IIS TX回调函数指针 
extern void (*i2s_rx_callback)(void);		//IIS RX回调函数指针 

void bsp_i2s_Config(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat);
void bsp_i2s_ExtConfig(uint16_t I2S_Standard,uint16_t I2S_Mode,uint16_t I2S_Clock_Polarity,uint16_t I2S_DataFormat);
uint8_t bsp_i2s_SampleRateSet(uint32_t samplerate);
void bsp_i2s_TX_DMAConfig(uint8_t* buf0,uint8_t *buf1,uint16_t num);
void bsp_i2s_RX_DMAConfig(uint8_t* buf0,uint8_t *buf1,uint16_t num);

void bsp_i2s_PlayStart(void);
void bsp_i2s_PlayStop(void);
void bsp_i2s_RecStart(void);
void bsp_i2s_RecStop(void);


# endif

