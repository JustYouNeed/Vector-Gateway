# ifndef __APP_AUDIO_H
# define __APP_AUDIO_H

# include "bsp.h"
# include "ff.h"
# include "wavplay.h"
# include "bsp_audio.h"
# include "bsp_i2s.h"

# define I2S_TX_DMA_BUF_SIZE		8192

typedef __packed struct
{
	uint8_t *i2sBuff1;  /*  I2S��������  */
	uint8_t *i2sBuff2;
	
	uint8_t *tempBuff;  /*  ��ʱ��������  */
	FIL *file;					/*  ��Ƶ�ļ�ָ��  */
	__IO uint8_t status;	/*  ����״ָ̬ʾ  */
	uint32_t (*file_seek)(uint32_t);
	
	uint8_t mode;			/*  ����ģʽ  */
	
	uint8_t *path;		/*  ��ǰ�ļ���·��  */
	uint8_t *name;	/*  ��ǰ��Ƶ����  */
	uint16_t nameLen;		/*  ��Ƶ���ֳ���  */
	uint8_t curNamePos;		/*  ��ǰ��ƫ��  */
	uint32_t audioTime;		/*  ��Ƶʱ��  */
	uint32_t curTime;		/*  ��ǰ���Ž���  */
	uint32_t bitrate;		/*  ������  */
	uint32_t samplerate;	/*  ������  */
	uint16_t bps;					/*  λ��  */
		
	uint16_t curIndex;		/*  ��Ƶ�ļ�����  */
	uint16_t mfileNum;		/*  �����ļ���Ŀ  */
	uint16_t *mfIndexTbl;	/*  ��Ƶ�ļ�������  */
}Audio_Str;

extern Audio_Str audio_Info;

//���ֲ��Ų����������
typedef enum {
	AP_OK=0X00,				//�����������
	AP_NEXT,				//������һ��
	AP_PREV,				//������һ��
	AP_ERR=0X80,			//�����д���(û����������,������ʾ����) 
}APRESULT;

void app_audio_Start(void);
void app_audio_Stop(void);
void app_audio_Play(void);
uint8_t app_audio_PlayAudio(uint8_t *fname);

uint16_t app_audio_GetAudios(uint8_t *path);



# endif

