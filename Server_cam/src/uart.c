#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

int uart_Init()
{
	int fd = open("/dev/ttyUSB0", O_RDWR|O_NOCTTY);
	if(fd == -1)
	{
		perror("open serial:");
		return -1;
	}
	struct termios opt;
	tcgetattr(fd, &opt);
	cfsetispeed(&opt, B9600);
	cfsetospeed(&opt, B9600);
	if(tcsetattr(fd, TCSANOW, &opt) != 0)
	{
		perror("tcsetattr error:");
		close(fd);
		return -1;
	}
	opt.c_cflag &= ~CSIZE;
	opt.c_cflag |= CS8;
	opt.c_cflag &= ~CSTOPB;
	opt.c_cflag &= ~PARENB;
	opt.c_cflag &= ~INPCK;
	opt.c_cflag |= (CLOCAL | CREAD);
	opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	opt.c_oflag &= ~OPOST;
	opt.c_oflag &= ~(ONLCR | OCRNL);
	opt.c_iflag &= ~(ICRNL | INLCR);
	opt.c_iflag &= ~(IXON | IXOFF | IXANY);
	opt.c_cc[VTIME] = 0;
	opt.c_cc[VMIN] = 0;
	tcflush(fd, TCIOFLUSH);
	printf("configure complete\n");
	if(tcsetattr(fd, TCSANOW, &opt) != 0)
	{
		perror("serial error:");
		return -1;
	}
	return fd;
}

