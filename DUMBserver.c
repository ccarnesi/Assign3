#include "DUMBheader.h"

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
                        printf("MATCH\n");
                        if(head == NULL){
                                //error
                        }else{
                                //search for box if found send ok else report error
                        }
                }else if(strcmp("GDBYE", command)==0){
                        run = 0;
                }else if(strcmp("CREAT", command)==0){
                        if(head == NULL){
                                //make head new box
                        }else{
                                //append to end
                        }
                }else if(strcmp("NXTMG", command)==0){
                        //must be in a box
                        //if messages at that box are null report error
                        //delete from start of list of messages at that box and show that message
                }else if(strcmp("PUTMG", command)==0){
                        //must be in a box
                        //append to end of list of messages at that box

                }else if(strcmp("DELBX", command)==0){
                        if(head == NULL){
                                //report error
                        }else{
                                //Search list of boxes. if found return ok else report error
                        }
                }else if(strcmp("CLSBX", command)==0){
                        //must be in a box
                        //if in box close that box if arg is current box  else report error
                }else if(strcmp("HELLO", command)==0){
                        //return ok upon start else throw error
                }
        }
    return 0;    
}
