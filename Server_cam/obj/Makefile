all:smart_baby_room
	mv $< ../bin/
	
smart_baby_room:main.o server.o camera.o
	arm-linux-gcc main.o server.o camera.o -o ${@} -L/home/linux/jpeg/jpeg-8/lib/ -lpthread -ljpeg

.PHONY:clean

clean:
	rm *.o
