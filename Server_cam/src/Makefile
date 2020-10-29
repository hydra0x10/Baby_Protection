OBJ= server.o main.o
CC = arm-linux-gcc -c

all:${OBJ}
	cp ${OBJ} ../obj/
	make -C ./camera/

server.o:server.c ../include/server.h  ../include/camera.h
	${CC} $< -o ${@} -I../include/ -lpthread
main.o:main.c 
	${CC} $< -o ${@} -I../include/

.PHONY:clean

clean:
	rm *.o
	make clean -C ./camera/
