# include "app_audio.h"
# include "bsp_i2s.h"



Audio_Str audio_Info;

/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void app_audio_Start(void)
{
	audio_Info.status=3<<0;//��ʼ����+����ͣ
	bsp_i2s_PlayStart();
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void app_audio_Stop(void)
{
	audio_Info.status=0;
	bsp_i2s_PlayStop();
}

/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
void app_audio_Play(void)
{
//	u8 res;
// 	DIR wavdir;	 		//Ŀ¼
//	FILINFO wavfileinfo;//�ļ���Ϣ
//	u8 *fn;   			//���ļ���
//	u8 *pname;			//��·�����ļ���
//	u16 totwavnum; 		//�����ļ�����
//	u16 curindex;		//ͼƬ��ǰ����
//	u8 key;				//��ֵ		  
// 	u16 temp;
//	u16 *wavindextbl;	//����������
//	
	bsp_audio_ADDAConfig(1,0);	//����DAC
	bsp_audio_InputConfig(0,0,0);//�ر�����ͨ��
	bsp_audio_OutputConfig(1,0);	//����DAC���   
									  
								   
//  	wavfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
//	wavfileinfo.lfname=bsp_Malloc(SRAMIN,wavfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
// 	pname=bsp_Malloc(SRAMIN,wavfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
// 	wavindextbl=bsp_Malloc(SRAMIN,2*totwavnum);				//����2*totwavnum���ֽڵ��ڴ�,���ڴ�������ļ�����
// 	while(wavfileinfo.lfname==NULL||pname==NULL||wavindextbl==NULL)//�ڴ�������
// 	{	    
//		
//	}  	 


//	while(res==FR_OK)//�򿪳ɹ�
//	{	
//		dir_sdi(&wavdir,wavindextbl[curindex]);			//�ı䵱ǰĿ¼����	   
//        res=f_readdir(&wavdir,&wavfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
//        if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
//     	fn=(u8*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
//		strcpy((char*)pname,"0:/MUSIC/");				//����·��(Ŀ¼)
//		strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
// 		LCD_Fill(60,190,240,190+16,WHITE);				//���֮ǰ����ʾ
//		Show_Str(60,190,240-60,16,fn,16,0);				//��ʾ�������� 
//		audio_index_show(curindex+1,totwavnum);

//	while(1)
			app_audio_PlayAudio("1:/login.wav"); 			 		//���������Ƶ�ļ�
//		if(key==KEY2_PRES)		//��һ��
//		{
//			if(curindex)curindex--;
//			else curindex=totwavnum-1;
// 		}else if(key==KEY0_PRES)//��һ��
//		{
//			curindex++;		   	
//			if(curindex>=totwavnum)curindex=0;//��ĩβ��ʱ��,�Զ���ͷ��ʼ
// 		}else break;	//�����˴��� 	 
//	} 											  
//	myfree(SRAMIN,wavfileinfo.lfname);	//�ͷ��ڴ�			    
//	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
//	myfree(SRAMIN,wavindextbl);			//�ͷ��ڴ�
	
}


/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint8_t app_audio_PlayAudio(uint8_t *fname)
{
		u8 res;  
//	res=f_typetell(fname); 
//	switch(res)
//	{
//		case T_WAV:
			res=wav_play_song(fname);
//			break;
//		default://�����ļ�,�Զ���ת����һ��
//			printf("can't play:%s\r\n",fname);
//			res=KEY0_PRES;
//			break;
//	}
	return res;
}



/*
*********************************************************************************************************
*                                          
*
* Description: 
*             
* Arguments : 
*
* Note(s)   : 
*********************************************************************************************************
*/
uint16_t app_audio_GetAudios(uint8_t *path)
{
	return 0;
}
