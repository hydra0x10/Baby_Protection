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

volatile int fdset[FRAME_NUM] = {0};
volatile char video_flag = 0;

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
	char recvCommand[10] = {0};
	int command = 0;
	pthread_t thread1;
	int arg = newSocketId;

	pthread_create(&thread1, NULL, sendImg, &arg);
	pthread_detach(thread1);

	while(1)
	{
		if(0 >= recv(newSocketId, &recvCommand, sizeof(recvCommand), 0))
		{
			perror("recv error");
			video_flag = -1;
			close(newSocketId);
			return -1;
		}
		command = atoi(recvCommand);
		switch(command)
		{
			case VIDEO_ON:
				video_flag = VIDEO_ON;
				break;
			case VIDEO_OFF:
				video_flag = VIDEO_OFF;
				break;
		}
	}

}

void *sendImg(void * arg)
{
	int newSocketId = *((int *)arg);
	int fd = 0;
	video_flag = VIDEO_OFF;
	char filename[20] = {0};
	int i = 0, filesize = 0, len = 0;
	struct stat buf;

	while(1)
	{
		for(i = 0; i < FRAME_NUM; i++)
		{
			switch(video_flag)
			{
				case VIDEO_ON:
				{
					sprintf(filename, "%s%d.jpg", PICNAME, i);
					if(fdset[i] != 1)
					{
						printf("-------\r\n");
						break;
					}
					fdset[i] = 2;
					fd = open(filename, O_RDONLY);
					if(fd < 0)
					{
						perror("open");
						fdset[i] = 0;
						break;
					}

					fstat(fd, &buf);
					filesize = buf.st_size;
					printf("%d\r\n", filesize);

					memset(msg, 0, SIZE);
					
					sprintf(msg, "%d%d", FILE_SIZE, filesize);
					int ret = send(newSocketId, msg, SIZE, 0);
					if(0 > ret)
					{
						fdset[i] = 0;
						return;
					}
					
					while(filesize > 0)
					{
						memset(msg, 0, SIZE);
						sprintf(msg, "%d", FILE_MSG);
						len = read(fd, msg+5, SIZE-5);
						if(0 > send(newSocketId, msg, SIZE, 0))
						{
							fdset[i] = 0;
							return;
						}
						filesize -= len;
					}
					fdset[i] = 0;
					close(fd);
					break;
				}
				case VIDEO_OFF:	
					break;
				default:
				{
					printf("exit\r\n");
					return;
				}
			}
		}
	}
}
	
