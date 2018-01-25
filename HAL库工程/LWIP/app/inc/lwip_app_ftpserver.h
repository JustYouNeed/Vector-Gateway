# ifndef __LWIP_APP_FTP_SERVER_H
# define __LWIP_APP_FTP_SERVER_H

# include "lwip/sockets.h"
# include "lwip/tcp.h"
# include "bsp.h"
# include "ff.h"

enum FTP_CMD
{
	NONE = 0, /*  指令不存在  */
	USER = 1, /*  客户端返回用户名  */
	PASS,     /*  客户端返回的用户密码  */
	LIST,			/*  列出远程主机文件  */
	NLST,			/*    */
	PWD,			/*    */
	TYPE,			/*    */
	PASV,			/*  让服务器在数据端口监听,进入被动模式  */
	RETR,			/*  下载文件  */
	STOR,			/*  上传文件  */
	SIZE,			/*  回显远程主机文件大小  */
	MDTM,
	SYST,			/*  显示远程主机操作系统类型  */
	CWD,			/*  改变工作目录  */
	CDUP,			/*    */
	PORT,			/*    */
	REST,			/*  略过指定点后的数据  */
	MKD,			/*    */
	DELE,			/*    */
	RMD,			/*    */
	QUIT,			/*  关闭服务器的连接  */
	UTIME, 
	SITE,
};

struct ftp_session
{
	int8_t is_anonymous;

	int sockfd;
	struct sockaddr_in remote;

	/* pasv data */
	char pasv_active;
	int  pasv_sockfd;

	unsigned short pasv_port;
	size_t offset;

	/* current directory */
	char currentdir[256];

	struct ftp_session* next;
};

void lwip_app_ftpserver_Config(void);

int ftp_CmdCompare(char *src, char *cmd);
uint8_t ftp_GetCmd(char *buff);
char *ftp_GetParameter(char *buff);
uint8_t ftp_UserVerify(struct ftp_session* session, char *para);
uint8_t ftp_PasswordVerify(struct ftp_session* session, char *para);
uint8_t ftp_EnterPassiveMode(struct ftp_session* session, char *para);
uint8_t ftp_EnterPositiveMode(struct ftp_session* session, char *para);
uint8_t ftp_ListFile(struct ftp_session* session, char *para);
uint8_t ftp_MakeDir(struct ftp_session* session, char *para);
uint8_t ftp_ChangeWorkDir(struct ftp_session* session, char *para);
uint8_t ftp_GetWorkDir(struct ftp_session* session, char *para);
uint8_t ftp_SetType(struct ftp_session* session, char *para);
uint8_t ftp_GetSystemInfo(struct ftp_session* session, char *para);
uint8_t ftp_Disconnect(struct ftp_session* session, char *para);
uint8_t ftp_SetFileOffset(struct ftp_session* session, char *para);
uint8_t ftp_GetFileSize(struct ftp_session* session, char *para);
uint8_t ftp_GetFileLastModTime(struct ftp_session* session, char *para);
uint8_t ftp_DelFile(struct ftp_session* session, char *para);
uint8_t ftp_UploadFile(struct ftp_session* session, char *para);
uint8_t ftp_MoveFile(struct ftp_session* session, char *para);
uint8_t ftp_DownloadFile(struct ftp_session* session, char *para);
uint8_t ftp_UpdataFileTime(struct ftp_session* session, char *para);




# endif

