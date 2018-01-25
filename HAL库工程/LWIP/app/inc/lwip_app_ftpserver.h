# ifndef __LWIP_APP_FTP_SERVER_H
# define __LWIP_APP_FTP_SERVER_H

# include "lwip/sockets.h"
# include "lwip/tcp.h"
# include "bsp.h"
# include "ff.h"

enum FTP_CMD
{
	NONE = 0, /*  ָ�����  */
	USER = 1, /*  �ͻ��˷����û���  */
	PASS,     /*  �ͻ��˷��ص��û�����  */
	LIST,			/*  �г�Զ�������ļ�  */
	NLST,			/*    */
	PWD,			/*    */
	TYPE,			/*    */
	PASV,			/*  �÷����������ݶ˿ڼ���,���뱻��ģʽ  */
	RETR,			/*  �����ļ�  */
	STOR,			/*  �ϴ��ļ�  */
	SIZE,			/*  ����Զ�������ļ���С  */
	MDTM,
	SYST,			/*  ��ʾԶ����������ϵͳ����  */
	CWD,			/*  �ı乤��Ŀ¼  */
	CDUP,			/*    */
	PORT,			/*    */
	REST,			/*  �Թ�ָ����������  */
	MKD,			/*    */
	DELE,			/*    */
	RMD,			/*    */
	QUIT,			/*  �رշ�����������  */
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

