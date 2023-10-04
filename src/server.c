#include "commom.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFSZ 1024

int main(int argc, char **argv)
{
    //Need to receive 5 args
    if(argc != 5){
        logExit("Parameter error");
    }
    
    struct sockaddr_storage storage;
    if (ServerSockInit(argv[1], argv[2], &storage) != 0)
    {
        logExit("Parameter error");
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logExit("Error at socket creation");
    }

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) != 0)
    {
        logExit("Setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (bind(s, addr, sizeof(storage)) != 0)
    {
        logExit("Error at bind");
    }
    if (listen(s, 1) != 0)
    {
        logExit("Error at listen");
    }
    char addrStr[BUFSIZ];
    addrToStr(addr, addrStr, BUFSIZ);
    printf("Bound to %s, waiting connection\n", addrStr);

    if(strcmp(argv[3], "-i") != 0){
        logExit("Parameter error");
    }

    //Read input file
    int gameBoard[4][4];
    FILE* fp = fopen(argv[4], "r");
    if (NULL == fp) {
        logExit("Error opening the file");
    }

    char line[100]; 

    char *token;
    int val;

    //Read the game board
    int i = -1;
    while (fgets(line, sizeof(line), fp) != NULL) {
        i++; int j = -1;
        token = strtok(line, ",");
        while (token != NULL) {
            j++;
            val = atoi(token); // Converte o token em um inteiro
            gameBoard[i][j] = val;
            token = strtok(NULL, ",");
        }
    }

    bool startedGame = false;

    //Variable to save the number of points revealed
    int revealed = 0;
    
    while (1)
    {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);
        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1)
        {
            logExit("Accept error");
        }

        char caddrStr[BUFSIZ];
        addrToStr(caddr, caddrStr, BUFSIZ);
        printf("Connection from %s\n", caddrStr);
         
        char buf[sizeof(struct action)];
        while(1){
            memset(buf, 0, sizeof(struct action));
            size_t count = read(csock, buf, sizeof(struct action));
            if (count == sizeof(struct action)) {

                struct  action rcv, res;
                memcpy(&rcv, buf, sizeof(struct action));
                
                //Whether the client exit, close connection
                if(rcv.type == 7)
                {
                    close(csock);
                    exit(EXIT_SUCCESS);
                }

                handleMessage_server(rcv, &res, &gameBoard, &revealed, &startedGame);

                //Captures whether is the game start
                if(rcv.type == 0 && !startedGame){ startedGame = true; }

                memcpy(buf, &res, sizeof(struct action));
                send(csock, buf, sizeof(struct action), 0);

                //Whether the client win or lose, close connection after send the message
                if(rcv.type == 6 || rcv.type == 8)
                {
                    close(csock);
                    exit(EXIT_SUCCESS);
                }

            }
        }
    }
    exit(EXIT_SUCCESS);
}