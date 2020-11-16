#ifndef _SERVER_H_
#define _SERVER_H_

#define PORT 10011
#define MAX 5
#define SIZE (32*1024)

enum COMMAND
{
	VIDEO_ON = 1,
	VIDEO_OFF,
	LED_ON,
	LED_OFF
};

enum TYPE
{
	_FILE = 10101,
	_HUMI_TEMP,
};


int socketInit(const int port);
/*
 功能:
 	初始化一个套接字
 参数:
 	服务器IP地址,端口号
 返回值:
 	成功返回创建的套接字，失败返回-1
 */

int doConnect(const int newSocketId, int fd);
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

void *send_Humi_Temp(void * arg);


void *pwm_buzz(void *arg);

#endif //_SERVER_H_
