#include "DUMBheader.h"

void readTillNewLine();
void mailboxHandler(char type[], char mailbox[]);


int main(int argc, char* argv[]){
        int run = 0;

        while (run == 0){
            char command[1024];
            char payload[2048];
            char mailbox[26];
            write(1, ">", 1);
            int n = read(0, command, sizeof(command));
            if(n<=7){
                command[n-1] = '\0';
            }
            
            if(strcmp("open", command)==0){
                    printf("What mailbox would you like to open?\n");
                    mailboxHandler("open", mailbox); 
            }else if (strcmp("create", command)==0){
                    printf("What would you like to call the mailbox?\n");
                    mailboxHandler("create", mailbox);
            }else if (strcmp("delete", command)==0){
                    printf("Which mailbox would you like to delete?\n");
                    mailboxHandler("delete", mailbox);
            }else if (strcmp("close", command)==0){
                    printf("You have just closed the mailbox\n");
            }else if (strcmp("next", command)==0){
                    strcpy(payload, "NXTMG!");
                    //send load
                    printf("%s\n", payload);
                    printf("The next message is:\n");
            }else if (strcmp("put", command)==0){
                    strcpy(payload, "PUTMG!");
                    printf("What message would you like to put in the mailbox?\n");
                    write(1, ">", 1);
                    int i = read(0, command, sizeof(command));
                    if (i<2){
                        printf("Error: No message was entered\n");
                    }else{
                            command[n] = '\0';
                            sprintf(payload+6, "%d", i-1);
                            payload[7] = '!';
                            payload[8] = '\0';
                            strcat(payload, command);
                            printf("PAYLOAD: %s\n", payload);

                    }
            }else if (strcmp("quit", command)==0){
                    strcpy(payload, "GDBYE!");
                    //send load
                    printf("%s\n", payload);
                    run = 1;
            }else{
                    printf("Error: Unknown Command. Try again\n");
            }
        }



        return 0;
}

void readTillNewLine(){
        char c = '0';
        while(c!='\n'){
            read(1, &c, 1);
        }
}


void mailboxHandler(char type[], char mailbox[]){
                    write(1, ">", 1);
                    int size = read(0, mailbox, sizeof(mailbox));
                    if(size<=25){
                            mailbox[size-1] = '\0';
                            if(isalpha(mailbox[0]) == 0){
                                printf("Error: Mailbox started with non-alphabetic char\n");
                            }
                            printf("Sent to server\n");
                            //try to send to server;
                     
                    }else{ 
                            printf("Error: Mailbox name was too long\n");
                            readTillNewLine();
                    }
}



