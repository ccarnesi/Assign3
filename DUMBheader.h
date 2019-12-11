#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <time.h>
#include <netdb.h>
#include <math.h>

typedef struct messageNode{
        char* message;
        int length;
        struct messageNode* next;
}messageNode;


typedef struct mailNode{
        char* name;
        pthread_mutex_t nodeLock;
        struct messageNode* messages;
        struct mailNode* next;
}mailNode;

typedef struct threadstruct{
        struct sockaddr_in* clientSock;
        int WRsocket;
        mailNode** head;
} threadstruct;








#endif
