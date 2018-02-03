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
	audio_Info.status=3<<0;//开始播放+非暂停
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
	uint8_t res;
 	DIR wavdir;	 		//目录
	FILINFO wavfileinfo;//文件信息
	uint8_t *fn;   			//长文件名
	uint8_t *pname;			//带路径的文件名
	uint16_t totwavnum; 		//音乐文件总数
	uint16_t curindex;		//图片当前索引
	uint8_t key;				//键值		  
 	uint16_t temp;
	uint16_t *wavindextbl;	//音乐索引表
	uint8_t f_opt;
		
	bsp_audio_ADDAConfig(1,0);	//开启DAC
	bsp_audio_InputConfig(0,0,0);//关闭输入通道
	bsp_audio_OutputConfig(1,0);	//开启DAC输出   
									  
	f_opt = f_opendir(&wavdir, "1:/MUSIC");
	if(f_opt != FR_OK)
	{
		printf("open dir failed, return\r\n");
		return;
	}
	
//	wavfileinfo.lfsize=_MAX_LFN*2+1;						//长文件名最大长度
//	wavfileinfo.lfname=bsp_mem_Malloc(SRAMIN,wavfileinfo.lfsize);	//为长文件缓存区分配内存
// 	pname=bsp_mem_Malloc(SRAMIN,wavfileinfo.lfsize);				//为带路径的文件名分配内存
// 	wavindextbl=bsp_mem_Malloc(SRAMIN,2*totwavnum);				//申请2*totwavnum个字节的内存,用于存放音乐文件索引
// 	while(wavfileinfo.lfname==NULL||pname==NULL||wavindextbl==NULL)//内存分配出错
// 	{	    
//		printf("mem eror, return\r\n");
//		return;
//	}  	 

//	f_opt = f_open();
//	while(res==FR_OK)//打开成功
//	{	
//		dir_sdi(&wavdir,wavindextbl[curindex]);			//改变当前目录索引	   
//        res=f_readdir(&wavdir,&wavfileinfo);       		//读取目录下的一个文件
//        if(res!=FR_OK||wavfileinfo.fname[0]==0)break;	//错误了/到末尾了,退出
//     	fn=(uint8_t*)(*wavfileinfo.lfname?wavfileinfo.lfname:wavfileinfo.fname);			 
//		strcpy((char*)pname,"0:/MUSIC/");				//复制路径(目录)
//		strcat((char*)pname,(const char*)fn);  			//将文件名接在后面
// 		LCD_Fill(60,190,240,190+16,WHITE);				//清除之前的显示
//		Show_Str(60,190,240-60,16,fn,16,0);				//显示歌曲名字 
//		audio_index_show(curindex+1,totwavnum);

//	while(1)
			app_audio_PlayAudio("1:/login.wav"); 			 		//播放这个音频文件
//		if(key==KEY2_PRES)		//上一曲
//		{
//			if(curindex)curindex--;
//			else curindex=totwavnum-1;
// 		}else if(key==KEY0_PRES)//下一曲
//		{
//			curindex++;		   	
//			if(curindex>=totwavnum)curindex=0;//到末尾的时候,自动从头开始
// 		}else break;	//产生了错误 	 
//	} 											  
//	myfree(SRAMIN,wavfileinfo.lfname);	//释放内存			    
//	myfree(SRAMIN,pname);				//释放内存			    
//	myfree(SRAMIN,wavindextbl);			//释放内存
	
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
		uint8_t res;  
//	res=f_typetell(fname); 
//	switch(res)
//	{
//		case T_WAV:
		bsp_audio_ADDAConfig(1,0);	//开启DAC
	bsp_audio_InputConfig(0,0,0);//关闭输入通道
	bsp_audio_OutputConfig(1,0);	//开启DAC输出   
			res=wav_play_song(fname);
//			break;
//		default://其他文件,自动跳转到下一曲
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
