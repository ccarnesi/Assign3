#include "DUMBheader.h"

void addMailBoxToEnd(mailNode* mail, mailNode* head);
mailNode* searchForMailBox(mailNode* head, char* mailName);
void addMessage(char * message, mailNode* current);
messageNode* fetchMessage(mailNode* mailBox);
int deleteMailBox(char* name, mailNode* head);


int main(int argc, char* argv[]){
        int run =1;
        mailNode* currentBox = malloc(sizeof(mailNode*));
        currentBox = NULL;
        mailNode* head = malloc(sizeof(mailNode*));
        head = NULL;
        while(run ==1){
                char payload[2048];
                int n = read(0,payload,5);
                int i = 0;
                char command[6];
                while (i<5){
                    command[i] = payload[i];
                    i++;
                }
                command[6] = '\0';
                if(strcmp("OPNBX", command)==0){
                        if(head == NULL){
                                //error
                        }else{
                                currentBox = searchForMailBox(head, NULL);
                                if(currentBox == NULL){
                                        //error
                                }else{
                                        //success
                                }
                        }
                }else if(strcmp("GDBYE", command)==0){
                        run = 0;
                        //success
                }else if(strcmp("CREAT", command)==0){
                        mailNode* newNode = malloc(sizeof(mailNode*));
                        newNode->next = NULL;
                        newNode->name = NULL; //pass in name of Box
                        if(head == NULL){
                                //make head new box
                                head = newNode;
                        }else{
                                //append to end
                                addMailBoxToEnd(newNode, head);
                        }
                }else if(strcmp("NXTMG", command)==0){
                        //must be in a box
                        if(currentBox == NULL){
                                //error
                        }else{
                                messageNode* mess = fetchMessage(currentBox);
                                if(mess ==NULL){
                                        //error
                                }else{
                                        //return message
                                }
                        }
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        if(currentBox==NULL){
                                //error
                        }else{
                                //pass message instead of NULL
                                addMessage(NULL,currentBox);
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
                        }else{
                                currentBox == NULL;
                                //success
                        }
                }else if(strcmp("HELLO", command)==0){
                        //return ok upon start else throw error
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

void addMessage(char * message, mailNode* current){
        messageNode* currentMess = current->messages;
        messageNode* newNode = malloc(sizeof(messageNode*));
        newNode->message = message;
        newNode->next = NULL;
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





