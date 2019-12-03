#include "DUMBheader.h"

int addMailBoxToEnd(mailNode* mail, mailNode* head);
mailNode* searchForMailBox(mailNode* head, char* mailName);
void addMessage(int size, char * message, mailNode* current);
messageNode* fetchMessage(mailNode* mailBox);
int deleteMailBox(char* name, mailNode* head);
int checkMailBoxConstraints(char* name);


int main(int argc, char* argv[]){
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
                                int n = read(0, payload, sizeof(payload));
                                payload[n-1] = '\0';
                                if(checkMailBoxConstraints(payload)==0){
                                        printf("ER:WHAT?\n");
                                        continue;
                                }
                                if(head==NULL){
                                        printf("ER:NXEST\n");
                                }
                                mailNode* foundBox = searchForMailBox(head, payload);
                                if(currentBox == foundBox){
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
                                        printf("OK!\n");
                                        //send function to socket
                                }
                }else if(strcmp("CREAT", command)==0){
                        char c = NULL;
                        int i = read(0, &c, 1);
                        int n = read(0, payload, sizeof(payload));
                        payload[n-1] = '\0';
                        if(checkMailBoxConstraints(payload)==0){
                                printf("ER:WHAT?\n");
                                continue;
                        }
                        mailNode* newNode = malloc(sizeof(mailNode*));
                        newNode->next = NULL;
                        newNode->name = payload; //pass in name of Box
                        //append to end
                        if(head ==NULL){
                            head = newNode;
                            printf("Ok!\n");
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
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        if(currentBox==NULL){
                                //error
                                printf("ER:NOOPN\n");
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
                                if(messToRead[n-1]!= '\n'){//change to \0 when server is hooked up
                                        printf("ER:WHAT?\n");
                                        c = ' ';
                                        while(c!='\n'){//change to \0
                                            read(0,&c, 1);
                                        }
                                        continue;
                                }
                                messToRead[n-1] = '\0';
                                addMessage(num-1, messToRead,currentBox);
                                printf("OK!%d\n", num-1);
                        }
                }else if(strcmp("DELBX", command)==0){
                        if(head == NULL){
                                //report error
                        }else{
                                //Search list of boxes. if found return ok else report error
                                int n = deleteMailBox(NULL, head);
                                if(n==1){
                                        //success
                                }else{
                                        //error couldnt find
                                }
                        }
                }else if(strcmp("CLSBX", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                                printf("No box was open\n");
                        }else{
                                char c = NULL;
                                int i = read(0, &c, 1);
                                int n = read(0, payload, sizeof(payload));
                                payload[n-1] = '\0';
                                printf("%s\n", payload);
                                printf("%s\n", currentBox->name);
                                if(currentBox!= NULL && strcmp(currentBox->name, payload)==0){
                                        currentBox = NULL;
                                        printf("OK!\n");
                                }else{
                                        printf("Dont have that box currently open\n");
                                }
                        }
                }else if(strcmp("HELLO", command)==0){
                        //return ok upon start else throw error
                }else{

                }

        }
    return 0;    
}

int addMailBoxToEnd(mailNode* mail, mailNode* head){
        mailNode* current = head;
        while(current->next != NULL){
                if(strcmp(current->name,mail->name)==0){
                        return 0;
                }
                current = current->next;
        }
        current->next = mail;
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
        mailNode* current = head;
        mailNode* prev = NULL;
        if(strcmp(head->name, name)==0){
                head = head->next;
                return 1;
        }
        while(current!=NULL){
                if(strcmp(current->name, name)==0){
                        prev->next = current->next;
                        return 1;
                }
                prev = current;
                current = current->next;
        }
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
        if(counter>25){
            return 0;
        }
        if(isdigit(name[0])){
                return 0;
        }
        return 1;
}



