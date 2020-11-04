#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <jconfig.h>
#include <jerror.h>
#include <jmorecfg.h>
#include <jpeglib.h>	
#include <linux/videodev2.h>
#include <linux/ioctl.h>
#include "camera.h"

char rgbBuffer[IMAGEHEIGHT * IMAGEWIDTH * 3];
struct v4l2_buffer buf;
struct buffer
{
	void * start;
	unsigned int length;
}*buffers;

int cameraInit(const char * dev)
{
	//打开设备
	int cameraFd = open(dev, O_RDWR | O_NONBLOCK, 0);
	if(0 > cameraFd)
	{
		perror("cameraFd error");
		return -1;
	}

	//获取当前视频设备支持的视频格式 
	int ret = 0;
	struct v4l2_fmtdesc fmtdesc;
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0;  
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	while ((ret = ioctl(cameraFd, VIDIOC_ENUM_FMT, &fmtdesc)) == 0)
	{   
		printf("%s\n", fmtdesc.description);
		struct v4l2_frmsizeenum frmsize;
		frmsize.pixel_format = fmtdesc.pixelformat;
		frmsize.index = 0;
		while (!ioctl(cameraFd, VIDIOC_ENUM_FRAMESIZES, &frmsize))
		{
			if (V4L2_FRMSIZE_TYPE_DISCRETE == frmsize.type)
			{
				printf("  %d x %d\n", frmsize.discrete.width, frmsize.discrete.height);
				frmsize.index++;
			}
			else
			{
				break;
			}
		}
		fmtdesc.index++;
	}

	//查询视频设备的功能 
	struct v4l2_capability cap;
	ret = ioctl(cameraFd, VIDIOC_QUERYCAP, &cap); 
	if(ret < 0)
	{
		printf("get vidieo capability error\r\n");   
		return -1;
	}
	else
	{
		printf("driver:\t\t%s\n",cap.driver);
		printf("card:\t\t%s\n",cap.card);
		printf("bus_info:\t%s\n",cap.bus_info);
		printf("version:\t%d\n",cap.version);
		printf("capabilities:\t%x\n",cap.capabilities);

		if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) 
		{
			printf("Device %s: supports capture.\n", dev);
		}
		if ((cap.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) 
		{
			printf("Device %s: supports streaming.\n", dev);
		}
	}

	//设置视频格式
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(cameraFd, VIDIOC_G_FMT, &fmt) < 0)
	{
		perror("VIDIOC_G_FMT");
		return -1;
	}
	printf("default format: %c%c%c%c %dx%d\n", 
			fmt.fmt.pix.pixelformat       & 0xff,
			fmt.fmt.pix.pixelformat >> 8  & 0xff,
			fmt.fmt.pix.pixelformat >> 16 & 0xff,
			fmt.fmt.pix.pixelformat >> 24 & 0xff,
			fmt.fmt.pix.width, 
			fmt.fmt.pix.height);

	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;//yuv格式
	fmt.fmt.pix.height = IMAGEHEIGHT;
	fmt.fmt.pix.width = IMAGEWIDTH;
	fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

	ret = ioctl(cameraFd, VIDIOC_S_FMT, &fmt);
	if(0 > ret)
	{
		printf("set fmt error\r\n");
		return -1;
	}

	//申请帧缓冲
	struct v4l2_requestbuffers  req;
	unsigned int n_buffers;

	memset(&req, 0, sizeof(struct v4l2_requestbuffers));
	req.count = FRAME_NUM;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(cameraFd, VIDIOC_REQBUFS, &req);
	if(0 > ret)
	{
		printf("request buffers error\r\n");
		return -1;
	}
	buffers = malloc(req.count * sizeof(*buffers));
	memset(&buf, 0, sizeof(buf));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	for(n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++) 
	{	
		buf.index = n_buffers;
		if(ioctl(cameraFd, VIDIOC_QUERYBUF, &buf) == -1)
		{
			printf("query buffer error\n");
			return -1;
		}
		buffers[n_buffers].start = (char *)mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, cameraFd, buf.m.offset);
		if(buffers[n_buffers].start == MAP_FAILED)
		{
			perror("map error");
			printf("buffer map error\n");
			return -1;
		}
	}
	//缓冲区入队
	for(n_buffers = 0; n_buffers < FRAME_NUM; n_buffers++)
	{
		buf.index = n_buffers;
		ioctl(cameraFd, VIDIOC_QBUF, &buf);
	}
	
	enum v4l2_buf_type v4l2type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(cameraFd, VIDIOC_STREAMON, &v4l2type); 
	
	return cameraFd;
}


//入队
int qbuf(int cameraFd, int index)
{
	buf.index = index;
	ioctl(cameraFd, VIDIOC_QBUF, &buf);
}

//出队
int dqbuf(int cameraFd, int index)
{
	buf.index = index;
	ioctl(cameraFd, VIDIOC_DQBUF, &buf);
}


int save_rgb_to_jpg(char *soureceData, int width, int height, char * fileName)
{
	struct jpeg_compress_struct cinfo ;
	struct jpeg_error_mgr jerr ;
	JSAMPROW  row_pointer[1] ;
	int row_stride ;
	char *buf=NULL ;
	int x ;
	FILE *fptr_jpg = fopen (fileName,"wb");
	if(fptr_jpg==NULL)
	{
		printf("open file failed!/n") ;
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fptr_jpg);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 80,TRUE);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = width * 3;
	buf=malloc(row_stride) ;
	row_pointer[0] = buf;
	while (cinfo.next_scanline < height)
	{
		for (x = 0; x < row_stride; x+=3)
		{
			buf[x]   = soureceData[x];
			buf[x+1] = soureceData[x+1];
			buf[x+2] = soureceData[x+2];
		}
		jpeg_write_scanlines (&cinfo, row_pointer, 1);//critical
		soureceData += row_stride;
	}
	jpeg_finish_compress(&cinfo);
	fclose(fptr_jpg);
	jpeg_destroy_compress(&cinfo);
	free(buf);
	return 0;
}  

int yuyv_to_rgb(char * pointer, unsigned char * frame_buffer, int width, int height)
{
	int i,j;
	unsigned char y1,y2,u,v;
	int r1,g1,b1,r2,g2,b2;
	for(i=0;i<height;i++)
	{
		for(j=0;j<width/2;j++)//每次取4个字节，也就是两个像素点，转换rgb，6个字节，还是两个像素点
		{
			y1 = *( pointer + (i*width/2+j)*4);     
			u  = *( pointer + (i*width/2+j)*4 + 1);
			y2 = *( pointer + (i*width/2+j)*4 + 2);
			v  = *( pointer + (i*width/2+j)*4 + 3);

			r1 = y1 + 1.042*(v-128);
			g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);
			b1 = y1 + 1.772*(u-128);

			r2 = y2 + 1.042*(v-128);
			g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);
			b2 = y2 + 1.772*(u-128);

			if(r1>255)
				r1 = 255;
			else if(r1<0)
				r1 = 0;

			if(b1>255)
				b1 = 255;
			else if(b1<0)
				b1 = 0;    

			if(g1>255)
				g1 = 255;
			else if(g1<0)
				g1 = 0;    

			if(r2>255)
				r2 = 255;
			else if(r2<0)
				r2 = 0;

			if(b2>255)
				b2 = 255;
			else if(b2<0)
				b2 = 0;    

			if(g2>255)
				g2 = 255;
			else if(g2<0)
				g2 = 0;        

			*(frame_buffer + (i*width/2+j)*6    ) = (unsigned char)b1;
			*(frame_buffer + (i*width/2+j)*6 + 1) = (unsigned char)g1;
			*(frame_buffer + (i*width/2+j)*6 + 2) = (unsigned char)r1;
			*(frame_buffer + (i*width/2+j)*6 + 3) = (unsigned char)b2;
			*(frame_buffer + (i*width/2+j)*6 + 4) = (unsigned char)g2;
			*(frame_buffer + (i*width/2+j)*6 + 5) = (unsigned char)r2;
		}
	}
}
