OBJ=camera.o
CC=arm-linux-gnueabihf-gcc -c

all:${OBJ} 
	cp $< ../../obj/

camera.o:camera.c ../../include/camera.h 
	${CC} $< -o $@ -I ../../include 
.PHONY:clean

clean:
	rm *.o
