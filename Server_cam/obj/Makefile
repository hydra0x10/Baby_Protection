CC=arm-linux-gcc

all:smart_baby_room
	mv $< ../bin/
	
smart_baby_room:main.o server.o camera.o
	${CC} main.o server.o camera.o -o ${@} -L/home/linux/jpeg/jpeg-8/lib/ -lpthread -ljpeg
	#${CC} main.o server.o camera.o -o ${@} -lpthread -ljpeg

.PHONY:clean

clean:
	rm *.o
