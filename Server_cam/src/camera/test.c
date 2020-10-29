#include <stdio.h>
#include "../../include/camera.h"

int main(int argc, const char *argv[])
{
	int fd = cameraInit(VIDEO_DEV);
	if(fd < 0)
		return;
	v4l2_frame_process(fd);
	return 0;
}
