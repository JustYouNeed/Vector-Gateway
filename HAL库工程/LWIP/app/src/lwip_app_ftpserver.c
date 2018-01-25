# include "lwip_app_ftpserver.h"
# include "stdio.h"
# include "string.h"

#define FTP_PORT			21
#define FTP_SRV_ROOT		"/"
#define FTP_MAX_CONNECTION	2
#define FTP_USER			"rtt"
#define FTP_PASSWORD		"demo"
#define FTP_WELCOME_MSG		"220-= welcome on RT-Thread FTP server =-\r\n220 \r\n"
#define FTP_BUFFER_SIZE		1024

static struct ftp_session* session_list = NULL;

# define FTP_TASK_PRIO		7
# define FTP_TASK_STK_SIZE	1024
OS_TCB FTPTestTaskTCB;
CPU_STK FTP_TASK_STK[FTP_TASK_STK_SIZE];

char REPLY_BUFF[FTP_BUFFER_SIZE];


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
struct ftp_session* ftp_new_session()
{
	struct ftp_session* session;

	session = (struct ftp_session*)bsp_mem_Malloc(SRAMIN, sizeof(struct ftp_session));

	session->next = session_list;
	session_list = session;

	return session;
}


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void ftp_close_session(struct ftp_session* session)
{
	struct ftp_session* list;

	if (session_list == session)
	{
		session_list = session_list->next;
		session->next = NULL;
	}
	else
	{
		list = session_list;
		while (list->next != session) list = list->next;

		list->next = session->next;
		session->next = NULL;
	}

	bsp_mem_Free(SRAMIN, session);
}


/*
*********************************************************************************************************
*                                        ftp_CmdCompare  
*
* Description: 比较ftp会话中的命令是否存在
*             
* Arguments  : 1> src:客户端发送过来的指令
*              2> cmd:需要匹配的命令
*
* Reutrn     : 1> 0:找到命令
*              2> -1:命令不存在
*
* Note(s)    : None.
*********************************************************************************************************
*/
int ftp_CmdCompare(char *src, char *cmd)
{
	while(*cmd)  
	{
		if(*src == 0) return -1;
		if(*cmd != *src) return -1;
		src++;
		cmd++;
	}
	return 0;
}

/*
*********************************************************************************************************
*                                          ftp_GetParameter
*
* Description: 用于获取ftp会话中客户端发送过来的参数
*             
* Arguments  : buff:接收的数据
*
* Reutrn     : 指向参数的字符串指针
*
* Note(s)    : None.
*********************************************************************************************************
*/
char *ftp_GetParameter(char *buff)
{
	char *parameter_ptr, *ptr;
	
	ptr = buff;
	
	while (*ptr)  /*  找到最后结束的地方,因为参数不包括最后\r\n的部分  */
	{
		if (*ptr == '\r' || *ptr == '\n') *ptr = 0;
		ptr ++;
	}
	
	parameter_ptr = strchr(buff, ' ');  /*  参数与命令相隔一个空格  */
	if (parameter_ptr != NULL) parameter_ptr ++; /*  空格的位置往后一个才是参数的位置  */
	
	return parameter_ptr;
}

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t ftp_GetCmd(char *buff)
{
	if(ftp_CmdCompare(buff, "USER") == 0) return USER;
	else if(ftp_CmdCompare(buff, "PASS") == 0) return PASS;
	else if(ftp_CmdCompare(buff, "LIST") == 0) return LIST;
	else if(ftp_CmdCompare(buff, "NLST") == 0) return NLST;
	else if(ftp_CmdCompare(buff, "PWD") == 0) return PWD;
	else if(ftp_CmdCompare(buff, "TYPE") == 0) return TYPE;
	else if(ftp_CmdCompare(buff, "PASV") == 0) return PASV;
	else if(ftp_CmdCompare(buff, "RETR") == 0) return RETR;
	else if(ftp_CmdCompare(buff, "STOR") == 0) return STOR;
	else if(ftp_CmdCompare(buff, "SIZE") == 0) return SIZE;
	else if(ftp_CmdCompare(buff, "MDTM") == 0) return MDTM;
	else if(ftp_CmdCompare(buff, "SYST") == 0) return SYST;
	else if(ftp_CmdCompare(buff, "CWD") == 0) return CWD;
	else if(ftp_CmdCompare(buff, "CDUP") == 0) return CDUP;
	else if(ftp_CmdCompare(buff, "PORT") == 0) return PORT;
	else if(ftp_CmdCompare(buff, "REST") == 0) return REST;
	else if(ftp_CmdCompare(buff, "MKD") == 0) return MKD;
	else if(ftp_CmdCompare(buff, "DELE") == 0) return DELE;
	else if(ftp_CmdCompare(buff, "RMD") == 0) return RMD;
	else if(ftp_CmdCompare(buff, "QUIT") == 0) return QUIT;
	else if(ftp_CmdCompare(buff, "SITE") == 0) return SITE;
	else if(ftp_CmdCompare(buff, "UTIME") == 0) return UTIME;
	else return NONE;
}

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t ftp_UserVerify(struct ftp_session* session, char *para)
{
	memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
	
	if(strcmp(para, "anonymous") == 0)  /*  匿名连接  */
	{
		session->is_anonymous = TRUE;
		sprintf(REPLY_BUFF, "331 Anonymous login OK send e-mail address for password.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		
	}
	else if (strcmp(para, FTP_USER) == 0)  /*  用户名正确  */
	{
		session->is_anonymous = FALSE;		
		sprintf(REPLY_BUFF, "331 Password required for %s\r\n", para);
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	}
	else  /*  用户名错误  */
	{
		sprintf(REPLY_BUFF, "530 Login incorrect. Bye.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		return 1;
	}
	return 0;
}

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t ftp_PasswordVerify(struct ftp_session* session, char *para)
{	
	if(strcmp(para, FTP_PASSWORD) == 0 || session->is_anonymous)
	{
		memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
		
		sprintf(REPLY_BUFF, "230 User Logged in\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		return 0;
	}
	return 1;
}

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t ftp_EnterPassiveMode(struct ftp_session* session, char *para)
{
	int dig1, dig2;
	int sockfd;
	int optval = 1;
	fd_set readfds;
	struct sockaddr_in local, pasvremote;
	struct timeval tv;
	socklen_t addr_len = sizeof(struct sockaddr_in);

	tv.tv_sec=3, tv.tv_usec=0;
	session->pasv_port = 2000;
	session->pasv_active = 1;

	local.sin_family=AF_INET;
	local.sin_port=htons(session->pasv_port);
	local.sin_addr.s_addr=INADDR_ANY;

	dig1 = (int)(session->pasv_port/256);
	dig2 = session->pasv_port % 256;

	FD_ZERO(&readfds);
	printf("build socket...\r\n");
	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	printf("sockfd:%d\r\n", sockfd);
	if(sockfd == -1)
	{
		memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
		printf("build socket failed\r\n");
		sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		goto err1;
	}
	printf("set socket...\r\n");
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))==-1)
	{
		memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
		printf("set socket failed\r\n");
		sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		goto err1;
	}
	printf("bind socket...\r\n");
	if(bind(sockfd, (struct sockaddr *)&local, addr_len)==-1)
	{
		memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
		printf("bind socket failed\r\n");
		sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		goto err1;
	}
	
	if(listen(sockfd, 1)==-1)
	{
		memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
		printf("listen socket failed\r\n");
		sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		goto err1;
	}
//		printf("Listening %d seconds @ port %d\n", tv.tv_sec, session->pasv_port);
	memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
	sprintf(REPLY_BUFF, "227 Entering passive mode (%d,%d,%d,%d,%d,%d)\r\n", 127, 0, 0, 1, dig1, dig2);
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	FD_SET(sockfd, &readfds);
	select(0, &readfds, 0, 0, &tv);
	if(FD_ISSET(sockfd, &readfds))
	{
		if((session->pasv_sockfd = accept(sockfd, (struct sockaddr*)&pasvremote, &addr_len))==-1)
		{
			memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
			sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
			send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
			goto err1;
		}
		else
		{
//			printf("Got Data(PASV) connection from %s\n", inet_ntoa(pasvremote.sin_addr));
			session->pasv_active = 1;
			closesocket(sockfd);
		}
	}
	else
	{
err1:
		closesocket(session->pasv_sockfd);
		session->pasv_active = 0;
		return 1;
	}
	
	return 0;
}
uint8_t is_absolute_path(char* path)
{
	if (path[0] == '0' && path[1] == ':') return TRUE;
	return TRUE;
}

int build_full_path(struct ftp_session* session, char* path, char* new_path, size_t size)
{
	printf("path:%s newpath:%s\r\n", path, new_path);
	if (is_absolute_path(path) == TRUE)
	{
		strcpy(new_path, path);
	}
	else
	{
		sprintf(new_path, "%s%s", session->currentdir, path);
	}

	return 0;
}

uint8_t ftp_EnterPositiveMode(struct ftp_session* session, char *para)
{
	int i;
	int portcom[6];
	char tmpip[100];
	fd_set readfds;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	struct sockaddr_in pasvremote;
	i=0;
	
//	printf("start port connect...para:%s\r\n", para);
	
	portcom[i++]=atoi(strtok(para, ".,;()"));
//	printf("start port connect...\r\n");
	for(;i<6;i++)
		portcom[i]=atoi(strtok(0, ".,;()"));
	
	sprintf(tmpip, "%d.%d.%d.%d", portcom[0], portcom[1], portcom[2], portcom[3]);
//	printf("ip:%d.%d.%d.%d, port:%d", portcom[0], portcom[1], portcom[2], portcom[3],portcom[4] * 256 + portcom[5]);
	FD_ZERO(&readfds);
	if((session->pasv_sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1)
	{
		memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
		sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		closesocket(session->pasv_sockfd);
		session->pasv_active = 0;
		return 0;
	}
	
	pasvremote.sin_len = sizeof(pasvremote);
	pasvremote.sin_addr.s_addr=inet_addr(tmpip);
	pasvremote.sin_port=PP_HTONS(portcom[4] * 256 + portcom[5]);
	pasvremote.sin_family=AF_INET;
	
	if(connect(session->pasv_sockfd, (struct sockaddr *)&pasvremote, sizeof(pasvremote))==-1)
	{
//		printf("connect failed\r\n");
		// is it only local address?try using gloal ip addr
		pasvremote.sin_addr=session->remote.sin_addr;
		if(connect(session->pasv_sockfd, (struct sockaddr *)&pasvremote, sizeof(pasvremote))==-1)
		{
			memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
			sprintf(REPLY_BUFF, "425 Can't open data connection.\r\n");
			send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
			closesocket(session->pasv_sockfd);
			return 0;
		}
	}
//	printf("connect ok\r\n");
	session->pasv_active=1;
	session->pasv_port = portcom[4] * 256 + portcom[5];
	memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
	sprintf(REPLY_BUFF, "200 Port Command Successful.\r\n");
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	return 0;
}

int do_list(char* directory, int sockfd)
{
	uint8_t result;
	DIR dir;
	FILINFO finfo;
	
	char *buff = bsp_mem_Malloc(SRAMIN, sizeof(char)*512);
	char *direc = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	char bufflen;
	
	sprintf(direc, "0:%s",directory);
	result = f_opendir(&dir, direc);
	if(result != FR_OK)
	{
		sprintf(REPLY_BUFF, "500 Internal Error\r\n");
		send(sockfd, REPLY_BUFF, sizeof(REPLY_BUFF), 0);
		return -1;
	}
	
	while(1)
	{
		result = f_readdir(&dir,&finfo);
		if(result != FR_OK || finfo.fname[0] == 0) break;
		sprintf(buff, "%s/%s", directory, finfo.fname);
		
		if(finfo.fattrib & AM_DIR)
		{
			bufflen = sprintf(buff, "drwxrwxr-x 2 502 504 %d Jan 1 2000 %s\r\n", (int)finfo.fsize, finfo.fname);
			send(sockfd, buff, bufflen, 0);
		}
		else
		{
			bufflen = sprintf(buff, "-rw-r--r-- 1 admin admin %d Jan 1 2000 %s\r\n", (int)finfo.fsize, finfo.fname);
			send(sockfd, buff, bufflen, 0);
		}
	}
	bsp_mem_Free(SRAMIN, buff);
	bsp_mem_Free(SRAMIN, direc);
	f_closedir(&dir);
	return 0;
}	

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t ftp_ListFile(struct ftp_session* session, char *para)
{
//	memset(REPLY_BUFF,0,FTP_BUFFER_SIZE);
	char *buff = bsp_mem_Malloc(SRAMIN, sizeof(char)*512);
	
	sprintf(buff, "150 Opening Binary mode connection for file list.\r\n");
	send(session->sockfd, buff, strlen(buff), 0);
	
	do_list(session->currentdir, session->pasv_sockfd);
	
	closesocket(session->pasv_sockfd);
	session->pasv_active = 0;
	sprintf(buff, "226 Transfert Complete.\r\n");
	send(session->sockfd, buff, strlen(buff), 0);
	bsp_mem_Free(SRAMIN, buff);
	
	return 0;
}

/*
*********************************************************************************************************
*                                          ftp_MakeDir
*
* Description: 新建一个文件夹
*             
* Arguments  : 1> session: FTP传输控制块
*              2> para: FTP客户端传送的参数
*
* Reutrn     : 1> 1: 文件夹创建失败
*              2> 0: 成功
*
* Note(s)    : None.
*********************************************************************************************************
*/
uint8_t ftp_MakeDir(struct ftp_session* session, char *para)
{
	printf("para%s\r\n", para);
	
	char bufflen;
	char *buff = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	uint8_t result = f_mkdir(para);
	
	if(result == FR_EXIST)
	{
		bufflen = sprintf(buff, "550 File \"%s\" exists.\r\n", para);
	}
	else if(result == FR_OK)
	{
		bufflen = sprintf(buff, "257 directory \"%s\" successfully created.\r\n", para);
	}
	else
		return 1;
	send(session->sockfd, buff, bufflen, 0);
	bsp_mem_Free(SRAMIN, buff);
	printf("result:%d\r\n", result);
	return 0;
}

/*
*********************************************************************************************************
*                                          ftp_ChangeWorkDir
*
* Description: 改变FTP工作目录
*             
* Arguments  :
*
* Reutrn     : 
*
* Note(s)    : 
*********************************************************************************************************
*/
uint8_t ftp_ChangeWorkDir(struct ftp_session* session, char *para)
{
	printf("Change Work dir,%s\r\n",para);
	char *filename = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	char *buff = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	char bufflen;
	build_full_path(session, para, filename, 256);

	bufflen = sprintf(buff, "250 Changed to directory \"%s\"\r\n", buff);
	send(session->sockfd, buff, bufflen, 0);
	strcpy(session->currentdir, filename);
	
	return 0;
}

uint8_t ftp_GetWorkDir(struct ftp_session* session, char *para)
{
	char bufflen = 0;
	printf("cur dir:%s\r\n", session->currentdir);
	bufflen = sprintf(REPLY_BUFF, "257 \"%s\" is current directory.\r\n", session->currentdir);
	send(session->sockfd, REPLY_BUFF, bufflen, 0);
	return 0;
}


uint8_t ftp_SetType(struct ftp_session* session, char *para)
{
	char bufflen;
	if(strcmp(para, "I")==0)
	{
		bufflen = sprintf(REPLY_BUFF, "200 Type set to binary.\r\n");
		send(session->sockfd, REPLY_BUFF, bufflen, 0);
	}
	else
	{
		bufflen = sprintf(REPLY_BUFF, "200 Type set to ascii.\r\n");
		send(session->sockfd, REPLY_BUFF, bufflen, 0);
	}
	return 0;
}

uint8_t ftp_GetSystemInfo(struct ftp_session* session, char *para)
{
	char bufflen = 0;
	bufflen = sprintf(REPLY_BUFF, "215 %s\r\n", "UCOS-III");
	send(session->sockfd, REPLY_BUFF, bufflen, 0);
	return 0;
}


uint8_t ftp_Disconnect(struct ftp_session* session, char *para)
{
	sprintf(REPLY_BUFF, "221 Bye!\r\n");
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);	
	return 	1;
}

uint8_t ftp_SetFileOffset(struct ftp_session* session, char *para)
{
	if(atoi(para)>=0)
	{
		session->offset=atoi(para);
		sprintf(REPLY_BUFF, "350 Send RETR or STOR to start transfert.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	}
	return 0;
}


uint8_t ftp_GetFileSize(struct ftp_session* session, char *para)
{
	FIL file;
	uint8_t res = FR_OK;
	char *path = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	sprintf(path, "%s/%s", session->currentdir, para);
	
	res = f_open(&file, path, FA_OPEN_EXISTING);
	if(res != FR_OK)
	{
		sprintf(REPLY_BUFF, "550 \"%s\" : not a regular file\r\n", para);
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		return 1;
	}
	sprintf(REPLY_BUFF, "213 %d\r\n", (int)file.fsize);
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	printf("get file size para:%s, dir:%s\r\n",para, session->currentdir);
	
	f_close(&file);
	bsp_mem_Free(SRAMIN, path);
	return 0;
}


uint8_t ftp_GetFileLastModTime(struct ftp_session* session, char *para)
{
	sprintf(REPLY_BUFF, "550 \"/\" : not a regular file\r\n");
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	return 0;
}

uint8_t ftp_DelFile(struct ftp_session* session, char *para)
{
	return 0;
}

uint8_t ftp_DownloadFile(struct ftp_session* session, char *para)
{
	uint8_t res;
	FIL file;
	UINT br;
	char *buff = (char*)bsp_mem_Malloc(SRAMIN, sizeof(char)*512);
	char *path = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	
	sprintf(path, "%s/%s", session->currentdir, para);
	
	res = f_open(&file, path, FA_OPEN_EXISTING | FA_READ);
	if(res != FR_OK)
	{
		sprintf(REPLY_BUFF, "550 \"%s\" : not a regular file\r\n", para);
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		session->offset=0;
		f_close(&file);
		return 1;
	}
	
	if(session->offset>0 && session->offset < file.fsize)
	{
		f_lseek(&file, session->offset);
		sprintf(REPLY_BUFF, "150 Opening binary mode data connection for partial \"%s\" (%ld/%ld bytes).\r\n",
				para, file.fsize - session->offset, file.fsize);
	}
	else
	{
		sprintf(REPLY_BUFF, "150 Opening binary mode data connection for \"%s\" (%ld bytes).\r\n", para, file.fsize);
	}
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	
	while(1)
	{
		res = f_read(&file, buff, 512, &br);
		if(br == 0 || res != FR_OK) break;
		send(session->pasv_sockfd, buff, br, 0);
		//printf("read bytes:%d\r\n", br);
	}
	sprintf(REPLY_BUFF, "226 Finished.\r\n");
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	closesocket(session->pasv_sockfd);
	f_close(&file);
	bsp_mem_Free(SRAMIN, path);
	bsp_mem_Free(SRAMIN, buff);
	return 0;
}

uint8_t ftp_UploadFile(struct ftp_session* session, char *para)
{
	struct timeval tv;
	fd_set readfds;
	UINT bw;
	uint32_t bytes;
		
	tv.tv_sec=3, tv.tv_usec=0;
	
	char *path = bsp_mem_Malloc(SRAMIN, sizeof(char)*256);
	sprintf(path, "%s%s", session->currentdir, para);
	
	int res = FR_OK;
	FIL file;
	if(session->is_anonymous == TRUE)
	{
		sprintf(REPLY_BUFF, "550 Permission denied.\r\n");
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		return 1;
	}
	
	res = f_open(&file, path, FA_CREATE_NEW);
	if(res != FR_OK)
	{
		sprintf(REPLY_BUFF, "550 Cannot open \"%s\" for writing.\r\n", para);
		send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		return 0;
	}
	
	sprintf(REPLY_BUFF, "150 Opening binary mode data connection for \"%s\".\r\n", para);
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	FD_ZERO(&readfds);
	FD_SET(session->pasv_sockfd, &readfds);
	
	while(select(session->pasv_sockfd+1, &readfds, 0, 0, &tv)>0 )
	{
		if((bytes = recv(session->pasv_sockfd, REPLY_BUFF, FTP_BUFFER_SIZE, 0))>0)
		{
			f_write(&file, REPLY_BUFF, bytes, &bw);
		}
		else if(bytes==0)
		{
			f_close(&file);
			closesocket(session->pasv_sockfd);
		  sprintf(REPLY_BUFF, "226 Finished.\r\n");
			send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
			break;
		}
		else if(bytes==-1)
		{
			f_close(&file);
			closesocket(session->pasv_sockfd);
			return 1;
		}
	}
	closesocket(session->pasv_sockfd);
	bsp_mem_Free(SRAMIN, path);
	return 0;
}


uint8_t ftp_UpdataFileTime(struct ftp_session* session, char *para)
{
	sprintf(REPLY_BUFF, "200 Finished.\r\n");
	send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
	return 0;
}

/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
int ftp_process_request(struct ftp_session *session, char *buff)
{
	uint8_t cmd = NONE;  	/*  存储接收到的指令  */
	char *parameter_ptr;	/*  存储接收到的参数  */
	
	parameter_ptr = ftp_GetParameter(buff);	/*  得到参数  */
	cmd = ftp_GetCmd(buff);   /*  得到命令  */
	
	printf("cmd:%d para:%s\r\n",cmd, parameter_ptr);
	
	memset(REPLY_BUFF, 0, FTP_BUFFER_SIZE);
	switch(cmd)
	{
		case USER: ftp_UserVerify(session, parameter_ptr); break;
		case PASS: ftp_PasswordVerify(session, parameter_ptr); break;
		case LIST: ftp_ListFile(session, parameter_ptr);break;
		case NLST: break;
		case PWD:	 ftp_GetWorkDir(session, parameter_ptr); break;
		case TYPE: ftp_SetType(session, parameter_ptr);break;
		case PASV: ftp_EnterPassiveMode(session, parameter_ptr); break;
		case RETR: ftp_DownloadFile(session, parameter_ptr); break;
		case STOR: ftp_UploadFile(session, parameter_ptr);break;
		case SIZE: ftp_GetFileSize(session, parameter_ptr);break;
		case MDTM: ftp_GetFileLastModTime(session, parameter_ptr); break;
		case SYST: ftp_GetSystemInfo(session, parameter_ptr);break;
		case CWD:  ftp_ChangeWorkDir(session, parameter_ptr);break;
		case CDUP: break;
		case PORT: ftp_EnterPositiveMode(session, parameter_ptr);break;
		case REST: ftp_SetFileOffset(session, parameter_ptr);break;
		case MKD:  ftp_MakeDir(session, parameter_ptr); break;
		case DELE: ftp_DelFile(session, parameter_ptr); break;
		case RMD: break;
		case SITE:
		{
			sprintf(REPLY_BUFF, "220 Finish\r\n");;
			send(session->sockfd, REPLY_BUFF, strlen(REPLY_BUFF), 0);
		}break;
		case UTIME: ftp_UpdataFileTime(session, parameter_ptr);break;
		case QUIT: ftp_Disconnect(session, parameter_ptr);break;
		default:break;
	}
	return 0;
}


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void lwip_app_ftp_Thread(void *p_arg)
{
	OS_ERR err;
	int numbytes;
	int sockfd, maxfdp1;
	struct sockaddr_in local;	
	fd_set readfds, tmpfds;
	struct ftp_session* session;
	socklen_t addr_len = sizeof(struct sockaddr);
	char * buffer = (char *) bsp_mem_Malloc(SRAMIN, FTP_BUFFER_SIZE);
	
	
	local.sin_port = htons(FTP_PORT);
	local.sin_family = PF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	
	FD_ZERO(&readfds);
	FD_ZERO(&tmpfds);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0)
	{
		printf("Create Socket Failed\r\n");
		OSTaskDel(&FTPTestTaskTCB, &err);
		return ;
	}
	
	bind(sockfd, (struct sockaddr *)&local, addr_len);
	listen(sockfd, FTP_MAX_CONNECTION);
	
	FD_SET(sockfd, &readfds);
	
	while(1)
	{
		/* get maximum fd */
		maxfdp1 = sockfd + 1;
		session = session_list;
		while (session != NULL)  /*    */
		{
				if (maxfdp1 < session->sockfd + 1)
							maxfdp1 = session->sockfd + 1;

				/*  将套接字加入集合  */
				FD_SET(session->sockfd, &readfds);
				session = session->next;
		}  /*  end while  */

		/*  检查套接字是否可读  */
		tmpfds=readfds;
		if (select(maxfdp1, &tmpfds, 0, 0, 0) == 0) continue;

		if(FD_ISSET(sockfd, &tmpfds))  /*  如果sockfd套接字在可读的列表里面  */
		{
			int com_socket;
			struct sockaddr_in remote;
			
			/*  等待连接,该函数会阻塞执行  */
			com_socket = accept(sockfd, (struct sockaddr*)&remote, &addr_len);
			if(com_socket == -1)  /*  没有连接则重新循环  */
			{
				continue;
			}
			else   /*  有连接  */
			{
				send(com_socket, FTP_WELCOME_MSG, strlen(FTP_WELCOME_MSG), 0); /*  发送欢迎信息  */
				
				FD_SET(com_socket, &readfds);			/*  将该连接的套接字加入到感兴趣的列表里面  */

				/*  为新的连接申请一个信息块  */
				session = ftp_new_session();
				if (session != NULL)
				{
					strcpy(session->currentdir, FTP_SRV_ROOT);  /*  新加入的连接处于文件的根目录下  */
					session->sockfd = com_socket;		/*  保存套接字及客户端地址  */
					session->remote = remote;
				}
			} /*  end else  */
		}  /*  end if(FD_ISSET(sockfd, &tmpfds))  */

		
		struct ftp_session* next;
		session = session_list;
		while (session != NULL) /*  处理每一个控制块的消息  */
		{
			next = session->next;
			if (FD_ISSET(session->sockfd, &tmpfds))	/*  如果要处理的套接字在感兴趣的列表里面  */
			{
				numbytes=recv(session->sockfd, buffer, FTP_BUFFER_SIZE, 0);  /*  阻塞接收该套接字的消息  */
				if(numbytes==0 || numbytes==-1)  /*  接收失败则断开接连,并将该连接的套接字从列表中删除  */
				{
					FD_CLR(session->sockfd, &readfds);
					closesocket(session->sockfd);
					ftp_close_session(session);  /*  释放控制块  */
				}
				else  /*  接收成功后处理消息  */
				{
					buffer[numbytes] = 0;
					if(ftp_process_request(session, buffer)==-1)
					{
						closesocket(session->sockfd);
						ftp_close_session(session);
					}
				}
			} /*  end if (FD_ISSET(session->sockfd, &tmpfds))  */
			session = next;
		}  /*  end while (session != NULL)  */
	}
	
}


/*
*********************************************************************************************************
*                                          
*
* Description:
*             
* Arguments  :
*
* Reutrn     :
*
* Note(s)    : 
*********************************************************************************************************
*/
void lwip_app_ftpserver_Config(void)
{
	OS_ERR err;
	OSTaskCreate( (OS_TCB *)&FTPTestTaskTCB,
							(CPU_CHAR *)"Start Task",
							(OS_TASK_PTR )lwip_app_ftp_Thread,
							(void *)0,
							(OS_PRIO )FTP_TASK_PRIO,
							(CPU_STK	*)&FTP_TASK_STK[0],
							(CPU_STK_SIZE )FTP_TASK_STK_SIZE/10,
							(CPU_STK_SIZE )FTP_TASK_STK_SIZE,
							(OS_MSG_QTY )0,
							(OS_TICK )0,
							(void *)0,
							(OS_OPT )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
							(OS_ERR *)&err);
}

