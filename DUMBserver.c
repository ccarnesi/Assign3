#include "DUMBheader.h"

int addMailBoxToEnd(mailNode* mail, mailNode** head);
mailNode* searchForMailBox(mailNode** head, char* mailName);
void addMessage(int size, char * message, mailNode* current);
messageNode* fetchMessage(mailNode* mailBox);
int deleteMailBox(char* name, mailNode** head);
int checkMailBoxConstraints(char* name);
void readTillEnd();
void threadFunc(void* args);
struct tm* getDateFunc();
void stdOut(char* ip, char* command, struct tm* date);
void stdErr(char* ip, char* error, struct tm* date);

pthread_mutex_t mainLock;


int main(int argc, char* argv[]){
    //init main lock
    pthread_mutex_init(&mainLock, NULL);

/*
    int server_fd, conn_fd, addrlen;
    struct sockaddr_in address;
    addrlen = sizeof(address);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
            printf("socket failed\n");
            return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("128.6.13.144");
    address.sin_port = htons(6969);

    if((bind(server_fd, (struct sockaddr *)&address, sizeof(address)))<0){
            printf("bind failed\n");
            return -1;
    }

    if((listen(server_fd, 3))<0){
            printf("listen failed\n");
            return -1;
    }
*/
    //if((conn_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))<0){
       //     printf("connection error\n");
     //       return -1;
   // }else{
  //          printf("WOOHOO\n");
  //
            threadFunc(NULL);
//    }
    return 0;    
}

void threadFunc(void* args){
        int run =1;
        

        struct tm* date = getDateFunc();

        mailNode* currentBox = malloc(sizeof(mailNode*));
        currentBox = NULL;
        mailNode* head = malloc(sizeof(mailNode*));
        head = NULL;
        while(run ==1){
                char* payload = malloc(sizeof(2048));
                int n = read(0,payload,5);
                int i = 0;
                char command[6];
                while (i<5){
                    command[i] = payload[i];
                    i++;
                }
                command[5] = '\0';
                if(strcmp("OPNBX", command)==0){
                                char c = ' ';
                                int i = read(0, &c, 1);
                                int n = read(0, payload, 2048);
                                payload[n-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        //printf("ER:WHAT?\n");
                                        stdErr(NULL, "ER:WHAT?", date);
                                        continue;
                                }
                                
                                if(currentBox!=NULL){
                                        //printf("ER:ALOPN\n");
                                        stdErr(NULL, "ER:ALOPN", date);
                                        continue;
                                }
                                mailNode* foundBox = searchForMailBox(&head, payload);
                                if(currentBox!= NULL && currentBox == foundBox){
                                        //printf("ER:OPEND\n");
                                        stdErr(NULL, "ER:OPEND", date);
                                        continue;
                                }else{
                                        currentBox = foundBox;
                                }
                                if(currentBox == NULL){
                                        //error
                                        //printf("ER:NXEST\n");
                                        stdErr(NULL, "ER:NXEST", date);
                                }else{
                                        //success
                                        if(pthread_mutex_trylock(&(currentBox->nodeLock))==0){
                                                //printf("OK!\n");
                                                stdOut(NULL, "OPNBX", date);
                                                continue;

                                        }
                                        currentBox = NULL;
                                        //printf("ER:OPEND\n");
                                        stdErr(NULL, "ER:OPEND", date);
                                        //send function to socket
                                }
                }else if(strcmp("CREAT", command)==0){
                        char c = ' ';
                        int i = read(0, &c, 1);
                        int n = read(0, payload, 2048);
                        payload[n-1] = '\0';
                        if(checkMailBoxConstraints(payload)==0){
                                //printf("ER:WHAT?\n");
                                stdErr(NULL, "ER:WHAT?", date);
                                continue;
                        }
                        mailNode* newNode = malloc(sizeof(mailNode));
                        newNode->next = NULL;
                        //newNode->nodeLock = malloc(sizeof(pthread_mutex_t));
                        pthread_mutex_init(&(newNode->nodeLock), NULL);
                        newNode->name = payload; //pass in name of Box
                        int ret = addMailBoxToEnd(newNode, &head);
                        if(ret==0){
                                //printf("ER:EXIST\n");
                                stdErr(NULL, "ER:EXIST", date);
                        }else{
                                //printf("OK!\n");
                                stdOut(NULL, "CREAT", date);
                        }
                }else if(strcmp("NXTMG", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //printf("ER:NOOPN\n");
                                stdErr(NULL, "ER:NOOPN", date);
                        }else{
                                messageNode* mess = fetchMessage(currentBox);
                                if(mess ==NULL){
                                        //printf("ER:EMPTY\n");
                                        stdErr(NULL, "ER:EMPTY", date);
                                }else{
                                        //printf("OK!%d!%s\n",mess->length, mess->message);
                                        stdOut(NULL, "NXTMG", date);
                                        
                                }
                        }
                        readTillEnd();
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        if(currentBox==NULL){
                                //printf("ER:NOOPN\n");
                                stdErr(NULL, "ER:NOOPN", date);
                                readTillEnd();
                                continue;
                        }else{
                                //pass message instead of NULL
                                char c = ' ';
                                int i = read(0, &c, 1);
                                c = '0';
                                int index = 0;
                                char bytes[10];
                                while(c!= '!'){
                                        read(0, &c, 1);
                                        bytes[index] = c;
                                        index++;
                                }
                                bytes[++index] = '\0';
                                int num = atoi(bytes);
                                ++num;
                                char* messToRead = malloc(sizeof(num));
                                int n = read(0, messToRead, num);
                                if(messToRead[n-1]!= '\n'|| n<num){//change to \0 when server is hooked up
                                        //printf("ER:WHAT?\n");
                                        stdErr(NULL, "ER:WHAT?", date);
                                        if(n<num){
                                                continue;
                                        }
                                        readTillEnd();
                                        continue;
                                }
                                messToRead[n-1] = '\0';
                                addMessage(num-1, messToRead,currentBox);
                                //printf("OK!%d\n", num-1);
                                stdOut(NULL, "PUTMG", date);
                        }
                }else if(strcmp("DELBX", command)==0){
                        if(head == NULL){
                                //report error
                                //printf("ER:NEXST\n");
                                stdErr(NULL, "ER:NEXST", date);
                                readTillEnd();
                        }else{
                                //Search list of boxes. if found return ok else report error
                                char c = ' ';
                                read(0, &c, 1);
                                int x = read(0, payload, 2048);
                                payload[x-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        //printf("ER:WHAT?\n");
                                        stdErr(NULL, "ER:WHAT?", date);
                                        continue;
                                }
                                int n = deleteMailBox(payload, &head);
                                if(n==1){
                                        //success
                                        //printf("OK!\n");
                                        stdOut(NULL, "DELBX", date);
                                }else if(n ==-1){
                                        //error not empty
                                        //printf("ER:NOTMT\n");
                                        stdErr(NULL, "ER:NOTMT", date);
                                }else if(n==0){
                                        //error doesnt exist
                                        //printf("ER:NEXST\n");
                                        stdErr(NULL, "ER:NEXST", date);
                                }else{
                                        //printf("ER:OPEND\n");
                                        stdErr(NULL, "ER:OPEND", date);
                                }
                        }
                }else if(strcmp("CLSBX", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                                //printf("ER:NOOPN\n");
                                stdErr(NULL, "ER:NOOPN", date);
                                readTillEnd();
                        }else{
                                char c = NULL;
                                int i = read(0, &c, 1);
                                int n = read(0, payload, 2048);
                                payload[n-1] = '\0';
                                if(currentBox!= NULL && strcmp(currentBox->name, payload)==0){
                                        pthread_mutex_unlock(&(currentBox->nodeLock));
                                        currentBox = NULL;
                                        //printf("OK!\n");
                                        stdOut(NULL, "CLSBX", date);
                                }else{
                                        //printf("ER:NOOPN\n");
                                        stdErr(NULL, "ER:NOOPN", date);
                                }
                        }
                }else if(strcmp("HELLO", command)==0){
                        //printf("HELLO DUMBv0 ready!\n");
                        stdOut(NULL, "HELLO", date);
                        readTillEnd();
                }else if(strcmp("GDBYE", command)==0){
                        stdOut(NULL, "GDBYE", date);
                        readTillEnd();
                        return;
                }else{
                        //printf("ER:WHAT?\n");
                        stdErr(NULL, "ER:WHAT?", date);
                        if(n==5){
                                readTillEnd();
                        }
                }
        }
}


void readTillEnd(){
        char c = ' ';
        while(c!= '\n'){
            read(0, &c, 1);
        }
}


int addMailBoxToEnd(mailNode* mail, mailNode** head){
        pthread_mutex_lock(&mainLock);
        mailNode* current = *head;
        if(current == NULL){
            *head = mail;
            pthread_mutex_unlock(&mainLock);
            return 1;
        }
        while(current->next != NULL){
                if(strcmp(current->name,mail->name)==0){
                        pthread_mutex_unlock(&mainLock);
                        return 0;
                }
                current = current->next;
        }
        current->next = mail;
        pthread_mutex_unlock(&mainLock);
        return 1;
}

mailNode* searchForMailBox(mailNode** head, char* mailName){
        mailNode* current = *head;
        while(current != NULL){
                if(strcmp(current->name, mailName)==0){
                        return current;
                }
                current = current->next;
        }
        return NULL;
}

void addMessage(int size, char * message, mailNode* current){
        messageNode* currentMess = current->messages;
        messageNode* newNode = malloc(sizeof(messageNode*));
        newNode->message = message;
        newNode->next = NULL;
        newNode->length = size;
        if(currentMess == NULL){
               current->messages = newNode; 
        }else{
                while(currentMess->next!= NULL){
                    currentMess = currentMess->next;
                }
                currentMess->next = newNode;
        }
}

messageNode* fetchMessage(mailNode* mailBox){
        if(mailBox->messages == NULL){
                return NULL;
        }else{
                messageNode* mess = mailBox->messages;
                mailBox->messages = mess->next;
                return mess;
        }
}

int deleteMailBox(char* name, mailNode** head){
        pthread_mutex_lock(&mainLock);
        mailNode* current = *head;
        mailNode* prev = NULL;
        if(strcmp(current->name, name)==0){
                if(current->messages!=NULL){
                            pthread_mutex_unlock(&mainLock);
                            return -1;
                }else if(pthread_mutex_trylock(&(current->nodeLock))!=0){
                                pthread_mutex_unlock(&mainLock);
                                return -2;
                }
                *head = (*head)->next;
                pthread_mutex_unlock(&mainLock);
                pthread_mutex_unlock(&(current->nodeLock));
                return 1;
        }
        while(current!=NULL){
                if(strcmp(current->name, name)==0){
                        if(current->messages!=NULL){
                                pthread_mutex_unlock(&mainLock);
                                return -1;
                        }else if(pthread_mutex_trylock(&(current->nodeLock))!=0){
                                pthread_mutex_unlock(&mainLock);
                                return -2;
                        }
                        prev->next = current->next;
                        pthread_mutex_unlock(&mainLock);
                        pthread_mutex_unlock(&(current->nodeLock));
                        return 1;
                }
                prev = current;
                current = current->next;
        }
        pthread_mutex_unlock(&mainLock);
        return 0;
}

int checkMailBoxConstraints(char* name){
        int counter = 0;
        int index = 0;
        while(name[index]!= '\0'){
                counter++;
                index++;
        }
        counter++;
        if(counter<=1){
                return 0;
        }
        if(counter>26){
            return 0;
        }
        if(isdigit(name[0])){
                return 0;
        }
        return 1;
}


struct tm* getDateFunc(){
        time_t t = time(NULL);
        struct tm* date = malloc(sizeof(struct tm*));
                date =localtime(&t);
        return date;
}

void stdOut(char* ip, char* command, struct tm* date){
        printf("%d %d %d %s\n", date->tm_year + 1900, date->tm_mday, date->tm_mon + 1, command);
}

void stdErr(char* ip, char* error, struct tm* date){
        printf("%d %d %d %s\n", date->tm_year + 1900, date->tm_mday, date->tm_mon + 1, error);
}




















