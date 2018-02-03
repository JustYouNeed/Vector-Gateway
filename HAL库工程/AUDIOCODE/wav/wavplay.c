# include "bsp.h"
# include "wavplay.h"
# include "ff.h"
# include "app_audio.h"
# include "includes.h"
# include "os_cfg_app.h"

__wavctrl wavctrl;		//WAV控制结构体
__IO uint8_t wavtransferend=0;	//i2s传输完成标志
__IO uint8_t wavwitchbuf=0;		//i2sbufx指示标志
 
//WAV解析初始化
//fname:文件路径+文件名
//wavx:wav 信息存放结构体指针
//返回值:0,成功;1,打开文件失败;2,非WAV文件;3,DATA区域未找到.
uint8_t wav_decode_init(uint8_t* fname,__wavctrl* wavx)
{
	FIL*ftemp;
	uint8_t *buf; 
	uint32_t br=0;
	uint8_t res=0;
	
	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;
	ftemp=(FIL*)bsp_mem_Malloc(SRAMIN,sizeof(FIL));
	buf=bsp_mem_Malloc(SRAMIN,512);
	if(ftemp&&buf)	//内存申请成功
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//打开文件
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	//读取512字节在数据
			riff=(ChunkRIFF *)buf;		//获取RIFF块
			if(riff->Format==0X45564157)//是WAV文件
			{
				fmt=(ChunkFMT *)(buf+12);	//获取FMT块 
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//读取FACT块
				if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//具有fact/LIST块的时候(未测试)
				else wavx->datastart=12+8+fmt->ChunkSize;  
				data=(ChunkDATA *)(buf+wavx->datastart);	//读取DATA块
				if(data->ChunkID==0X61746164)//解析成功!
				{
					wavx->audioformat=fmt->AudioFormat;		//音频格式
					wavx->nchannels=fmt->NumOfChannels;		//通道数
					wavx->samplerate=fmt->SampleRate;		//采样率
					wavx->bitrate=fmt->ByteRate*8;			//得到位速
					wavx->blockalign=fmt->BlockAlign;		//块对齐
					wavx->bps=fmt->BitsPerSample;			//位数,16/24/32位
					
					wavx->datasize=data->ChunkSize;			//数据块大小
					wavx->datastart=wavx->datastart+8;		//数据流开始的地方. 
					 
					printf("wavx->audioformat:%d\r\n",wavx->audioformat);
					printf("wavx->nchannels:%d\r\n",wavx->nchannels);
					printf("wavx->samplerate:%d\r\n",wavx->samplerate);
					printf("wavx->bitrate:%d\r\n",wavx->bitrate);
					printf("wavx->blockalign:%d\r\n",wavx->blockalign);
					printf("wavx->bps:%d\r\n",wavx->bps);
					printf("wavx->datasize:%d\r\n",wavx->datasize);
					printf("wavx->datastart:%d\r\n",wavx->datastart);  
				}else res=3;//data区域未找到.
			}else res=2;//非wav文件
			
		}else res=1;//打开文件错误
	}
	f_close(ftemp);
	bsp_mem_Free(SRAMIN,ftemp);//释放内存
	bsp_mem_Free(SRAMIN,buf); 
	return 0;
}

//填充buf
//buf:数据区
//size:填充数据量
//bits:位数(16/24)
//返回值:读到的数据个数
uint32_t wav_buffill(uint8_t *buf,uint16_t size,uint8_t bits)
{
	uint16_t readlen=0;
	uint32_t bread;
	uint16_t i;
	uint8_t *p;
	if(bits==24)//24bit音频,需要处理一下
	{
		readlen=(size/4)*3;							//此次要读取的字节数
		f_read(audio_Info.file,audio_Info.tempBuff,readlen,(UINT*)&bread);	//读取数据
		p=audio_Info.tempBuff;
		for(i=0;i<size;)
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		} 
		bread=(bread*4)/3;		//填充后的大小.
	}else 
	{
		f_read(audio_Info.file,buf,size,(UINT*)&bread);//16bit音频,直接读取数据  
		if(bread<size)//不够数据了,补充0
		{
			for(i=bread;i<size-bread;i++)buf[i]=0; 
		}
	}
	return bread;
}  
//WAV播放时,I2S DMA传输回调函数
void wav_i2s_dma_tx_callback(void) 
{   
	uint16_t i;
	printf("sending...\r\n");
	if(DMA1_Stream4->CR&(1<<19))
	{
		wavwitchbuf=0;
		if((audio_Info.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audio_Info.i2sBuff1[i]=0;//填充0
			}
		}
	}else 
	{
		wavwitchbuf=1;
		if((audio_Info.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//暂停
			{
				audio_Info.i2sBuff2[i]=0;//填充0
			}
		}
	}
	wavtransferend=1;
} 
//得到当前播放时间
//fx:文件指针
//wavx:wav播放控制器
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;  	
 	wavx->totsec=wavx->datasize/(wavx->bitrate/8);	//歌曲总长度(单位:秒) 
	fpos=fx->fptr-wavx->datastart; 					//得到当前文件播放到的地方 
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;	//当前播放到第多少秒了?	
}
//wav文件快进快退函数
//pos:需要定位到的文件位置
//返回值:当前文件位置(即定位后的结果)
uint32_t wav_file_seek(uint32_t pos)
{
	uint8_t temp;
	if(pos>audio_Info.file->fsize)
	{
		pos=audio_Info.file->fsize;
	}
	if(pos<wavctrl.datastart)pos=wavctrl.datastart;
	if(wavctrl.bps==16)temp=8;	//必须是8的倍数
	if(wavctrl.bps==24)temp=12;	//必须是12的倍数 
	if((pos-wavctrl.datastart)%temp)
	{
		pos+=temp-(pos-wavctrl.datastart)%temp;
	}
	f_lseek(audio_Info.file,pos);
	return audio_Info.file->fptr;
}
//播放某个WAV文件
//fname:wav文件路径.
//返回值:0,正常播放完成
//[b7]:0,正常状态;1,错误状态
//[b6:0]:b7=0时,表示操作码
//       b7=1时,表示有错误(这里不判定具体错误,0X80~0XFF,都算是错误)
uint8_t wav_play_song(uint8_t* fname)
{
	uint8_t res;  
	uint32_t fillnum; 
	audio_Info.file=(FIL*)bsp_mem_Malloc(SRAMIN,sizeof(FIL));
	audio_Info.i2sBuff1=bsp_mem_Malloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audio_Info.i2sBuff2=bsp_mem_Malloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audio_Info.tempBuff=bsp_mem_Malloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audio_Info.file_seek=wav_file_seek;
	if(audio_Info.file&&audio_Info.i2sBuff1&&audio_Info.i2sBuff2&&audio_Info.tempBuff)
	{ 
		res=wav_decode_init(fname,&wavctrl);//得到文件的信息
		if(res==0)//解析文件成功
		{
			printf("解析文件成功,正在播放...\r\n");
			if(wavctrl.bps==16)
			{
				bsp_audio_I2SConfig(2,0);	//飞利浦标准,16位数据长度
				bsp_i2s_Config(0,2,0,1);		//飞利浦标准,主机发送,时钟低电平有效,16位扩展帧长度  
			}else if(wavctrl.bps==24)
			{
				bsp_audio_I2SConfig(2,2);	//飞利浦标准,24位数据长度
				bsp_i2s_Config(0,2,0,2);		//飞利浦标准,主机发送,时钟低电平有效,24位扩展帧长度
			}
			bsp_i2s_SampleRateSet(wavctrl.samplerate);//设置采样率
			bsp_i2s_TX_DMAConfig(audio_Info.i2sBuff1,audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE/2); //配置TX DMA
			i2s_tx_callback=wav_i2s_dma_tx_callback;			//回调函数指wav_i2s_dma_callback
			app_audio_Stop();
			res=f_open(audio_Info.file,(TCHAR*)fname,FA_READ);	//打开文件
	
			if(res==0)
			{
				printf("open file successful\r\n");
				
				f_lseek(audio_Info.file, wavctrl.datastart);		//跳过文件头
				fillnum=wav_buffill(audio_Info.i2sBuff1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				fillnum=wav_buffill(audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				app_audio_Start();  
				printf("start...\r\n");
				while(1)
				{ 
					while(wavtransferend==0)//等待wav传输完成;
					{
						//delay_ms(1000/OS_TICKS_PER_SEC);
					//	bsp_tim_DelayMs(1000/OS_CFG_TICK_RATE_HZ);
		
					}
					wavtransferend=0;
					
 					if(wavwitchbuf)fillnum=wav_buffill(audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//填充buf2
					else fillnum=wav_buffill(audio_Info.i2sBuff1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//填充buf1
					
 					while(1)//正常播放中
					{		 
						printf("playing\r\n");
						wav_get_curtime(audio_Info.file,&wavctrl);//得到总时间和当前播放的时间 
						audio_Info.audioTime=wavctrl.totsec;			//参数传递
						audio_Info.curTime=wavctrl.cursec;
						audio_Info.bitrate=wavctrl.bitrate;
						audio_Info.samplerate=wavctrl.samplerate;
						audio_Info.bps=wavctrl.bps; 
						
  					if(audio_Info.status&0X01)break;//没有按下暂停 
						else bsp_tim_DelayMs(1000/OS_CFG_TICK_RATE_HZ);
					}
					if((audio_Info.status&(1<<1))==0||(fillnum!=WAV_I2S_TX_DMA_BUFSIZE))//请求结束播放/播放完成
					{  
						break;
					} 					
				}
				app_audio_Stop();
				printf("stop\r\n");
			}else res=AP_ERR; 
		}else res=AP_ERR;
	}else res=AP_ERR; 
	bsp_mem_Free(SRAMIN,audio_Info.tempBuff);	//释放内存
	bsp_mem_Free(SRAMIN,audio_Info.i2sBuff1);//释放内存
	bsp_mem_Free(SRAMIN,audio_Info.i2sBuff2);//释放内存 
	bsp_mem_Free(SRAMIN,audio_Info.file);	//释放内存 
	return res;
} 
	









