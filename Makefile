all:
	gcc -Wall -g -c src/commom.c
	gcc -Wall -g src/client.c commom.o -o bin/client
	gcc -Wall -g src/server.c commom.o -o bin/server