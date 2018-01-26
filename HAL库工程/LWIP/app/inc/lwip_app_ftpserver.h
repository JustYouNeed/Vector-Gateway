# ifndef __LWIP_APP_FTP_SERVER_H
# define __LWIP_APP_FTP_SERVER_H

# include "lwip/sockets.h"
# include "lwip/tcp.h"
# include "bsp.h"
# include "ff.h"

/*
******************************************************************************************
FTPӦ��������Ӧ��

110: �����������Ӧ��
120: ��n������׼����
125: ���Ӵ�׼������
150: ����������
200: ����ɹ�
202: ����ʧ��
211: ϵͳ״̬
212: Ŀ¼״̬
213: �ļ�״̬
214: ������Ϣ
215: ����ϵͳ����
220: ���û�����׼������
221: ����رտ�������,�����˳���¼
225: �������Ӵ�,�޴������ڽ���
226: �ر���������,������ļ������ɹ�
227: ���뱻��ģʽ
230: �û���¼
250: ������ļ��������
257: ����"PATHNAME"
331: �û�����ȷ,��Ҫ����
332: ��¼ʱ��Ҫ�˻���Ϣ
350: ��һ������
421: �����ṩ����,�رտ�������
425: ���ܴ���������
426: �ر�����,��ֹ����
450: ������ļ�����ʾִ��
451: ��ֹ����Ĳ���,�б��ش���
452: δִ������Ĳ���,ϵͳ�洢�ռ䲻��
500: ��ʽ����,�����ʶ��
501: �����﷨����
502: ����δʵ��
503: ����˳�����
504: �˲����µ������δʵ��
530: δ��¼
532: �洢�ļ���Ҫ�˻���Ϣ
550: δִ�е��������
551: ���������ֹ,ҳ����δ֪
552: ������ļ�������ֹ,�洢�������
553: δִ�е��������,�ļ������Ϸ�

*******************************************************************************************
*/


enum FTP_CMD
{
	NONE = 0, /*  ָ�����  */
	ABOR,     /*  �ж��������ӳ���, �޲���  */
	ACCT,			/*  ϵͳ��Ȩ�˺�, ACCT<acount>  */
	ALLO,			/*  Ϊ�������ϵ��ļ��洢�������ֽ�, ALLO<bytes>  */
	APPE,			/*  ����ļ���������ͬ���ļ�, APPE<filename>  */
	CDUP,			/*  �ı�������ϵĸ�Ŀ¼, CDUP<dir path>  */
	CWD,			/*  �ı乤��Ŀ¼, CWD<dir path>  */
	DELE,			/*  ɾ���������ϵ�ָ���ļ�, DELE<filename>  */
	HELP,			/*  ����ָ��������Ϣ, HELP<command>  */
	LIST,			/*  �г�Զ������Ŀ¼���ļ���Ϣ, LIST<name>  */
	MODE,			/*  ����ģʽ,S:��ģʽ, B:��ģʽ, C:ѹ��ģʽ, MODE<mode>  */
	MKD,			/*  �ڷ������Ͻ���ָ��Ŀ¼, MKD<directory>  */
	MDTM,			/*    */
	NLST,			/*  �г�������Ŀ¼����, �޲���  */
	NOOP,     /*  �޶���,���˲���������ϵĳ���  */
	PASS,     /*  ϵͳ��¼����, PASS<password>  */
	PASV,			/*  ����������ȴ���������,�޲���  */
	PORT,			/*  IP��ַ�����ֽڵĶ˿�ID, PORT<address>  */
	PWD,			/*  ��ʾ��ǰ����Ŀ¼,�޲���  */
	QUIT,			/*  ��FTP���������˳���¼,�޲���  */
	REIN,			/*  ���³�ʼ����¼״̬����  */
	REST,			/*  ���ض���ƫ���������ļ�����, REST<offset>  */
	RETR,			/*  �ӷ��������һ�(����)�ļ�, RETR<filename>  */
	RMD,			/*  �ڷ�������ɾ��ָ��Ŀ¼, RMD<directory>  */
	RNFR,     /*  �Ծ�·��������, RNFR<old path>  */
	RNTO,     /*  ����·��������, RNTO<new path>  */
	SMNT,     /*  ����ָ���ļ��ṹ, SMNT<pathname>  */
	STAT, 		/*  �ڵ�ǰ�����Ŀ¼������Ϣ, STAT<directory>  */
	STOR,			/*  �ϴ��ļ�  */
	SIZE,			/*  ����Զ�������ļ���С  */
	SITE,			/*  �ɷ������ṩ��վ���������, SITE<params>  */
	STOU,     /*  �����ļ���������������, STOU<filename>  */
	STRU,     /*  ���ݽṹ,F:�ļ�, R:��¼, P:ҳ��, STRU<type>  */
	SYST,			/*  ���ط�����ʹ�õĲ���ϵͳ, �޲���  */
	TYPE,			/*   ��������,A:ASCII, E:EBCDIC, I:binary, TYPE<data type> */
	USER,     /*  ϵͳ��¼���û���, USER<username>  */
	UTIME, 		/*  վ���ṩ���������,�����ļ�����ʱ��  */	
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

uint8_t ftp_Handle_ABOR(struct ftp_session* session);
uint8_t ftp_Handle_ACCT(struct ftp_session* session, char *para);
uint8_t ftp_Handle_ALLO(struct ftp_session* session, char *para);
uint8_t ftp_Handle_APPE(struct ftp_session* session, char *para);
uint8_t ftp_Handle_CDUP(struct ftp_session* session, char *para);
uint8_t ftp_Handle_CWD(struct ftp_session* session, char *para);
uint8_t ftp_Handle_DELE(struct ftp_session* session, char *para);
uint8_t ftp_Handle_HELP(struct ftp_session* session, char *para);
uint8_t ftp_Handle_LIST(struct ftp_session* session, char *para);
uint8_t ftp_Handle_MODE(struct ftp_session* session, char *para);
uint8_t ftp_Handle_MKD(struct ftp_session* session, char *para);
uint8_t ftp_Handle_NLST(struct ftp_session* session, char *para);
uint8_t ftp_Handle_NOOP(struct ftp_session* session);
uint8_t ftp_Handle_PASS(struct ftp_session* session, char *para);
uint8_t ftp_Handle_PORT(struct ftp_session* session, char *para);
uint8_t ftp_Handle_PASV(struct ftp_session* session);
uint8_t ftp_Handle_PWD(struct ftp_session* session);
uint8_t ftp_Handle_QUIT(struct ftp_session* session);
uint8_t ftp_Handle_REIN(struct ftp_session* session);
uint8_t ftp_Handle_REST(struct ftp_session* session, char *para);
uint8_t ftp_Handle_RETR(struct ftp_session* session, char *para);
uint8_t ftp_Handle_RMD(struct ftp_session* session, char *para);
uint8_t ftp_Handle_RNTO(struct ftp_session* session, char *para);
uint8_t ftp_Handle_RNFR(struct ftp_session* session, char *para);
uint8_t ftp_Handle_SITE(struct ftp_session* session, char *para);
uint8_t ftp_Handle_SMNT(struct ftp_session* session, char *para);
uint8_t ftp_Handle_STAT(struct ftp_session* session, char *para);
uint8_t ftp_Handle_STOR(struct ftp_session* session, char *para);
uint8_t ftp_Handle_STOU(struct ftp_session* session, char *para);
uint8_t ftp_Handle_STRU(struct ftp_session* session, char *para);
uint8_t ftp_Handle_SYST(struct ftp_session* session);
uint8_t ftp_Handle_TYPE(struct ftp_session* session, char *para);
uint8_t ftp_Handle_USER(struct ftp_session* session, char *para);
uint8_t ftp_Handle_SIZE(struct ftp_session* session, char *para);
uint8_t ftp_Handle_MDTM(struct ftp_session* session, char *para);
uint8_t ftp_Handle_UTIME(struct ftp_session* session, char *para);


# endif

