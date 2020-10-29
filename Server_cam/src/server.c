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

int doConnect(const int newSocketId)
{
	int recvCommand = 0;
	pthread_t thread1;
	int arg = newSocketId;

	pthread_create(&thread1, NULL, sendImg, &arg);
	pthread_detach(thread1);
/*
	if(0 > recv(newSocketId, &recvCommand, sizeof(int), 0))
	{
		perror("recv error");
		return -1;
	}
	switch(recvCommand)
	{
		case VIDEO_ON:
			video_flag = VIDEO_ON;
			break;
		case VIDEO_OFF:
			video_flag = VIDEO_OFF;
			break;
		default:
			video_flag = -1;
	}*/
	video_flag = VIDEO_ON;
	while(1);

}

void *sendImg(void * arg)
{
	int newSocketId = *((int *)arg);
	int fd = 0;
	char filename[20] = {0};
	int i = 0, filesize;
	struct stat buf;
	while(1)
	{
		if(VIDEO_ON == video_flag)
		{
			for(i = 0; i < FRAME_NUM; i++)
			{
				sprintf(filename, "%s%d.jpg", PICNAME, i);
				fd = open(filename, O_RDONLY);
				if(fd < 0)
				{
					perror("open");
					continue;
				}
				fstat(fd, &buf);
				filesize = buf.st_size;
				printf("filesize = %d\n", filesize);
				close(fd);
			}
		}
		else if(VIDEO_OFF == video_flag)
			;
		else
			break;
	}
	
}
