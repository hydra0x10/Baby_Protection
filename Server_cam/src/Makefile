OBJ= server.o main.o uart.o
CC = /home/xu/gcc-4.9.4/bin/arm-linux-gnueabi-gcc -c

all:${OBJ}
	cp ${OBJ} ../obj/
	make -C ./camera/

server.o:server.c ../include/server.h  ../include/camera.h ../include/pwm.h
	${CC} $< -o ${@} -I../include/
main.o:main.c 
	${CC} $< -o ${@} -I../include/
uart.o:uart.c
	${CC} $< -o ${@}

.PHONY:clean

clean:
	rm *.o
	make clean -C ./camera/
