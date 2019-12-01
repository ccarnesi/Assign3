#ifndef HEADER_H
#define HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


typedef struct messageNode{
        char* message;
        struct messageNode* next;
}messageNode;


typedef struct mailNode{
        char* name;
        struct messageNode* messages;
        struct mailNode* next;
}mailNode;









#endif
