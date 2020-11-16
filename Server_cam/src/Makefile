OBJ= server.o main.o uart.o
CC = arm-linux-gcc -c

all:${OBJ}
	cp ${OBJ} ../obj/
	make -C ./camera/

server.o:server.c ../include/server.h  ../include/camera.h ../include/fspwm.h
	${CC} $< -o ${@} -I../include/
main.o:main.c 
	${CC} $< -o ${@} -I../include/
uart.o:uart.c
	${CC} $< -o ${@}

.PHONY:clean

clean:
	rm *.o
	make clean -C ./camera/
