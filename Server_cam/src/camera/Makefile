OBJ=camera.o
CC=/home/xu/gcc-4.9.4/bin/arm-linux-gnueabi-gcc -c

all:${OBJ} 
	cp $< ../../obj/

camera.o:camera.c ../../include/camera.h 
	${CC} $< -o $@ -I ../../include -I /home/xu/nfsdir/include
	#${CC} $< -o $@ -I ../../include -I /home/linux/jpeg/jpeg-8/include
.PHONY:clean

clean:
	rm *.o
