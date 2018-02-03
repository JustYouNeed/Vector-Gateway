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
	uint8_t *i2sBuff1;  /*  I2S解码数组  */
	uint8_t *i2sBuff2;
	
	uint8_t *tempBuff;  /*  零时解码数组  */
	FIL *file;					/*  音频文件指针  */
	__IO uint8_t status;	/*  播放状态指示  */
	uint32_t (*file_seek)(uint32_t);
	
	uint8_t mode;			/*  播放模式  */
	
	uint8_t *path;		/*  当前文件夹路径  */
	uint8_t *name;	/*  当前音频名字  */
	uint16_t nameLen;		/*  音频名字长度  */
	uint8_t curNamePos;		/*  当前的偏移  */
	uint32_t audioTime;		/*  音频时长  */
	uint32_t curTime;		/*  当前播放进度  */
	uint32_t bitrate;		/*  比特率  */
	uint32_t samplerate;	/*  采样率  */
	uint16_t bps;					/*  位数  */
		
	uint16_t curIndex;		/*  音频文件索引  */
	uint16_t mfileNum;		/*  音乐文件数目  */
	uint16_t *mfIndexTbl;	/*  音频文件索引表  */
}Audio_Str;

extern Audio_Str audio_Info;

//音乐播放操作结果定义
typedef enum {
	AP_OK=0X00,				//正常播放完成
	AP_NEXT,				//播放下一曲
	AP_PREV,				//播放上一曲
	AP_ERR=0X80,			//播放有错误(没定义错误代码,仅仅表示出错) 
}APRESULT;

void app_audio_Start(void);
void app_audio_Stop(void);
void app_audio_Play(void);
uint8_t app_audio_PlayAudio(uint8_t *fname);

uint16_t app_audio_GetAudios(uint8_t *path);



# endif

