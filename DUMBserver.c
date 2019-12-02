#include "DUMBheader.h"

void addMailBoxToEnd(mailNode* mail, mailNode* head);
mailNode* searchForMailBox(mailNode* head, char* mailName);
void addMessage(int size, char * message, mailNode* current);
messageNode* fetchMessage(mailNode* mailBox);
int deleteMailBox(char* name, mailNode* head);


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
                        printf("HEREOP\n");
                        if(head == NULL){
                                //error
                                printf("Error: No mail Box with that name\n");
                        }else{
                                char c = NULL;
                                int i = read(0, &c, 1);
                                int n = read(0, payload, sizeof(payload));
                                payload[n-1] = '\0';
                                currentBox = searchForMailBox(head, payload);
                                printf("currentBox: %s\n",currentBox->name);
                                if(currentBox == NULL){
                                        //error
                                        printf("Error: No mail box with that name\n");
                                }else{
                                        //success
                                        printf("OK!\n");
                                        char* success = "OK!";
                                        //send function to socket
                                }
                        }
                }else if(strcmp("GDBYE", command)==0){
                        run = 0;
                        //success
                }else if(strcmp("CREAT", command)==0){
                        printf("HERECR\n");
                        char c = NULL;
                        int i = read(0, &c, 1);
                        int n = read(0, payload, sizeof(payload));
                        payload[n-1] = '\0';
                        mailNode* newNode = malloc(sizeof(mailNode*));
                        newNode->next = NULL;
                        newNode->name = payload; //pass in name of Box
                        if(head == NULL){
                                //make head new box
                                head = newNode;
                        }else{
                                //append to end
                                addMailBoxToEnd(newNode, head);
                        }
                        printf("OK!\n");
                }else if(strcmp("NXTMG", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                                printf("Error: No box open\n");
                        }else{
                                messageNode* mess = fetchMessage(currentBox);
                                if(mess ==NULL){
                                        //error
                                        printf("Error:Empty mailbox\n");
                                }else{
                                        //return message
                                        printf("OK!%d!%s\n",mess->length, mess->message);
                                }
                        }
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        if(currentBox==NULL){
                                //error
                                printf("Error: No Box open\n");
                        }else{
                                //pass message instead of NULL
                                char c = NULL;
                                int i = read(0, &c, 1);
                                int n = read(0, payload, sizeof(payload));
                                payload[n-1] = '\0';
                                addMessage(n-1, payload,currentBox);
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

void addMailBoxToEnd(mailNode* mail, mailNode* head){
        mailNode* current = head;
        while(current->next != NULL){
                current = current->next;
        }
        current->next = mail;
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





