CC=/home/xu/gcc-4.9.4/bin/arm-linux-gnueabi-gcc

all:smart_baby_room
	mv $< ../bin/
	
smart_baby_room:main.o server.o camera.o
	${CC} main.o server.o camera.o uart.o -o ${@} -L/home/xu/nfsdir/lib -lpthread -ljpeg
	#${CC} main.o server.o camera.o uart.o -o ${@} -L/home/linux/jpeg/jpeg-8/lib/ -lpthread -ljpeg

.PHONY:clean

clean:
	rm *.o
