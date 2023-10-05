#include "commom.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSZ 1024

int main(int argc, char **argv)
{

    struct sockaddr_storage storage;
    if (addrParse(argv[1], argv[2], &storage) != 0)
    {
        logExit("Parameter error\n");
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1)
    {
        logExit("Error at socket creation\n");
    }
    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (connect(s, addr, sizeof(storage)) != 0)
    {
        logExit("Error at connect\n");
    }

    char buf[BUFSIZ];
    int inputType;

    struct action act;
    act.coordinates[0] = -1;
    act.coordinates[1] = -1;

    //First iteration
    bool first = 1;

    //Client win or loss
    bool gameover = 0;
    while(1){
        
        //Read from stdin
        memset(buf, 0, BUFSIZ);
        fgets(buf, BUFSIZ, stdin);

        //Buffer to error messages
        char errbuf[BUFSIZ];
    
        if(first && !gameover)
        {
            while((strcmp(buf, "start\n") != 0) && (strcmp(buf, "exit\n") != 0)){
                printf("Game hasn't started yet.\n");
                memset(buf, 0, BUFSIZ);
                fgets(buf, BUFSIZ, stdin);
            }
            first = 0;
        }

        else if(gameover)
        {
            while((strcmp(buf, "reset\n") != 0) && (strcmp(buf, "exit\n") != 0)){
                printf("Game over: enter reset or exit.\n");
                memset(buf, 0, BUFSIZ);
                fgets(buf, BUFSIZ, stdin);
            }
            gameover = 0;
        }

        do
        {
            inputType = processClientMessage(buf, errbuf, &act);
            if(inputType == -1)
            {
                puts(errbuf);
                memset(buf, 0, BUFSIZ);
                memset(errbuf, 0, BUFSIZ);
                fgets(buf, BUFSIZ, stdin);
            }
        }while(inputType == -1);

        char sendbuf[sizeof(struct action)];
        memcpy(sendbuf, &act, sizeof(struct action));
        send(s, sendbuf, sizeof(struct action), 0);

        //Whether client required exit
        if(inputType == 7)
        {
            close(s);
            exit(EXIT_SUCCESS);
        }
        
        char recvbuf[sizeof(struct action)];
        ssize_t sz = read(s, recvbuf, sizeof(struct action));
        if(sz == sizeof(struct action))
        {
            memcpy(&act, recvbuf, sizeof(struct action));
            int r = handleMessage_client(act);
            if(r == -1)
            { 
                close(s);
                return 0; 
            }
            // Client win or loss
            else if(r == 0)
            { gameover = true; }
        }
    }
    
}