#include "DUMBheader.h"

int addMailBoxToEnd(mailNode* mail, mailNode** head);
mailNode* searchForMailBox(mailNode** head, char* mailName);
void addMessage(int size, char * message, mailNode* current);
messageNode* fetchMessage(mailNode* mailBox);
int deleteMailBox(char* name, mailNode** head);
int checkMailBoxConstraints(char* name);
void readTillEnd(int client);
void* threadFunc(void* args);
struct tm* getDateFunc();
void stdOut(char* ip, char* command, struct tm* date);
void stdErr(char* ip, char* error, struct tm* date);

pthread_mutex_t mainLock;
mailNode* head = NULL;


int main(int argc, char* argv[]){
    //init main lock
    pthread_mutex_init(&mainLock, NULL);
    mailNode* head = malloc(sizeof(mailNode));
    head = NULL;


    if(argc<2||argc>2){
            printf("Invalid number of arguments\n");
            return -1;
    }
    int port = atoi(argv[1]);



    char hostBuffer[256];
    char* IPbuffer;
    int hostName;
    struct hostent* hostInfo;
    //get ip address of machine
    hostName = gethostname(hostBuffer, sizeof(hostBuffer));
    hostInfo = gethostbyname(hostBuffer);
    IPbuffer = inet_ntoa(*((struct in_addr*)hostInfo->h_addr_list[0]));


    int server_fd, addrlen;
    struct sockaddr_in address;
    addrlen = sizeof(address);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
            printf("socket failed\n");
            return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(IPbuffer);
    address.sin_port = htons(port);

    if((bind(server_fd, (struct sockaddr *)&address, sizeof(address)))<0){
            printf("bind failed\n");
            return -1;
    }

    if((listen(server_fd, 3))<0){
            printf("listen failed\n");
            return -1;
    }
    int conn_fd;
    pthread_attr_t attrib;
    pthread_attr_init(&attrib);
    pthread_attr_setdetachstate(&attrib, PTHREAD_CREATE_DETACHED);
        while((conn_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen))){
            threadstruct* threadArgs = malloc(sizeof(threadstruct));
            int sockNo = conn_fd;
            pthread_t thread;
            threadArgs->head = &head;
            threadArgs->clientSock = &address;
            threadArgs->WRsocket = sockNo;
            pthread_create(&thread, &attrib, threadFunc, (void *)threadArgs);

        }
    return 0;    
}

void* threadFunc(void* args){
        int run =1;
        
        threadstruct* threadArgs = (threadstruct *) args;
        struct tm* date = getDateFunc();

        //getting ip of client as string
        struct sockaddr_in* ipNameStruct = (struct sockaddr_in *)threadArgs->clientSock;
        struct in_addr ipAddr = ipNameStruct->sin_addr;
        char ipName[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, ipName, INET_ADDRSTRLEN);


        stdOut(ipName, "connected", date);
        mailNode* currentBox = malloc(sizeof(mailNode));
        currentBox = NULL;
        
        while(run ==1){
                char* payload = malloc(2048);
                memset(payload, '.', 2048);
                int n = read(threadArgs->WRsocket,payload,5);
                int i = 0;
                char command[6];
                while (i<5){
                    command[i] = payload[i];
                    i++;
                }
                command[5] = '\0';
                if(strcmp("OPNBX", command)==0){
                                char c = ' ';
                                int i = read(threadArgs->WRsocket, &c, 1);
                                int n = read(threadArgs->WRsocket, payload, 2048);
                                payload[n-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        write(threadArgs->WRsocket,"ER:WHAT?", 9);
                                        stdErr(ipName, "ER:WHAT?", date);
                                        continue;
                                }
                                
                                if(currentBox!=NULL){
                                        write(threadArgs->WRsocket, "ER:ALOPN", 9);
                                        stdErr(ipName, "ER:ALOPN", date);
                                        continue;
                                }
                                mailNode* foundBox = searchForMailBox(threadArgs->head, payload);
                                if(currentBox!= NULL && currentBox == foundBox){
                                        write(threadArgs->WRsocket, "ER:OPEND", 9);
                                        stdErr(ipName, "ER:OPEND", date);
                                        continue;
                                }else{
                                        currentBox = foundBox;
                                }
                                if(currentBox == NULL){
                                        //error
                                        write(threadArgs->WRsocket, "ER:NXEST", 9);
                                        stdErr(ipName, "ER:NXEST", date);
                                }else{
                                        //success
                                        if(pthread_mutex_trylock(&(currentBox->nodeLock))==0){
                                                write(threadArgs->WRsocket, "OK!", 4);
                                                stdOut(ipName, "OPNBX", date);
                                                continue;

                                        }
                                        currentBox = NULL;
                                        write(threadArgs->WRsocket, "ER:OPEND", 9);
                                        stdErr(ipName, "ER:OPEND", date);
                                        //send function to socket
                                }
                }else if(strcmp("CREAT", command)==0){
                        char c = ' ';
                        int i = read(threadArgs->WRsocket, &c, 1);
                        int n = read(threadArgs->WRsocket, payload, 2048);
                        payload[n-1] = '\0';
                        if(checkMailBoxConstraints(payload)==0){
                                write(threadArgs->WRsocket, "ER:WHAT?", 9);
                                stdErr(ipName, "ER:WHAT?", date);
                                continue;
                        }
                        mailNode* newNode = malloc(sizeof(mailNode));
                        newNode->next = NULL;
                        pthread_mutex_init(&(newNode->nodeLock), NULL);
                        newNode->name = payload; //pass in name of Box
                        int ret = addMailBoxToEnd(newNode, threadArgs->head);
                        if(ret==0){
                                write(threadArgs->WRsocket, "ER:EXIST", 9);
                                stdErr(ipName, "ER:EXIST", date);
                        }else{
                                write(threadArgs->WRsocket, "OK!", 4);
                                stdOut(ipName, "CREAT", date);
                        }
                }else if(strcmp("NXTMG", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                write(threadArgs->WRsocket, "ER:NOOPN", 9);
                                stdErr(ipName, "ER:NOOPN", date);
                        }else{
                                messageNode* mess = fetchMessage(currentBox);
                                if(mess ==NULL){
                                        write(threadArgs->WRsocket, "ER:EMPTY", 9);
                                        stdErr(ipName, "ER:EMPTY", date);
                                }else{
                                        //add number
                                        char numStr[5];
                                        sprintf(numStr, "%d", mess->length);
                                        char okay[15+mess->length];
                                        okay[0] = '\0';
                                        strcat(okay,"OK!");
                                        strcat(okay, numStr);
                                        strcat(okay,"!");
                                        strcat(okay,mess->message);
                                        okay[strlen(okay)] = '\0';
                                        write(threadArgs->WRsocket, okay,strlen(okay)+1);
                                        stdOut(ipName, "NXTMG", date);
                                        free(mess);
                                        
                                }
                        }
                        readTillEnd(threadArgs->WRsocket);
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        if(currentBox==NULL){
                                write(threadArgs->WRsocket, "ER:NOOPN", 9);
                                stdErr(ipName, "ER:NOOPN", date);
                                readTillEnd(threadArgs->WRsocket);
                                continue;
                        }else{
                                //pass message instead of NULL
                                char c = ' ';
                                int i = read(threadArgs->WRsocket, &c, 1);
                                c = '0';
                                int index = 0;
                                char bytes[10];
                                while(c!= '!'){
                                        read(threadArgs->WRsocket, &c, 1);
                                        bytes[index] = c;
                                        index++;
                                }
                                bytes[++index] = '\0';
                                int num = atoi(bytes);
                                //++num;
                                char* messToRead = malloc(sizeof(num));
                                int n = read(threadArgs->WRsocket, messToRead, num);
                                if(messToRead[num-1]!= '\0'){//change to \0 when server is hooked up
                                        //printf("ER:WHAT?\n");
                                        write(threadArgs->WRsocket, "ER:WHAT?", 9);
                                        stdErr(ipName, "ER:WHAT?", date);
                                        if(n<num){
                                                continue;
                                        }
                                        readTillEnd(threadArgs->WRsocket);
                                        continue;
                                }
                                addMessage(num-1, messToRead,currentBox);
                                //printf("OK!%d\n", num-1);
                                //add number
                                char okay[11] = "OK!";
                                okay[4] = '\0';
                                char numStr[5];
                                sprintf(numStr, "%d", num-1);
                                numStr[strlen(numStr)] = '\0';
                                strcat(okay, numStr);
                                write(threadArgs->WRsocket, okay, strlen(okay)+1);
                                stdOut(ipName, "PUTMG", date);
                        }
                }else if(strcmp("DELBX", command)==0){
                        if(threadArgs->head == NULL){
                                //report error
                                write(threadArgs->WRsocket, "ER:NXEST", 9);
                                stdErr(ipName, "ER:NEXST", date);
                                readTillEnd(threadArgs->WRsocket);
                        }else{
                                //Search list of boxes. if found return ok else report error
                                char c = ' ';
                                read(threadArgs->WRsocket, &c, 1);
                                int x = read(threadArgs->WRsocket, payload, 2048);
                                payload[x-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        write(threadArgs->WRsocket, "ER:WHAT?", 9);
                                        stdErr(ipName, "ER:WHAT?", date);
                                        continue;
                                }
                                int n = deleteMailBox(payload, threadArgs->head);
                                if(n==1){
                                        //success
                                        write(threadArgs->WRsocket, "OK!", 4);
                                        stdOut(ipName, "DELBX", date);
                                }else if(n ==-1){
                                        //error not empty
                                        write(threadArgs->WRsocket, "ER:NOTMT", 9);
                                        stdErr(ipName, "ER:NOTMT", date);
                                }else if(n==0){
                                        //error doesnt exist
                                        write(threadArgs->WRsocket, "ER:NEXST", 9);
                                        stdErr(ipName, "ER:NEXST", date);
                                }else{
                                        write(threadArgs->WRsocket, "ER:OPEND", 9);
                                        stdErr(ipName, "ER:OPEND", date);
                                }
                        }
                }else if(strcmp("CLSBX", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                                //printf("ER:NOOPN\n");
                                write(threadArgs->WRsocket, "ER:NOOPN", 9);
                                stdErr(ipName, "ER:NOOPN", date);
                                readTillEnd(threadArgs->WRsocket);
                        }else{
                                char c = NULL;
                                int i = read(threadArgs->WRsocket, &c, 1);
                                int n = read(threadArgs->WRsocket, payload, 2048);
                                payload[n-1] = '\0';
                                if(currentBox!= NULL && strcmp(currentBox->name, payload)==0){
                                        pthread_mutex_unlock(&(currentBox->nodeLock));
                                        currentBox = NULL;
                                        write(threadArgs->WRsocket, "OK!", 4);
                                        stdOut(ipName, "CLSBX", date);
                                }else{
                                        //printf("ER:NOOPN\n");
                                        write(threadArgs->WRsocket, "ER:NOOPN", 9);
                                        stdErr(ipName, "ER:NOOPN", date);
                                }
                        }
                }else if(strcmp("HELLO", command)==0){
                        //printf("HELLO DUMBv0 ready!\n");
                        stdOut(ipName, "HELLO", date);
                        write(threadArgs->WRsocket, "HELLO DUMBv0 ready!", 20);
                        readTillEnd(threadArgs->WRsocket);
                }else if(strcmp("GDBYE", command)==0){
                        stdOut(ipName, "GDBYE", date);
                        readTillEnd(threadArgs->WRsocket);
                        close(threadArgs->WRsocket);
                        return NULL;
                }else{
                        write(threadArgs->WRsocket, "ER:WHAT?", 9);
                        stdErr(ipName, "ER:WHAT?", date);
                        if(n==5){
                                readTillEnd(threadArgs->WRsocket);
                        }
                }
        }
        stdOut(ipName, "disconnected", date);
}


void readTillEnd(int client){
        char c = ' ';
        while(c!= '\0'){
            read(client, &c, 1);
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
        if(strcmp(current->name,mail->name)==0){
                pthread_mutex_unlock(&mainLock);
                return 0;
        }
        while(current->next != NULL){
                if(strcmp(current->name,mail->name)==0){
                        pthread_mutex_unlock(&mainLock);
                        return 0;
                }
                current = current->next;
        }
        if(strcmp(current->name,mail->name)==0){
                        pthread_mutex_unlock(&mainLock);
                        return 0;
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
        messageNode* newNode = malloc(sizeof(messageNode));
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
                pthread_mutex_destroy(&((*head)->nodeLock));
                mailNode* newHead = (*head)->next;
                mailNode* toFree = *head;
                free(toFree);
                *head = newHead;
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
                        pthread_mutex_destroy(&(prev->next->nodeLock));
                        mailNode* toFree = prev->next;
                        free(toFree);
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
        char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct","Nov", "Dec"};
        printf("%d%d %d %s %s %s\n", date->tm_hour, date->tm_min,date->tm_mday, months[date->tm_mon], ip, command);
}

void stdErr(char* ip, char* error, struct tm* date){
        char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct","Nov", "Dec"};
        fprintf(stderr,"%d%d %d %s %s %s\n", date->tm_hour, date->tm_min, date->tm_mday, months[date->tm_mon],ip, error);
}




















