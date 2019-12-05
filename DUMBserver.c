#include "DUMBheader.h"

int addMailBoxToEnd(mailNode* mail, mailNode* head);
mailNode* searchForMailBox(mailNode* head, char* mailName);
void addMessage(int size, char * message, mailNode* current);
messageNode* fetchMessage(mailNode* mailBox);
int deleteMailBox(char* name, mailNode* head);
int checkMailBoxConstraints(char* name);
void readTillEnd();
void threadFunc(void* args);
void signalHandler(int num);

pthread_mutex_t mainLock;


int main(int argc, char* argv[]){
    //init main lock
    pthread_mutex_init(&mainLock, NULL);

    signal(SIGINT, signalHandler);

    int server_fd, conn_fd;
    struct sockaddr_in address;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
            printf("socket failed\n");
            return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(5269);

    if((bind(server_fd, (struct sockaddr *)&address, sizeof(address)))<0){
            printf("bind failed\n");
            return -1;
    }

    if((listen(server_fd, 3))<0){
            printf("listen failed\n");
            return -1;
    }

    if((conn_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)sizeof(address)))<0){
            printf("connection error\n");
            return -1;
    }else{
            printf("WOOHOO\n");
//            threadFunc(NULL);
    }
    return 0;    
}

void threadFunc(void* args){
        int run =1;
        mailNode* currentBox = malloc(sizeof(mailNode*));
        currentBox = NULL;
        mailNode* head = malloc(sizeof(mailNode*));
        head = NULL;
        while(run ==1){
                char* payload = malloc(sizeof(2048));
                memset(payload, '\0', 2048);
                int n = read(0,payload,5);
                int i = 0;
                char command[6];
                while (i<5){
                    command[i] = payload[i];
                    i++;
                }
                command[5] = '\0';
                if(strcmp("OPNBX", command)==0){
                                char c = NULL;
                                int i = read(0, &c, 1);
                                int n = read(0, payload, 2048);
                                payload[n-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        printf("ER:WHAT?\n");
                                        continue;
                                }
                                
                                if(head==NULL){
                                        printf("ER:NXEST\n");
                                        continue;
                                }else if(currentBox!=NULL){
                                        printf("ALOPN\n");
                                        continue;
                                }
                                mailNode* foundBox = searchForMailBox(head, payload);
                                if(currentBox!= NULL && currentBox == foundBox){
                                        printf("ER:OPEND\n");
                                        continue;
                                }else{
                                        currentBox = foundBox;
                                }
                                if(currentBox == NULL){
                                        //error
                                        printf("ER:NXEST\n");
                                }else{
                                        //success
                                        if(pthread_mutex_trylock(&(currentBox->nodeLock))==0){
                                                printf("OK!\n");
                                                continue;

                                        }
                                        currentBox = NULL;
                                        printf("ER:OPEND\n");
                                        //send function to socket
                                }
                }else if(strcmp("CREAT", command)==0){
                        char c = NULL;
                        int i = read(0, &c, 1);
                        int n = read(0, payload, 2048);
                        payload[n-1] = '\0';
                        if(checkMailBoxConstraints(payload)==0){
                                printf("ER:WHAT?\n");
                                continue;
                        }
                        mailNode* newNode = malloc(sizeof(mailNode*));
                        newNode->next = NULL;
                        pthread_mutex_init(&(newNode->nodeLock), NULL);
                        newNode->name = payload; //pass in name of Box
                        //append to end
                        if(head ==NULL){
                            head = newNode;
                            printf("OK!\n");
                            continue;
                        }
                        int ret = addMailBoxToEnd(newNode, head);
                        if(ret==0){
                                printf("ER:EXIST\n");
                        }else{
                                printf("OK!\n");
                        }
                }else if(strcmp("NXTMG", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                                printf("ER:NOOPN\n");
                        }else{
                                messageNode* mess = fetchMessage(currentBox);
                                if(mess ==NULL){
                                        //error
                                        printf("ER:EMPTY\n");
                                }else{
                                        //return message
                                        printf("OK!%d!%s\n",mess->length, mess->message);
                                        
                                }
                        }
                        readTillEnd();
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        if(currentBox==NULL){
                                //error
                                printf("ER:NOOPN\n");
                                readTillEnd();
                                //READ UNTIL END OF PAYLOAD
                                continue;
                        }else{
                                //pass message instead of NULL
                                char c = NULL;
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
                                        printf("ER:WHAT?\n");
                                        if(n<num){
                                                continue;
                                        }
                                        readTillEnd();
                                        continue;
                                }
                                messToRead[n-1] = '\0';
                                addMessage(num-1, messToRead,currentBox);
                                printf("OK!%d\n", num-1);
                        }
                }else if(strcmp("DELBX", command)==0){
                        if(head == NULL){
                                //report error
                                printf("ER:NEXST\n");
                                readTillEnd();
                        }else{
                                //Search list of boxes. if found return ok else report error
                                char c = NULL;
                                read(0, &c, 1);
                                int x = read(0, payload, 2048);
                                payload[x-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        printf("ER:WHAT?\n");
                                        continue;
                                }
                                int n = deleteMailBox(payload, head);
                                if(n==1){
                                        //success
                                        printf("OK!\n");
                                }else if(n ==-1){
                                        //error not empty
                                        printf("ER:NOTMT\n");
                                }else if(n==0){
                                        //error doesnt exist
                                        printf("ER:NEXST\n");
                                }else{
                                        printf("ER:OPEND\n");
                                }
                        }
                }else if(strcmp("CLSBX", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                                printf("ER:NOOPN\n");
                                readTillEnd();
                        }else{
                                char c = NULL;
                                int i = read(0, &c, 1);
                                int n = read(0, payload, 2048);
                                payload[n-1] = '\0';
                                if(currentBox!= NULL && strcmp(currentBox->name, payload)==0){
                                        pthread_mutex_unlock(&(currentBox->nodeLock));
                                        currentBox = NULL;
                                        printf("OK!\n");
                                }else{
                                        printf("ER:NOOPN\n");
                                }
                        }
                }else if(strcmp("HELLO", command)==0){
                        printf("HELLO DUMBv0 ready!\n");
                        readTillEnd();
                }else if(strcmp("GDBYE", command)==0){
                        readTillEnd();
                        return;
                }else{
                        printf("ER:WHAT?\n");
                        if(n==5){
                                readTillEnd();
                        }
                }
        }
}

void signalHandler(int num){
        printf("CTRL C pressed\n");
        exit(0);
}

void readTillEnd(){
        char c = NULL;
        while(c!= '\n'){
            read(0, &c, 1);
        }
}


int addMailBoxToEnd(mailNode* mail, mailNode* head){
        pthread_mutex_lock(&mainLock);
        mailNode* current = head;
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

mailNode* searchForMailBox(mailNode* head, char* mailName){
        mailNode* current = head;
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

int deleteMailBox(char* name, mailNode* head){
        pthread_mutex_lock(&mainLock);
        mailNode* current = head;
        mailNode* prev = NULL;
        if(strcmp(head->name, name)==0){
                if(head->messages!=NULL){
                            pthread_mutex_unlock(&mainLock);
                            return -1;
                }else if(pthread_mutex_trylock(&(head->nodeLock))!=0){
                                pthread_mutex_unlock(&mainLock);
                                return -2;
                }
                head = head->next;
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



