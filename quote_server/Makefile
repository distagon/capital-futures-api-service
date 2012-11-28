all : quote_server.exe

quote_server.exe : main.c quoteapi.c quoteapi.h command.c
	gcc -Wall -o quote_server main.c command.c quoteapi.c -lws2_32
    
clean :
	rm quote_server.exe *.o *.~

