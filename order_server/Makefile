all : order_server.exe


order_server.exe : main.c orderapi.c command.c orderapi.h
	gcc -ggdb -Wall -DDEBUG -o order_server main.c orderapi.c command.c -lws2_32

clean :
	rm order_server.exe *.o *.~

