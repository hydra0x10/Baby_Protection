#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "camera.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/time.h>
#include <jconfig.h>
#include <jerror.h>
#include <jmorecfg.h>
#include <jpeglib.h>
#include <errno.h>

struct v4l2_requestbuffers reqbufs;
struct cam_buf bufs[REQBUFS_COUNT];
char rgbBuffer[IMAGEHEIGHT * IMAGEWIDTH * 3];

int camera_init(char *devpath, unsigned int width, unsigned int height)
{
	int i;
	int fd = -1;
	int ret;
	struct v4l2_buffer vbuf;
	struct v4l2_format format;
	struct v4l2_capability capability;

	/*open device of video*/
	if((fd = open(devpath, O_RDWR)) == -1)
	{
		perror("open");
		return -1;
	}
	struct v4l2_fmtdesc fmtdesc;
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0;  
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
	while ((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc)) == 0)
	{   
		printf("%s\n", fmtdesc.description);
		struct v4l2_frmsizeenum frmsize;
		frmsize.pixel_format = fmtdesc.pixelformat;
		frmsize.index = 0;
		while (!ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize))
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

	/*ioctl look support drives*/
	ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
	if(ret == -1)
	{
		perror("camera->init");
		return -1;
	}

	/*look support video capture yes or no*/
	if(!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		fprintf(stderr, "camera->init: device can not supprot V4L2_CAP_VIDEO_CAPTURE.\n");
		close(fd);
		return -1;
	}
	else
	{
		printf("driver:\t\t%s\n",capability.driver);
		printf("card:\t\t%s\n",capability.card);
		printf("bus_info:\t%s\n",capability.bus_info);
		printf("version:\t%d\n",capability.version);
		printf("capabilities:\t%x\n",capability.capabilities);

		if ((capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) == V4L2_CAP_VIDEO_CAPTURE) 
		{
			printf("Device %s: supports capture.\n", devpath);
		}
		if ((capability.capabilities & V4L2_CAP_STREAMING) == V4L2_CAP_STREAMING) 
		{
			printf("Device %s: supports streaming.\n", devpath);
		}
	}

	/*look device support video stream yes or no*/
	if(!(capability.capabilities & V4L2_CAP_STREAMING))
	{
		fprintf(stderr,"camera->init: device can not support V4L2_CAP_STREAMING.\n");
		close(fd);
		return -1;
	}


	/*set capture format YUYV*/
	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;			//永远都是这个类型
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;		//设置采集图片的格式
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
	format.fmt.pix.field = V4L2_FIELD_ANY;				//设置图片一行一行的采集
	ret = ioctl(fd, VIDIOC_S_FMT, &format);				//ioctl	是设置生效
	if(ret == -1)
	{
		perror("camera init");
	}
	else 
	{
		fprintf(stdout, "camera->init: picture format is yuyv\n");
	}
	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret == -1) 
	{
		perror("camera init");
		return -1;
	}
	/*向驱动申请缓存*/
	memset(&reqbufs, 0, sizeof(struct v4l2_requestbuffers));
	reqbufs.count	= REQBUFS_COUNT;					//缓存区个数
	reqbufs.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory	= V4L2_MEMORY_MMAP;					//设置操作申请缓存的方式:映射 MMAP
	ret = ioctl(fd, VIDIOC_REQBUFS, &reqbufs);			
	if (ret == -1) 
	{	
		perror("camera init");
		close(fd);
		return -1;
	}

	/*循环映射并入队*/
	for (i = 0; i < reqbufs.count; i++)
	{
		/*真正获取缓存的地址大小*/
		memset(&vbuf, 0, sizeof(struct v4l2_buffer));
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		vbuf.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &vbuf);
		if (ret == -1) 
		{
			perror("camera init");
			close(fd);
			return -1;
		}
		/*映射缓存到用户空间,通过mmap讲内核的缓存地址映射到用户空间,并切和文件描述符fd相关联*/
		bufs[i].length = vbuf.length;
		bufs[i].start = mmap(NULL, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, vbuf.m.offset);
		if (bufs[i].start == MAP_FAILED)
		{
			perror("camera init");
			close(fd);
			return -1;
		}
		/*每次映射都会入队,放入缓冲队列*/
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
		if (ret == -1) {
			perror("camera init");
			close(fd);
			return -1;
		}
	}
	camera_start(fd);
	return fd;
}

int camera_start(int fd)
{
	int ret;

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret == -1) 
	{
		perror("camera->start");
		return -1;
	}
	fprintf(stdout, "camera->start: start capture\n");
	return 0;
}

int camera_dqbuf(int fd, unsigned int *index)
{
	int ret;
	fd_set fds;
	struct timeval timeout;
	struct v4l2_buffer vbuf;

	while (1) 
	{
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		ret = select(fd + 1, &fds, NULL, NULL, &timeout);
		if (ret == -1) 
		{
			perror("camera->dbytesusedqbuf");
			if (errno == EINTR)
				return -1;
			else
				return -1;
		}
		else if (ret == 0) 
		{
			fprintf(stderr, "camera->dqbuf: dequeue buffer timeout\n");
			return -1;
		} 
		else 
		{
			vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			vbuf.memory = V4L2_MEMORY_MMAP;
			ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
			if (ret == -1) 
			{
				perror("camera->dqbuf");
				return -1;
			}
			*index = vbuf.index;
			return 0;
		}
	}
}

int camera_eqbuf(int fd, unsigned int index)
{
	int ret;
	struct v4l2_buffer vbuf;

	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	vbuf.index = index;
	ret = ioctl(fd, VIDIOC_QBUF, &vbuf);		//入队
	if (ret == -1) 
	{
		perror("camera->eqbuf");
		return -1;
	}
	return 0;
}

int camera_stop(int fd)
{
	int ret;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
	if (ret == -1) 
	{
		perror("camera->stop");
		return -1;
	}
	fprintf(stdout, "camera->stop: stop capture\n");
	return 0;
}

int camera_exit(int fd)
{
	int i;
	int ret;
	struct v4l2_buffer vbuf;
	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	for (i = 0; i < reqbufs.count; i++) 
	{
		ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
		if (ret == -1)
			break;
	}
	for (i = 0; i < reqbufs.count; i++)
		munmap(bufs[i].start, bufs[i].length);
	fprintf(stdout, "camera->exit: camera exit\n");
	return close(fd);
}

int buf_init(int fd)
{
	int i;
	struct v4l2_buffer vbuf;
	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;	
	for(i = 0; i < REQBUFS_COUNT; i++)
	{
		ioctl(fd, VIDIOC_DQBUF, &vbuf);
	}
	for(i = 0; i < REQBUFS_COUNT; i++)
	{
		vbuf.index = i;
		ioctl(fd, VIDIOC_QBUF, &vbuf);
	}
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

long rgb_to_jpeg(char *rgb, unsigned char ** jpeg, int width, int height)
{
	long jpeg_size;
	struct jpeg_compress_struct jcs;
	struct jpeg_error_mgr jem;
	JSAMPROW row_pointer[1];
	int row_stride;
	
	jcs.err = jpeg_std_error(&jem);
	jpeg_create_compress(&jcs);
	
	jpeg_mem_dest(&jcs, jpeg, &jpeg_size);
	
	jcs.image_width = width;
	jcs.image_height = height;

	jcs.input_components = 3;//1;
	jcs.in_color_space = JCS_RGB;//JCS_GRAYSCALE;

	jpeg_set_defaults(&jcs);
	jpeg_set_quality(&jcs, 70, TRUE);
	
	jpeg_start_compress(&jcs, TRUE);
	row_stride =jcs.image_width * 3;
	
	while(jcs.next_scanline < jcs.image_height){//对每一行进行压缩
		row_pointer[0] = &rgb[jcs.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&jcs, row_pointer, 1);
	}
	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);

	return jpeg_size;
}


