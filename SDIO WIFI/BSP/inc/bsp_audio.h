# ifndef __BSP_AUDIO_H
# define __BSP_AUDIO_H

# include "bsp.h"
# include "bsp_i2c.h"
#define AUDIO_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 
 
#define EQ1_80Hz		0X00
#define EQ1_105Hz		0X01
#define EQ1_135Hz		0X02
#define EQ1_175Hz		0X03

#define EQ2_230Hz		0X00
#define EQ2_300Hz		0X01
#define EQ2_385Hz		0X02
#define EQ2_500Hz		0X03

#define EQ3_650Hz		0X00
#define EQ3_850Hz		0X01
#define EQ3_1100Hz		0X02
#define EQ3_14000Hz		0X03

#define EQ4_1800Hz		0X00
#define EQ4_2400Hz		0X01
#define EQ4_3200Hz		0X02
#define EQ4_4100Hz		0X03

#define EQ5_5300Hz		0X00
#define EQ5_6900Hz		0X01
#define EQ5_9000Hz		0X02
#define EQ5_11700Hz		0X03

# define AUDIO_WRITE		0
# define AUDIO_READ			1


# define AUDIO_SDA_PORT			GPIOE
# define AUDIO_SDA_PIN				GPIO_Pin_2
# define AUDIO_SCL_PORT			GPIOE
# define AUDIO_SCL_PIN				GPIO_Pin_3


extern I2C_Str audio_i2c;

void bsp_audio_IICConfig(void);

void AUDIO_SDA_HIGH(void);
void AUDIO_SDA_LOW(void);
void AUDIO_SDA_OUT(void);
void AUDIO_SDA_IN(void);
uint8_t AUDIO_SDA_READ(void);

void AUDIO_SCL_HIGH(void);
void AUDIO_SCL_LOW(void);



uint8_t bsp_audio_Config(void);
void bsp_audio_ADDAConfig(uint8_t dacEn, uint8_t adcEn);
void bsp_audio_InputConfig(uint8_t micEn, uint8_t lineInEn, uint8_t auxEn);
void bsp_audio_OutputConfig(uint8_t dacEn, uint8_t bpsEn);
void bsp_audio_MICGain(uint8_t gain);
void bsp_audio_LineInGain(uint8_t gain);
void bsp_audio_AUXGain(uint8_t gain);
uint8_t bsp_audio_WriteReg(uint8_t reg, uint16_t val);
uint16_t bsp_audio_ReadReg(uint8_t reg);
void bsp_audio_HPVolSet(uint8_t voll, uint8_t volr);
void bsp_audio_SPKVolSet(uint8_t vol);
void bsp_audio_I2SConfig(uint8_t fmt, uint8_t len);
void bsp_audio_3DSet(uint8_t deepth);
void bsp_audio_EQ3DDir(uint8_t dir);
void bsp_audio_EQ1Set(uint8_t cfreq, uint8_t gain);
void bsp_audio_EQ2Set(uint8_t cfreq, uint8_t gain);
void bsp_audio_EQ3Set(uint8_t cfreq, uint8_t gain);
void bsp_audio_EQ4Set(uint8_t cfreq, uint8_t gain);
void bsp_audio_EQ5Set(uint8_t cfreq, uint8_t gain);

# endif

