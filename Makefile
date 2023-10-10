all:
	gcc -Wall  -c src/commom.c -o obj/commom.o
	gcc -Wall  src/client.c obj/commom.o -o bin/client
	gcc -Wall  src/server.c obj/commom.o -o bin/server
