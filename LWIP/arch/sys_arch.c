# include "os_cfg_app.h"
# include "lwip/mem.h"
# include "arch/sys_arch.h"
# include "lwip/pbuf.h"
# include "lwip/sys.h"
# include "bsp.h"



//当消息指针为空时,指向一个常量pvNullPointer所指向的值.
//在UCOS中如果OSQPost()中的msg==NULL会返回一条OS_ERR_POST_NULL
//错误,而在lwip中会调用sys_mbox_post(mbox,NULL)发送一条空消息,我们
//在本函数中把NULL变成一个常量指针0Xffffffff
const void * const pvNullPointer = (mem_ptr_t*)0xffffffff;
 
 
 
 
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
int sys_sem_valid(sys_sem_t *sem)
{
	if(sem->NamePtr)
		return (strcmp(sem->NamePtr,"?SEM"))? 1:0;
	else
		return 0;           
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
void sys_sem_set_invalid(sys_sem_t *sem)
{
	if(sys_sem_valid(sem))
     sys_sem_free(sem);
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
int sys_mbox_valid(sys_mbox_t *mbox)
{  
	if(mbox->NamePtr)  
		return (strcmp(mbox->NamePtr,"?Q"))? 1:0;
	else
		return 0;
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
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
	if(sys_mbox_valid(mbox))
    sys_mbox_free(mbox);
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
void sys_init(void)
{
	
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
err_t sys_sem_new(sys_sem_t * sem, u8_t count)
{
	OS_ERR err;
	OSSemCreate ((OS_SEM*	)sem,
                 (CPU_CHAR*	)"LWIP Sem",
                 (OS_SEM_CTR)count,		
                 (OS_ERR*	)&err);
	if(err!=OS_ERR_NONE)return ERR_MEM; 
	LWIP_ASSERT("OSSemCreate ",sem != NULL );
	return ERR_OK;
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
void sys_sem_free(sys_sem_t *sem)
{
	OS_ERR err;
	OSSemDel(sem,OS_OPT_DEL_ALWAYS,&err);
	LWIP_ASSERT("OSSemDel ",err==OS_ERR_NONE);
	sem = NULL;
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
void sys_sem_signal(sys_sem_t * sem)
{
	OS_ERR err;
	OSSemPost(sem,OS_OPT_POST_1,&err);//发送信号量
	LWIP_ASSERT("OSSemPost ",err == OS_ERR_NONE ); 
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
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
	OS_ERR err;
	u32_t ucos_timeout, timeout_new; 
	if(	timeout!=0) 
	{
		ucos_timeout = (timeout * OS_CFG_TICK_RATE_HZ) / 1000;//转换为节拍数,因为UCOS延时使用的是节拍数,而LWIP是用ms
		if(ucos_timeout < 1)
		ucos_timeout = 1;
	}else ucos_timeout = 0; 
	timeout = OSTimeGet(&err);  
	OSSemPend(sem,ucos_timeout,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
 	if(err == OS_ERR_TIMEOUT)timeout=SYS_ARCH_TIMEOUT;//请求超时	
	else
	{     
 		timeout_new = OSTimeGet(&err); 
		if (timeout_new>=timeout) timeout_new = timeout_new - timeout;
		else timeout_new = 0xffffffff - timeout + timeout_new;
 		timeout = (timeout_new*1000/OS_CFG_TICK_RATE_HZ + 1);//算出请求消息或使用的时间(ms)
	}
	return timeout;
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
err_t sys_mbox_new( sys_mbox_t *mbox, int size)
{
	
	OS_ERR err;
	if(size>MAX_QUEUE_ENTRIES)size=MAX_QUEUE_ENTRIES;		//消息队列最多容纳MAX_QUEUE_ENTRIES消息数目 
	OSQCreate((OS_Q*		)mbox,				//消息队列
              (CPU_CHAR*	)"LWIP Quiue",		//消息队列名称
              (OS_MSG_QTY	)size,				//消息队列长度
              (OS_ERR*		)&err);				//错误码
	if(err==OS_ERR_NONE) return ERR_OK;
	return ERR_MEM;
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
void sys_mbox_free(sys_mbox_t * mbox)
{
	OS_ERR err;
	
#if OS_CFG_Q_FLUSH_EN > 0u  
	OSQFlush(mbox,&err);
#endif
	
	OSQDel((OS_Q*	)mbox,
           (OS_OPT	)OS_OPT_DEL_ALWAYS,
           (OS_ERR*	)&err);
	LWIP_ASSERT( "OSQDel ",err == OS_ERR_NONE ); 
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
void sys_mbox_post(sys_mbox_t *mbox,void *msg)
{    
	OS_ERR err;
	CPU_INT08U i=0;
	if(msg==NULL)msg=(void*)&pvNullPointer;	//当msg为空时 msg等于pvNullPointer指向的值 
	//发送消息
  while(i<10)	//试10次
	{
		OSQPost((OS_Q*		)mbox,		
			    (void*		)msg,
			    (OS_MSG_SIZE)strlen(msg),
			    (OS_OPT		)OS_OPT_POST_FIFO,
			    (OS_ERR*	)&err);
		if(err==OS_ERR_NONE) break;
		i++;
		OSTimeDlyHMSM(0,0,0,5,OS_OPT_TIME_HMSM_STRICT,&err); //延时5ms
	}
	LWIP_ASSERT( "sys_mbox_post error!\n", i !=10 );  	
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
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{ 
	OS_ERR err;
	if(msg==NULL)msg=(void*)&pvNullPointer;//当msg为空时 msg等于pvNullPointer指向的值 
	OSQPost((OS_Q*		)mbox,		
			(void*		)msg,
			(OS_MSG_SIZE)sizeof(msg),
			(OS_OPT		)OS_OPT_POST_FIFO,
			(OS_ERR*	)&err);
	if(err!=OS_ERR_NONE) return ERR_MEM;
	return ERR_OK;
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
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
	OS_ERR err;
	OS_MSG_SIZE size;
	u32_t ucos_timeout,timeout_new;
	void *temp;
	if(timeout!=0)
	{
		ucos_timeout=(timeout*OS_CFG_TICK_RATE_HZ)/1000; //转换为节拍数,因为UCOS延时使用的是节拍数,而LWIP是用ms
		if(ucos_timeout<1)ucos_timeout=1;//至少1个节拍
	}else ucos_timeout = 0; 
	timeout = OSTimeGet(&err); //获取系统时间 
	//请求消息
	temp=OSQPend((OS_Q*			)mbox,   
				(OS_TICK		)ucos_timeout,
                (OS_OPT			)OS_OPT_PEND_BLOCKING,
                (OS_MSG_SIZE*	)&size,		
                (CPU_TS*		)0,
                (OS_ERR*		)&err);
	if(msg!=NULL)
	{	
		if(temp==(void*)&pvNullPointer)*msg = NULL;   	//因为lwip发送空消息的时候我们使用了pvNullPointer指针,所以判断pvNullPointer指向的值
 		else *msg=temp;									//就可知道请求到的消息是否有效
	}    
	if(err==OS_ERR_TIMEOUT)timeout=SYS_ARCH_TIMEOUT;  //请求超时
	else
	{
		LWIP_ASSERT("OSQPend ",err==OS_ERR_NONE); 
		timeout_new=OSTimeGet(&err);
		if (timeout_new>=timeout) timeout_new = timeout_new - timeout;//算出请求消息或使用的时间
		else timeout_new = 0xffffffff - timeout + timeout_new; 
		timeout=timeout_new*1000/OS_CFG_TICK_RATE_HZ + 1; 
	}
	return timeout; 
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
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
	return sys_arch_mbox_fetch(mbox,msg,1);//尝试获取一个消息
}



extern CPU_STK * TCPIP_THREAD_TASK_STK;//TCP IP内核任务堆栈,在lwip_comm函数定义
//LWIP内核任务的任务控制块
OS_TCB TcpipthreadTaskTCB;

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
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	if(strcmp(name,TCPIP_THREAD_NAME)==0)//创建TCP IP内核任务
	{
		OS_CRITICAL_ENTER();	//进入临界区			 
		//创建开始任务
		OSTaskCreate((OS_TCB 	* )&TcpipthreadTaskTCB,			//任务控制块
					 (CPU_CHAR	* )"TCPIPThread task", 			//任务名字
                     (OS_TASK_PTR )thread, 						//任务函数
                     (void		* )0,							//传递给任务函数的参数
                     (OS_PRIO	  )prio,     					//任务优先级
                     (CPU_STK   * )&TCPIP_THREAD_TASK_STK[0],	//任务堆栈基地址
                     (CPU_STK_SIZE)stacksize/10,				//任务堆栈深度限位
                     (CPU_STK_SIZE)stacksize,					//任务堆栈大小
                     (OS_MSG_QTY  )0,							//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                     (OS_TICK	  )0,							//当使能时间片轮转时的时间片长度，为0时为默认长度，
                     (void   	* )0,							//用户补充的存储区
                     (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                     (OS_ERR 	* )&err);					//存放该函数错误时的返回值
		OS_CRITICAL_EXIT();	//退出临界区
	} 
	return 0;
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
u32_t sys_now(void)
{
	OS_ERR err;
	u32_t ucos_time, lwip_time;
	ucos_time = OSTimeGet(&err);
	lwip_time = (ucos_time * 1000 / OS_CFG_TICK_RATE_HZ+1);
	return lwip_time;
}

void sys_msleep(u32_t ms)
{
	bsp_tim_DelayMs(ms);
}

