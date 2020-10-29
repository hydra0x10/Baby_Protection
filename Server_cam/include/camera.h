#ifndef _CAMERA_H_
#define _CAMERA_H_

#define VIDEO_DEV "/dev/video0"
#define IMAGEHEIGHT 360
#define IMAGEWIDTH  480
#define FRAME_NUM   4
#define PICNAME   "/myDir/pic"
struct buffer
{
	void * start;
	unsigned int length;
	long long int timestamp;
}*buffers;


char rgbBuffer[IMAGEHEIGHT * IMAGEWIDTH * 3];
//初始化摄像头设备
int cameraInit(const char * dev);

//v4l2采集图片线程
void *v4l2_frame_process(void * arg);

//YUV转RGB
int yuyv_to_rgb(char * pointer, unsigned char * frame_buffer, int width, int height);

//压缩rgb->jpg
int save_rgb_to_jpg(char *soureceData, int width, int height, char * fileName);
#endif //_CAMERA_H_
