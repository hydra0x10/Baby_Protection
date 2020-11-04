#include <stdio.h>
#include "server.h"
#include "camera.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

volatile int fdset[FRAME_NUM] = {2};
volatile char video_flag = 0;
volatile char video_recv;
extern struct buffer
{
	void * start;
	unsigned int length;
}*buffers;
extern char rgbBuffer[IMAGEHEIGHT * IMAGEWIDTH * 3];
int cameraFd;

int socketInit(const char * ip, const int port)
{
	if(NULL == ip)
	{
		printf("arg error!\r\n");
		return -1;
	}
	int socketId = 0;
	socketId = socket(PF_INET, SOCK_STREAM, 0);
	if(0 > socketId)
	{
		perror("socket error");
		return -1;
	}

	int on = 1;
	setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	struct sockaddr_in addr = {
		.sin_family = PF_INET,
		.sin_port = htons(PORT),
		.sin_addr = {
			.s_addr = inet_addr(ip),
		},
	};

	int addrLength = sizeof(addr);
	
	if(0 > bind(socketId, (struct sockaddr *)&addr, addrLength))
	{
		perror("bind error");
		return -1;
	}
	return socketId;
}

int doConnect(const int newSocketId, int fd)
{
	
	char recvCommand;
	pthread_t thread1;
	int arg = newSocketId;
	int command = 0;
	pthread_create(&thread1, NULL, sendImg, &arg);
	pthread_detach(thread1);
	int nSendBuf= 32 * 1024;
	setsockopt(newSocketId,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
	char filename[20] = {0};
	int j = 0;
	cameraFd = fd;
	while(1)
	{
		if(0 >= recv(newSocketId, &recvCommand, sizeof(recvCommand), 0))
		{
			perror("recv error");
			video_flag = -1;
			video_recv = -1;
			close(newSocketId);
			return -1;
		}
		printf("recvCommand = %c\r\n", recvCommand);
		command = recvCommand - '0';
		switch(command)
		{
			case VIDEO_ON:
				video_flag = VIDEO_ON;
				video_recv = VIDEO_RECV;
				fdset[0] = 0;
				break;
			case VIDEO_OFF:
				video_flag = VIDEO_OFF;
				break;
			case VIDEO_RECV:
				video_recv = VIDEO_RECV;
				break;
		}
	}

}

void *sendImg(void * arg)
{
	int newSocketId = *((int *)arg);
	char *msg = (char *)malloc(SIZE);
	int fd = 0;
	video_flag = VIDEO_OFF;
	char fileName[20] = {0};
	int i = 0, filesize = 0, len = 0;
	struct stat buf;
	
	while(1)
	{
		dqbuf(cameraFd, i);
		memset(fileName, 0, 20);
		sprintf(fileName, "%s%d", PICNAME, i);
		
		//客户端断开连接
		if(video_flag == -1)
		{
			break;
		}
		
		if(video_flag == VIDEO_ON && video_recv == VIDEO_RECV)
		{
			memset(rgbBuffer, 0, (3 * IMAGEHEIGHT * IMAGEWIDTH));
			yuyv_to_rgb(buffers[i].start, rgbBuffer, IMAGEWIDTH, IMAGEHEIGHT);
			save_rgb_to_jpg(rgbBuffer, IMAGEWIDTH, IMAGEHEIGHT, fileName);
			fd = open(fileName, O_RDONLY);
			if(fd < 0)
			{
				perror("open");

				break;
			}
			fstat(fd, &buf);
			filesize = buf.st_size;
			memset(msg, 0, SIZE);
			sprintf(msg, "%d%d#", _FILE, filesize);
			read(fd, msg+strlen(msg), SIZE-strlen(msg));
			if(0 > send(newSocketId, msg, SIZE, 0))
			{
				perror("send");
				break;
			}
			video_recv = -1;
			close(fd);
		}
		qbuf(cameraFd, i);
		i++;
		if(i == 30) i = 0;
	}
	free(msg);
}
