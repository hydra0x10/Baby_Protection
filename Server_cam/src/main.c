#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include "camera.h"

int main(int argc, const char *argv[])
{
	int socketId = socketInit(SIP, PORT);
	if(0 > socketId)
	{
		printf("socketInit error\r\n");
		return -1;
	}
	if(0 > listen(socketId, MAX))
	{
		perror("listen error");
		return -1;
	}
	if(0 > cameraInit(VIDEO_DEV))
	{
		return -1;
	}
	video_flag = 0;
	int newSocketId = 0;
	while(1)
	{

		//newSocketId  = accept(socketId, (struct sockaddr *)&addr, &addrLength);
		doConnect(newSocketId);
	}
	
	return 0;
}
