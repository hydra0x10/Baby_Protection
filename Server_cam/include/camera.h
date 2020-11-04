#ifndef _CAMERA_H_
#define _CAMERA_H_

#define VIDEO_DEV "/dev/video0"
#define IMAGEHEIGHT 288
#define IMAGEWIDTH  352
#define FRAME_NUM   30
#define PICNAME   "/myDir/pic"


//初始化摄像头设备
int cameraInit(const char * dev);

//入队
int qbuf(int cameraFd, int index);

//出队
int dqbuf(int cameraFd, int index);

//YUV转RGB
int yuyv_to_rgb(char * pointer, unsigned char * frame_buffer, int width, int height);

//压缩rgb->jpg
int save_rgb_to_jpg(char *soureceData, int width, int height, char * fileName);
#endif //_CAMERA_H_
