#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>


#define FSPWM_MAGIC	'f'
#define FSPWM_START	_IO(FSPWM_MAGIC, 0)
#define FSPWM_STOP	_IO(FSPWM_MAGIC, 1)
#define FSPWM_SET_FREQ	_IOW(FSPWM_MAGIC, 2, unsigned int)

int pwm_init()
{
    int i;
	int fd;
	int ret;

    fd = open("/dev/pwm", O_RDWR);

    if (fd == -1)
		goto fail;

	ret = ioctl(fd, FSPWM_START);
	if (ret == -1)
		goto fail;

    ret = ioctl(fd, FSPWM_STOP);
	if (ret == -1)
		goto fail;

	exit(EXIT_SUCCESS);
fail:
	perror("pwm test");
	exit(EXIT_FAILURE);
}