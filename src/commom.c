#include "commom.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>

void logExit(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int addrParse(const char *addrStr, const char *portStr, struct sockaddr_storage *storage)
{
    if (addrStr == NULL || portStr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portStr);
    if (port == 0)
        return -1;
    port = htons(port);

    struct in_addr inaddr4;
    if (inet_pton(AF_INET, addrStr, &inaddr4.s_addr))
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr = inaddr4;
        return 0;
    }

    struct in6_addr inaddr6;
    if (inet_pton(AF_INET6, addrStr, &inaddr6))
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        memcpy(&(addr6->sin6_addr), &inaddr6, sizeof(inaddr6));
        return 0;
    }

    return -1;
}

void addrToStr(const struct sockaddr *addr, char *str, size_t strSize)
{
    int version;
    char addrStr[INET6_ADDRSTRLEN + 1] = "";
    uint16_t port;
    if (addr->sa_family == AF_INET)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        version = 4;
        if (!inet_ntop(AF_INET, &(addr4->sin_addr), addrStr, INET_ADDRSTRLEN))
        {
            logExit("ntop");
        }
        port = ntohs(addr4->sin_port);
    }
    else if (addr->sa_family == AF_INET6)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        version = 6;
        if (!inet_ntop(AF_INET6, &(addr6->sin6_addr), addrStr, INET6_ADDRSTRLEN))
        {
            logExit("ntop");
        }
        port = ntohs(addr6->sin6_port);
    }
    else
    {
        logExit("unknown protocol family.");
    }
    if (str)
    {
        snprintf(str, strSize, "IPv%d %s %hu", version, addrStr, port);
    }
}

int ServerSockInit(const char *proto, const char *portStr,
                   struct sockaddr_storage *storage)
{
    if (proto == NULL || portStr == NULL)
    {
        return -1;
    }
    uint16_t port = (uint16_t)atoi(portStr);
    if (port == 0){return -1;}
    port = htons(port);

    memset(storage, 0, sizeof(*storage));
    if (strcmp(proto, "v4") == 0)
    {
        struct sockaddr_in *addr4 = (struct sockaddr_in *)storage;
        addr4->sin_family = AF_INET;
        addr4->sin_port = port;
        addr4->sin_addr.s_addr = INADDR_ANY;
        return 0;
    }
    else if (strcmp(proto, "v6") == 0)
    {
        struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)storage;
        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = port;
        addr6->sin6_addr = in6addr_any;
        return 0;
    }
    else
    {
        return -1;
    }
}

void printBoard(int (*board)[4][4]){

    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){

            //Map the val in the board to the respective character
            int val = (*board)[i][j];
            char mappedChar;
            switch (val)
            {
            case -1:
                mappedChar = '*';
                break;
            
            case -2:
                mappedChar = '-';
                break;
            
            case -3:
                mappedChar = '>';
                break;
            
            default:
                mappedChar = val + '0';
                break;
            }

            printf("%c\t\t", mappedChar);
        }
        printf("\n");
    }
}

//Copy the board from source to destination
void copyBoard(int  (*destination)[4][4], int  (*source)[4][4]){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            (*destination)[i][j] = (*source)[i][j];
        }
    }
}

//Check whether the coordenates reveal are valid. Returns 0 whether the reveal is valid and the respective error type otherwise.
int checkCoordinateReveal(int* coordinates, int  (*playerBoard)[4][4]){
    int x = coordinates[0], y = coordinates[1];
    if(x >= 4 || x < 0){ return 9; }
    else if(y >= 4 || y < 0){ return 9; }
    else if((*playerBoard)[x][y] != -2 && (*playerBoard)[x][y] != -3){ return 11; }

    return 0;
}

//Check whether the flag insertion is valid. Returns 0 whether flag insertion is valid and the respective error type otherwise.
int checkCoordinateFlag(int* coordinates, int  (*playerBoard)[4][4]){
    int x = coordinates[0], y = coordinates[1];
    if(x >= 4 || x < 0){ return 9; }
    else if(y >= 4 || y < 0){ return 9; }
    else if((*playerBoard)[x][y] == -3){ return 12; }
    else if((*playerBoard)[x][y] >= 0){ return 13; }

    return 0;
}

int checkCoordinateRemFlag(int* coordinates, int  (*playerBoard)[4][4]){
    int x = coordinates[0], y = coordinates[1];
    if(x >= 4 || x < 0){ return 9; }
    else if(y >= 4 || y < 0){ return 9; }

    return 0;
}

//Reset the players board
void resetBoard(int  (*playerBoard)[4][4]){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            (*playerBoard)[i][j] = -2;
        }
    }
}

//Input parameters are the struct action received from the server
/*Returns:
-1 whether captures an error.
0  whether the client must close the connection with the server.
1  whether the client need to enter another action.
*/
int handleMessage_client(struct action msg){
    //Copy data
    int type = msg.type;
    int (*board)[4][4] = &msg.board;
    

    //Switch over the type
    switch (type)
    {
    case 3:
        printBoard(board);
        return 1;
        break;
    
    case 6:
        printf("YOU WIN!\n");
        printBoard(board);
        return 0;
        break;

    case 8:
        printf("GAME OVER!\n");
        printBoard(board);
        return 0;
        break;

    default:
        logExit("unknown action type.");
        return -1;
        break;
    }
}

//Input parameters are the struct action received from the client, the response struct address, 
//the current game board and the player's board.
void handleMessage_server(struct action msg, struct action* response, int  (*gameBoard)[4][4], int* revealed, bool* started){

    int (*resBoard)[4][4] = &response->board;
    int *resType = &response->type;
    
    int type = msg.type;
    int* coordinates = msg.coordinates;
    int x = coordinates[0], y = coordinates[1];

    switch (type)
    {
    //Case whether the client starts a game     
    case 0:
        *resType = 3;
        if(!*started){
            //Initialize Player's board
            int playerBoard[4][4];
            for(int i = 0; i < 4; i ++){
                for(int j = 0; j < 4; j++){
                    playerBoard[i][j] = -2;
                }
            }
            copyBoard(resBoard, &playerBoard);
        }

        break;

    //Case whether the client tries to reveal a point 
    case 1:
        

        //If the revealed point has a bomb:
        if((*gameBoard)[x][y] == -1){
            (*resType) = 8;
            copyBoard(resBoard, gameBoard);
        }
        else{
            (*resBoard)[x][y] = (*gameBoard)[x][y];
            (*revealed) += 1;

            //If the player reveals 13 points, the game is completed
            if((*revealed) == 13){
                (*resType) = 6;
                copyBoard(resBoard, gameBoard);
            }
            else{
                (*resType) = 3;
            }
        }
        break;
    
    //Client tries to insert a flag
    case 2:
       
        (*resBoard)[x][y] = -3;
        (*resType) = 3;
        break;

    case 4:
        if((*resBoard)[x][y] == -3){
            (*resBoard)[x][y] = -2;
        }
        *resType = 3;
        break;

    case 5:
        *revealed = 0;
        *resType = 3;
        resetBoard(resBoard);
        printf("starting new game\n");
        
    default:
        break;
    }
}

//Input parameters are the message entered by the user, the res buffer and the pointer to player board.
//Returns an integer >= 0 correspondind to the type whether the message is valid
int processClientMessage(const char* message, char* res, struct action* act){

    int* coordinates = act->coordinates;
    int (*playerBoard)[4][4] = &act->board;

    //If the client enter the start action
    if(!strcmp(message, "start\n")){
        act->type = 0;
        return 0; 
    }

    //If the message starts with reveal
    else if(!strncmp(message, "reveal", 6)){
        if(strlen(message) != 11)
        {
            strcpy(res, "error: command not found");
            return -1;
        }
        else
        {
            char  space = message[6], x = message[7], y = message[9], comma = message[8];
            
            if(space == ' ' && comma == ',' && (x >= '0' || x <= '9') && (y >= '0' || y <= '9')){
                int inputCoordinates[2] = {x - '0', y - '0'};

                int check = checkCoordinateReveal(inputCoordinates, &act->board);
                if(!check)
                {
                    coordinates[0] = inputCoordinates[0];
                    coordinates[1] = inputCoordinates[1];
                    act->type = 1; 
                    return 1; 
                }
                else if( check == 9 ){
                    strcpy(res, "error: invalid cell");
                    return -1;
                }
                else{
                    strcpy(res, "error: cell already revealed");
                    return -1;
                }

            } 
            else
            {
                strcpy(res, "error: command not found");
                return -1;
            } 
        }
        
    }

    //If the message starts with flag
    else if(!strncmp(message, "flag", 4)){
        if(strlen(message) != 9)
        {
            strcpy(res, "error: command not found");
            return -1;
        }
        else
        {
            char  space = message[4], x = message[5], y = message[7], comma = message[6];
            if(space == ' ' && comma == ',' && (x >= '0' || x <= '9') && (y >= '0' || y <= '9')){
                int inputCoordinates[2] = {x - '0', y - '0'};

                int check = checkCoordinateFlag(inputCoordinates, playerBoard);
                if(!check)
                {
                    coordinates[0] = inputCoordinates[0];
                    coordinates[1] = inputCoordinates[1];
                    act->type = 2;  
                    return 2; 
                }
                else if( check == 9 ){
                    strcpy(res, "error: invalid cell");
                    return -1;
                }
                else if( check == 12 ){
                    strcpy(res, "error: cell already has a flag");
                    return -1;
                }
                else{
                    strcpy(res, "error: cannot insert flag in revealed cell");
                    return -1;
                }
            } 
            else
            {
                strcpy(res, "error: command not found");
                return -1;
            } 
        }
    }

    //If the message starts with remove_flag
    else if(!strncmp(message, "remove_flag", 11)){
        if(strlen(message) != 16)
        {
            strcpy(res, "error: command not found");
            return -1;
        }
        else
        {
            char  space = message[11], x = message[12], y = message[14], comma = message[13];
            if(space == ' ' && comma == ',' && (x >= '0' || x <= '9') && (y >= '0' || y <= '9'))
            {
                int inputCoordinates[2] = {x - '0', y - '0'};

                int check = checkCoordinateRemFlag(inputCoordinates, playerBoard);
                if(!check)
                {
                    int x = inputCoordinates[0], y = inputCoordinates[1];

                    //If tries to remove a flag that doesnt exist
                    if((*playerBoard)[x][y] != -3)
                    {
                        act->type = 0;
                        return 0;
                    }
                    else
                    {
                        coordinates[0] = inputCoordinates[0];
                        coordinates[1] = inputCoordinates[1];
                        act->type = 4;  
                        return 4; 
                    }
                }
                else{
                    strcpy(res, "error: invalid cell");
                    return -1;
                }
            }
            else
            {
                strcpy(res, "error: command not found");
                return -1;
            } 
        }
    }

    //If its reset message
    else if(!strcmp(message, "reset\n")){
        act->type = 5; 
        return 5; 
    }
    //If its exit message
    else if(!strcmp(message, "exit\n")){
        act->type = 7; 
        return 7;
    }
    else
    {
        strcpy(res, "error: command not found");
        return -1;
    }
}