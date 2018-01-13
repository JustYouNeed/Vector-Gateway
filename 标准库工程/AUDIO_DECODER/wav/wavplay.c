# include "wavplay.h" 
# include "includes.h"
# include "app_audio.h"
# include "bsp_i2s.h"
# include "bsp.h"
# include "ff.h"



__wavctrl wavctrl;		//WAV���ƽṹ��
vu8 wavtransferend=0;	//i2s������ɱ�־
vu8 wavwitchbuf=0;		//i2sbufxָʾ��־
 
//WAV������ʼ��
//fname:�ļ�·��+�ļ���
//wavx:wav ��Ϣ��Žṹ��ָ��
//����ֵ:0,�ɹ�;1,���ļ�ʧ��;2,��WAV�ļ�;3,DATA����δ�ҵ�.
u8 wav_decode_init(u8* fname,__wavctrl* wavx)
{
	FIL*ftemp;
	u8 *buf; 
	u32 br=0;
	u8 res=0;
	
	ChunkRIFF *riff;
	ChunkFMT *fmt;
	ChunkFACT *fact;
	ChunkDATA *data;
	ftemp = (FIL*)bsp_Malloc(SRAMIN,sizeof(FIL));
	buf = bsp_Malloc(SRAMIN,512);

	if(ftemp&&buf)	//�ڴ�����ɹ�
	{
		res=f_open(ftemp,(TCHAR*)fname,FA_READ);//���ļ�
		if(res==FR_OK)
		{
			bsp_UsartPrintf(COM3, "open ok\r\n");
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
					 
//					printf("wavx->audioformat:%d\r\n",wavx->audioformat);
//					printf("wavx->nchannels:%d\r\n",wavx->nchannels);
//					printf("wavx->samplerate:%d\r\n",wavx->samplerate);
//					printf("wavx->bitrate:%d\r\n",wavx->bitrate);
//					printf("wavx->blockalign:%d\r\n",wavx->blockalign);
//					printf("wavx->bps:%d\r\n",wavx->bps);
//					printf("wavx->datasize:%d\r\n",wavx->datasize);
//					printf("wavx->datastart:%d\r\n",wavx->datastart);  
				}else res=3;//data����δ�ҵ�.
			}else res=2;//��wav�ļ�
			
		}else res=1;//���ļ�����
	}
	f_close(ftemp);
	bsp_Free(SRAMIN,ftemp);//�ͷ��ڴ�
	bsp_Free(SRAMIN,buf); 
	return 0;
}

//���buf
//buf:������
//size:���������
//bits:λ��(16/24)
//����ֵ:���������ݸ���
u32 wav_buffill(u8 *buf,u16 size,u8 bits)
{
	u16 readlen=0;
	u32 bread;
	u16 i;
	u8 *p;
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
	u16 i;
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
u32 wav_file_seek(u32 pos)
{
	u8 temp;
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
u8 wav_play_song(u8* fname)
{
	u8 res;  
	u32 fillnum; 
	audio_Info.file=(FIL*)bsp_Malloc(SRAMIN,sizeof(FIL));
	audio_Info.i2sBuff1=bsp_Malloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audio_Info.i2sBuff2=bsp_Malloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audio_Info.tempBuff=bsp_Malloc(SRAMIN,WAV_I2S_TX_DMA_BUFSIZE);
	audio_Info.file_seek=wav_file_seek;
	if(audio_Info.file&&audio_Info.i2sBuff1&&audio_Info.i2sBuff2&&audio_Info.tempBuff)
	{ 
		res=wav_decode_init(fname,&wavctrl);//�õ��ļ�����Ϣ
		if(res==0)//�����ļ��ɹ�
		{
			if(wavctrl.bps==16)
			{
				bsp_audio_I2SConfig(2,0);	//�����ֱ�׼,16λ���ݳ���
				bsp_i2s_Config(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_16bextended);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,16λ��չ֡����  
			}else if(wavctrl.bps==24)
			{
				bsp_audio_I2SConfig(2,2);	//�����ֱ�׼,24λ���ݳ���
				bsp_i2s_Config(I2S_Standard_Phillips,I2S_Mode_MasterTx,I2S_CPOL_Low,I2S_DataFormat_24b);		//�����ֱ�׼,��������,ʱ�ӵ͵�ƽ��Ч,24λ��չ֡����
			}
			bsp_i2s_SampleRateSet(wavctrl.samplerate);//���ò�����
			bsp_i2s_TX_DMAConfig(audio_Info.i2sBuff1,audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE/2); //����TX DMA
			i2s_tx_callback=wav_i2s_dma_tx_callback;			//�ص�����ָwav_i2s_dma_callback
			app_audio_Stop();
			res=f_open(audio_Info.file,(TCHAR*)fname,FA_READ);	//���ļ�
			if(res==0)
			{
				f_lseek(audio_Info.file, wavctrl.datastart);		//�����ļ�ͷ
				fillnum=wav_buffill(audio_Info.i2sBuff1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				fillnum=wav_buffill(audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);
				app_audio_Start();  
				while(1)
				{ 
					while(wavtransferend==0)//�ȴ�wav�������;
					{
						bsp_TimerDelayMs(1000/OSCfg_TickRate_Hz);
					}
					wavtransferend=0;
 					if(wavwitchbuf)fillnum=wav_buffill(audio_Info.i2sBuff2,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//���buf2
					else fillnum=wav_buffill(audio_Info.i2sBuff1,WAV_I2S_TX_DMA_BUFSIZE,wavctrl.bps);//���buf1
 					while(audio_Info.status&(1<<1))//����������
					{		 
						wav_get_curtime(audio_Info.file,&wavctrl);//�õ���ʱ��͵�ǰ���ŵ�ʱ�� 
						audio_Info.audioTime=wavctrl.totsec;			//��������
						audio_Info.curTime=wavctrl.cursec;
						audio_Info.bitrate=wavctrl.bitrate;
						audio_Info.samplerate=wavctrl.samplerate;
						audio_Info.bps=wavctrl.bps; 
  						if(audio_Info.status&0X01)break;//û�а�����ͣ 
						else bsp_TimerDelayMs(1000/OSCfg_TickRate_Hz);
					}
					if((audio_Info.status&(1<<1))==0||(fillnum!=WAV_I2S_TX_DMA_BUFSIZE))//�����������/�������
					{  
						break;
					} 					
				}
				app_audio_Stop(); 
			}else res=AP_ERR; 
		}else res=AP_ERR;
	}else res=AP_ERR; 
	bsp_Free(SRAMIN,audio_Info.tempBuff);	//�ͷ��ڴ�
	bsp_Free(SRAMIN,audio_Info.i2sBuff1);//�ͷ��ڴ�
	bsp_Free(SRAMIN,audio_Info.i2sBuff2);//�ͷ��ڴ� 
	bsp_Free(SRAMIN,audio_Info.file);	//�ͷ��ڴ� 
	return res;
} 
	









