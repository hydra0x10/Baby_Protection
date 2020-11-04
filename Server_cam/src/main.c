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
	int cameraFd = cameraInit(VIDEO_DEV);
	if(0 > cameraFd)
	{
		return -1;
	}
	int newSocketId = 0;
	struct sockaddr_in addr;
	int addrLength = sizeof(addr);
	while(1)
	{

		newSocketId  = accept(socketId, (struct sockaddr *)&addr, &addrLength);
		doConnect(newSocketId, cameraFd);
	}
	
	return 0;
}
