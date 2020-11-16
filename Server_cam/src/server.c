#include <stdio.h>
#include "server.h"
#include "camera.h"
#include "fspwm.h"
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
#include <sys/ioctl.h>

volatile int video_flag = 0;
int buzz_flag = 1;

extern struct cam_buf bufs[REQBUFS_COUNT];
extern unsigned char rgbBuffer[IMAGEHEIGHT * IMAGEWIDTH * 3];
char msg[SIZE] = {0};
int uartFd;

extern int uart_Init();
int cameraFd;

int socketInit(const int port)
{
	int socketId = 0;
	socketId = socket(PF_INET, SOCK_STREAM, 0); //创建套接字，流式套接字， TCP
	if(0 > socketId)
	{
		perror("socket error");
		return -1;
	}

	int on = 1;
	setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); //设置地址复用

	struct sockaddr_in addr = {
		.sin_family = PF_INET,
		.sin_port = htons(PORT),
		.sin_addr = {
			.s_addr = INADDR_ANY,
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
	pthread_t thread2, thread3;
	int arg = newSocketId;
	
	int command = 0;
	pthread_create(&thread1, NULL, sendImg, &arg);
	pthread_detach(thread1);
	pthread_create(&thread2, NULL, send_Humi_Temp, &arg);
	pthread_detach(thread2);
	pthread_create(&thread3, NULL, pwm_buzz, NULL);
	pthread_detach(thread3);

	video_flag = VIDEO_OFF;
	cameraFd = fd;
	
	while(1)
	{
		if(0 >= recv(newSocketId, &recvCommand, sizeof(recvCommand), 0))
		{
			video_flag = -1;
			return -1;
		}
		printf("recvCommand = %c\r\n", recvCommand);
		command = recvCommand - '0';
		switch(command)
		{
			case VIDEO_ON:
				video_flag = VIDEO_ON;
				break;
			case VIDEO_OFF:
				video_flag = VIDEO_OFF;
				break;
			case LED_ON:
				if(uartFd > 0)
					write(uartFd, "on", 2);
				break;
			case LED_OFF:
				if(uartFd > 0)
					write(uartFd, "off", 3);
				break;
		}
	}

}

void *sendImg(void * arg)
{
	int newSocketId = *((int *)arg);
	int  filesize = 0;
	int index = 0;
	int len = 0, sendSize;
	
	buf_init(cameraFd);
	//camera_start(cameraFd);
	printf("cameraFd ok\r\n");
	
	while(1)
	{
		if(video_flag == -1)
		{
			break;
		}
		
		int ret = camera_dqbuf(cameraFd, &index);
		if(ret == -1)
		{
			continue;
		}
		
		
		if(video_flag == VIDEO_ON)
		{
			memset(rgbBuffer, 0, (3 * IMAGEHEIGHT * IMAGEWIDTH));
			
			//yuyv_to_rgb(bufs[index].start, rgbBuffer, IMAGEWIDTH, IMAGEHEIGHT);
			yuv_to_rgb(bufs[index].start, rgbBuffer, IMAGEWIDTH, IMAGEHEIGHT);

			char * jpegBuffer = NULL;
			filesize = rgb_to_jpeg(rgbBuffer, (unsigned char **)&jpegBuffer, IMAGEWIDTH, IMAGEHEIGHT);
			memset(msg, '0', SIZE);
			sprintf(msg, "%d%d#", _FILE, filesize); //命令字+文件大小+‘#’+文件内容
			len  = strlen(msg);
			memcpy(msg+len, jpegBuffer, filesize);		
			//printf("%d\r\n", *(msg+len+filesize-1));
			
			sendSize = send(newSocketId, msg, SIZE, 0);
			if(0 >= sendSize)
			{
				camera_eqbuf(cameraFd,index);
				free(jpegBuffer);
				break;	
			}
			//printf("filesize = %d, sendSize = %d\r\n", filesize, sendSize);
			free(jpegBuffer);
		}
		camera_eqbuf(cameraFd,index);
	}
	//camera_stop(cameraFd);
	printf("sendImg exit\r\n");
}

void * send_Humi_Temp(void * arg)
{
	int newSocketId = *((int *)arg);
	uartFd = uart_Init();
	if(uartFd < 0)
		return;
	char buf[20];
	int len;
	int i = 0;
	int j = 0;
	while(1)
	{
		if(video_flag == -1)
			break;
		memset(buf, 0, 20);
		memset(msg, 0, SIZE);

		read(uartFd, buf, 20);  //湿度#温度#a 湿度#温度#b
		len = strlen(buf);
		if(buf[len - 1] == 'a')
		{
			buzz_flag = 0;
		}
		else
			buzz_flag = 1;
		sprintf(msg, "%d%s", _HUMI_TEMP,  buf); //命令字+湿度#温度#a
		
		if(0 >= send(newSocketId, msg, SIZE, 0))
		{
			break;
		}
		sleep(2);
	}
	printf("send_Humi_Temp exit\r\n");
	close(newSocketId);
	close(uartFd);
}

void * pwm_buzz(void *arg)
{
	int buzz_on = 0;
	buzz_flag = 1;
	int buzzFd = open(PWM_DEV, O_RDWR);
	if(buzzFd < 0)
	{
		perror("open buzz");
		pthread_exit(0);
	}
	ioctl(buzzFd, FSPWM_SET_FREQ, 900);
	while(1)
	{
		if(video_flag == -1)
		{
			ioctl(buzzFd, FSPWM_STOP);
			close(buzzFd);
			printf("buzz eixt\r\n");
			pthread_exit(0);
		}
		if(buzz_flag == 0 && buzz_on == 0)
		{
			ioctl(buzzFd, FSPWM_START);
			buzz_on = -1;
		}
		if(buzz_flag == 1 && buzz_on == -1)
		{
			ioctl(buzzFd, FSPWM_STOP);
			buzz_on = 0;
		}
	}
}