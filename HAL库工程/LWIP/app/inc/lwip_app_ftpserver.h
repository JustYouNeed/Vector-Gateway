# ifndef __LWIP_APP_FTP_SERVER_H
# define __LWIP_APP_FTP_SERVER_H

# include "lwip/sockets.h"
# include "lwip/tcp.h"
# include "bsp.h"
# include "ff.h"

/*
******************************************************************************************
FTP应答码与响应码

110: 重新启动标记应答
120: 在n分钟内准备好
125: 连接打开准备传送
150: 打开数据连接
200: 命令成功
202: 命令失败
211: 系统状态
212: 目录状态
213: 文件状态
214: 帮助信息
215: 名字系统类型
220: 新用户服务准备好了
221: 服务关闭控制连接,可以退出登录
225: 数据连接打开,无传输正在进行
226: 关闭数据连接,请求的文件操作成功
227: 进入被动模式
230: 用户登录
250: 请求的文件操作完成
257: 创建"PATHNAME"
331: 用户名正确,需要口令
332: 登录时需要账户信息
350: 下一步命令
421: 不能提供服务,关闭控制连接
425: 不能打开数据连接
426: 关闭连接,中止传输
450: 请求的文件操作示执行
451: 中止请求的操作,有本地错误
452: 未执行请求的操作,系统存储空间不足
500: 格式错误,命令不可识别
501: 参数语法错误
502: 命令未实现
503: 命令顺序错误
504: 此参数下的命令功能未实现
530: 未登录
532: 存储文件需要账户信息
550: 未执行的请求操作
551: 请求操作中止,页类型未知
552: 请求的文件操作中止,存储分配溢出
553: 未执行的请求操作,文件名不合法

*******************************************************************************************
*/


enum FTP_CMD
{
	NONE = 0, /*  指令不存在  */
	ABOR,     /*  中断数据连接程序, 无参数  */
	ACCT,			/*  系统特权账号, ACCT<acount>  */
	ALLO,			/*  为服务器上的文件存储器分配字节, ALLO<bytes>  */
	APPE,			/*  添加文件到服务器同名文件, APPE<filename>  */
	CDUP,			/*  改变服务器上的父目录, CDUP<dir path>  */
	CWD,			/*  改变工作目录, CWD<dir path>  */
	DELE,			/*  删除服务器上的指定文件, DELE<filename>  */
	HELP,			/*  返回指定命令信息, HELP<command>  */
	LIST,			/*  列出远程主机目录、文件信息, LIST<name>  */
	MODE,			/*  传输模式,S:流模式, B:块模式, C:压缩模式, MODE<mode>  */
	MKD,			/*  在服务器上建立指定目录, MKD<directory>  */
	MDTM,			/*    */
	NLST,			/*  列出服务器目录名字, 无参数  */
	NOOP,     /*  无动作,除了不算服务器上的承认  */
	PASS,     /*  系统登录密码, PASS<password>  */
	PASV,			/*  请求服务器等待数据连接,无参数  */
	PORT,			/*  IP地址和两字节的端口ID, PORT<address>  */
	PWD,			/*  显示当前工作目录,无参数  */
	QUIT,			/*  从FTP服务器中退出登录,无参数  */
	REIN,			/*  重新初始化登录状态连接  */
	REST,			/*  由特定的偏移量重启文件传递, REST<offset>  */
	RETR,			/*  从服务器上找回(复制)文件, RETR<filename>  */
	RMD,			/*  在服务器上删除指定目录, RMD<directory>  */
	RNFR,     /*  对旧路径重命名, RNFR<old path>  */
	RNTO,     /*  对新路径重命名, RNTO<new path>  */
	SMNT,     /*  挂载指定文件结构, SMNT<pathname>  */
	STAT, 		/*  在当前程序或目录返回信息, STAT<directory>  */
	STOR,			/*  上传文件  */
	SIZE,			/*  回显远程主机文件大小  */
	SITE,			/*  由服务器提供的站点特殊参数, SITE<params>  */
	STOU,     /*  储存文件到服务器名称上, STOU<filename>  */
	STRU,     /*  数据结构,F:文件, R:记录, P:页面, STRU<type>  */
	SYST,			/*  返回服务器使用的操作系统, 无参数  */
	TYPE,			/*   数据类型,A:ASCII, E:EBCDIC, I:binary, TYPE<data type> */
	USER,     /*  系统登录的用户名, USER<username>  */
	UTIME, 		/*  站点提供的特殊参数,更新文件更改时间  */	
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

