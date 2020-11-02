#ifndef _SERVER_H_
#define _SERVER_H_

#define SIP "192.168.0.120"
//#define SIP "192.168.0.214"
#define PORT 10011
#define MAX 5
#define SIZE 512

enum COMMAND
{
	VIDEO_ON = 1,
	VIDEO_OFF,
};

enum TYPE
{
	FILE_SIZE = 10101,
	FILE_MSG = 10102,
};

char msg[SIZE];
char * image;

int socketInit(const char * ip, const int port);
/*
 功能:
 	初始化一个套接字
 参数:
 	服务器IP地址,端口号
 返回值:
 	成功返回创建的套接字，失败返回-1
 */

int doConnect(const int newSocketId);
/*
 功能:
 	处理与客户端的连接
 参数:
 	与客户端链接的套接字
 返回值
 	成功返回0,出错返回-1
 */

void *sendImg(void * arg);
/*
 功能:
 	线程处理函数，给客户端发送图片
 参数:
 	已连接的套接字地址
 */
#endif //_SERVER_H_
