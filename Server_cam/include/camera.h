#ifndef _CAMERA_H_
#define _CAMERA_H_

#define VIDEO_DEV "/dev/video0"
#define IMAGEHEIGHT 240
#define IMAGEWIDTH  320
#define REQBUFS_COUNT   15

struct cam_buf
{
	void *start;
	size_t length;
};

//初始化摄像头设备
int camera_init(char *devpath, unsigned int width, unsigned int height);

//开启
int camera_start(int fd);

//出队
int camera_dqbuf(int fd, unsigned int *index);

//入队
int camera_eqbuf(int fd, unsigned int index);

//停止
int camera_stop(int fd);

//退出
int camera_exit(int fd);

int buf_init(int fd);
//YUV转RGB
int yuyv_to_rgb(char * pointer, unsigned char * frame_buffer, int width, int height);


long rgb_to_jpeg(char *rgb, unsigned char ** jpeg, int width, int height);
#endif //_CAMERA_H_
