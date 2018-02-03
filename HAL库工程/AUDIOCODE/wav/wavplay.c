# include "bsp.h"
# include "wavplay.h"
# include "ff.h"
# include "app_audio.h"
# include "includes.h"
# include "os_cfg_app.h"

__wavctrl wavctrl;		//WAV���ƽṹ��
__IO uint8_t wavtransferend=0;	//i2s������ɱ�־
__IO uint8_t wavwitchbuf=0;		//i2sbufxָʾ��־
 
//WAV������ʼ��
//fname:�ļ�·��+�ļ���
//wavx:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��;2,��WAV�ļ�;3,DATA����δ�ҵ�.
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
	if(ftemp&&buf)	//�ڴ�����ɹ�
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//���ļ�
		if(res==FR_OK)
		{
			f_read(ftemp,buf,512,&br);	//��ȡ512�ֽ�������
			riff=(ChunkRIFF *)buf;		//��ȡRIFF��
			if(riff->Format==0X45564157)//��WAV�ļ�
			{
				fmt=(ChunkFMT *)(buf+12);	//��ȡFMT�� 
				fact=(ChunkFACT *)(buf+12+8+fmt->ChunkSize);//��ȡFACT��
				if(fact->ChunkID==0X74636166||fact->ChunkID==0X5453494C)wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;//����fact/LIST���ʱ��(δ����)
				else wavx->datastart=12+8+fmt->ChunkSize;  
				data=(ChunkDATA *)(buf+wavx->datastart);	//��ȡDATA��
				if(data->ChunkID==0X61746164)//�����ɹ�!
				{
					wavx->audioformat=fmt->AudioFormat;		//��Ƶ��ʽ
					wavx->nchannels=fmt->NumOfChannels;		//ͨ����
					wavx->samplerate=fmt->SampleRate;		//������
					wavx->bitrate=fmt->ByteRate*8;			//�õ�λ��
					wavx->blockalign=fmt->BlockAlign;		//�����
					wavx->bps=fmt->BitsPerSample;			//λ��,16/24/32λ
					
					wavx->datasize=data->ChunkSize;			//���ݿ��С
					wavx->datastart=wavx->datastart+8;		//��������ʼ�ĵط�. 
					 
					printf("wavx->audioformat:%d\r\n",wavx->audioformat);
					printf("wavx->nchannels:%d\r\n",wavx->nchannels);
					printf("wavx->samplerate:%d\r\n",wavx->samplerate);
					printf("wavx->bitrate:%d\r\n",wavx->bitrate);
					printf("wavx->blockalign:%d\r\n",wavx->blockalign);
					printf("wavx->bps:%d\r\n",wavx->bps);
					printf("wavx->datasize:%d\r\n",wavx->datasize);
					printf("wavx->datastart:%d\r\n",wavx->datastart);  
				}else res=3;//data����δ�ҵ�.
			}else res=2;//��wav�ļ�
			
		}else res=1;//���ļ�����
	}
	f_close(ftemp);
	bsp_mem_Free(SRAMIN,ftemp);//�ͷ��ڴ�
	bsp_mem_Free(SRAMIN,buf); 
	return 0;
}

//���buf
//buf:������
//size:���������
//bits:λ��(16/24)
//����ֵ:���������ݸ���
uint32_t wav_buffill(uint8_t *buf,uint16_t size,uint8_t bits)
{
	uint16_t readlen=0;
	uint32_t bread;
	uint16_t i;
	uint8_t *p;
	if(bits==24)//24bit��Ƶ,��Ҫ����һ��
	{
		readlen=(size/4)*3;							//�˴�Ҫ��ȡ���ֽ���
		f_read(audio_Info.file,audio_Info.tempBuff,readlen,(UINT*)&bread);	//��ȡ����
		p=audio_Info.tempBuff;
		for(i=0;i<size;)
		{
			buf[i++]=p[1];
			buf[i]=p[2]; 
			i+=2;
			buf[i++]=p[0];
			p+=3;
		} 
		bread=(bread*4)/3;		//����Ĵ�С.
	}else 
	{
		f_read(audio_Info.file,buf,size,(UINT*)&bread);//16bit��Ƶ,ֱ�Ӷ�ȡ����  
		if(bread<size)//����������,����0
		{
			for(i=bread;i<size-bread;i++)buf[i]=0; 
		}
	}
	return bread;
}  
//WAV����ʱ,I2S DMA����ص�����
void wav_i2s_dma_tx_callback(void) 
{   
	uint16_t i;
	printf("sending...\r\n");
	if(DMA1_Stream4->CR&(1<<19))
	{
		wavwitchbuf=0;
		if((audio_Info.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audio_Info.i2sBuff1[i]=0;//���0
			}
		}
	}else 
	{
		wavwitchbuf=1;
		if((audio_Info.status&0X01)==0)
		{
			for(i=0;i<WAV_I2S_TX_DMA_BUFSIZE;i++)//��ͣ
			{
				audio_Info.i2sBuff2[i]=0;//���0
			}
		}
	}
	wavtransferend=1;
} 
//�õ���ǰ����ʱ��
//fx:�ļ�ָ��
//wavx:wav���ſ�����
void wav_get_curtime(FIL*fx,__wavctrl *wavx)
{
	long long fpos;  	
 	wavx->totsec=wavx->datasize/(wavx->bitrate/8);	//�����ܳ���(��λ:��) 
	fpos=fx->fptr-wavx->datastart; 					//�õ���ǰ�ļ����ŵ��ĵط� 
	wavx->cursec=fpos*wavx->totsec/wavx->datasize;	//��ǰ���ŵ��ڶ�������?	
}
//wav�ļ�������˺���
//pos:��Ҫ��λ�����ļ�λ��
//����ֵ:��ǰ�ļ�λ��(����λ��Ľ��)
uint32_t wav_file_seek(uint32_t pos)
{
	uint8_t temp;
	if(pos>audio_Info.file->fsize)
	{
		pos=audio_Info.file->fsize;
	}
	if(pos<wavctrl.datastart)pos=wavctrl.datastart;
	if(wavctrl.bps==16)temp=8;	//������8�ı���
	if(wavctrl.bps==24)temp=12;	//������12�ı��� 
	if((pos-wavctrl.datastart)%temp)
	{
		pos+=temp-(pos-wavctrl.datastart)%temp;
	}
	f_lseek(audio_Info.file,pos);
	return audio_Info.file->fptr;
}
//����ĳ��WAV�ļ�
//fname:wav�ļ�·��.
//����ֵ:0,�����������
//[b7]:0,����״̬;1,����״̬
//[b6:0]:b7=0ʱ,��ʾ������
//       b7=1ʱ,��ʾ�д���(���ﲻ�ж��������,0X80~0XFF,�����Ǵ���)
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
		res=wav_decode_init(fname,&wavctrl);//�õ��ļ�����Ϣ
		if(res==0)//�����ļ��ɹ�
		{
			printf("�����ļ��ɹ�,���ڲ���...\r\n");
			if(wavctrl.bps==16)
			{
				bsp_audio_I2SConfig(2,0);	//�����ֱ�׼,16λ���ݳ���
				bsp_i2s_Config(0,2,0,1);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����  
			}else if(wavctrl.bps==24)
			{
				bsp_audio_I2SConfig(2,2);	//�����ֱ�׼,24λ���ݳ���
				bsp_i2s_Config(0,2,0,2);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,24λ��չ֡����
			}
			bsp_i2s_SampleRateSet(wavctrl.samplerate);//���ò�����
			bsp_i2s_TX_DMAConfig(audio_Info.i2sBuff1,audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE/2); //����TX DMA
			i2s_tx_callback=wav_i2s_dma_tx_callback;			//�ص�����ָwav_i2s_dma_callback
			app_audio_Stop();
			res=f_open(audio_Info.file,(TCHAR*)fname,FA_READ);	//���ļ�
	
			if(res==0)
			{
				printf("open file successful\r\n");
				
				f_lseek(audio_Info.file, wavctrl.datastart);		//�����ļ�ͷ
				fillnum=wav_buffill(audio_Info.i2sBuff1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				fillnum=wav_buffill(audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				app_audio_Start();  
				printf("start...\r\n");
				while(1)
				{ 
					while(wavtransferend==0)//�ȴ�wav�������;
					{
						//delay_ms(1000/OS_TICKS_PER_SEC);
					//	bsp_tim_DelayMs(1000/OS_CFG_TICK_RATE_HZ);
		
					}
					wavtransferend=0;
					
 					if(wavwitchbuf)fillnum=wav_buffill(audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//���buf2
					else fillnum=wav_buffill(audio_Info.i2sBuff1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//���buf1
					
 					while(1)//����������
					{		 
						printf("playing\r\n");
						wav_get_curtime(audio_Info.file,&wavctrl);//�õ���ʱ��͵�ǰ���ŵ�ʱ�� 
						audio_Info.audioTime=wavctrl.totsec;			//��������
						audio_Info.curTime=wavctrl.cursec;
						audio_Info.bitrate=wavctrl.bitrate;
						audio_Info.samplerate=wavctrl.samplerate;
						audio_Info.bps=wavctrl.bps; 
						
  					if(audio_Info.status&0X01)break;//û�а�����ͣ 
						else bsp_tim_DelayMs(1000/OS_CFG_TICK_RATE_HZ);
					}
					if((audio_Info.status&(1<<1))==0||(fillnum!=WAV_I2S_TX_DMA_BUFSIZE))//�����������/�������
					{  
						break;
					} 					
				}
				app_audio_Stop();
				printf("stop\r\n");
			}else res=AP_ERR; 
		}else res=AP_ERR;
	}else res=AP_ERR; 
	bsp_mem_Free(SRAMIN,audio_Info.tempBuff);	//�ͷ��ڴ�
	bsp_mem_Free(SRAMIN,audio_Info.i2sBuff1);//�ͷ��ڴ�
	bsp_mem_Free(SRAMIN,audio_Info.i2sBuff2);//�ͷ��ڴ� 
	bsp_mem_Free(SRAMIN,audio_Info.file);	//�ͷ��ڴ� 
	return res;
} 
	









