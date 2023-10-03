#ifndef COMMOM_H
#define COMMOM_H

#include <stdlib.h>
#include <arpa/inet.h>

struct action {
    int type;
    int coordinates[2];
    int board[4][4];
};


//Functions

void logExit(const char *msg);

int addrParse(const char *addrStr, const char *portStr, struct sockaddr_storage *storage);

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize);

int ServerSockInit(const char *proto, const char *portStr,
                   struct sockaddr_storage *storage);

void printBoard(int  (*board)[4]);

void copyBoard(int  (*destination)[4], int  (*source)[4]);

int checkCoordinateReveal(int* coordinates, int  (*playerBoard)[4]);

int checkCoordinateFlag(int* coordinates, int  (*playerBoard)[4]);

void resetBoard(int  (*playerBoard)[4]);

//Client receives a message
int handleMessage_client(struct action msg);

//Server receives a message
int handleMessage_server(struct action msg, struct action* response, int  (*gameBoard)[4], int* revealed, bool* started);

#endif