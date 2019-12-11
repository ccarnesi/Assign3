#include "DUMBheader.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void readTillNewLine();
void sendpackage(),runner(),mailboxHandler(char type[], char mailbox[]);


int main(int argc, char* argv[]){
	if(argc != 3){
		printf("Invalid number of arguments \n");
		return -1;
	}
	int i = 0;
	int hellowork = 0; 
	int port = atoi(argv[2]);
	/*this is where we attempt to establish the connection three times*/
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM,0))<0){
		printf("\n socket creation error \n");
		return -1;
	}
	serv_addr.sin_family= AF_INET;
    	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(port);
	char hello[] = "HELLO\0";
	while(i<3){
		if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
			++i;
			printf("iter 1\n");
		}
		else{
			// we reached the server now lets see if hello works
			printf("Conneted now trying hello\n");
			sendpackage(hello,sock,0,&hellowork);
			if(hellowork==1){
			       printf("DUMB mailbox is fully connected!\n");
			       break;
			}   
		}
	}
	if(i==3){
		printf("Error, unable to connect to the server, terminating program\n");
		return -1;
	}
	
	printf("Welcome! Please designate which message box you would like to open or create\n");
	runner(sock);
	return 0;
}

void runner(int socket){
	int run =0;
        while (run == 0){
            char command[8];
            char mailbox[26];
	    char message[2000];
	    char payload[2048];
            write(1, ">", 1);
            int n = read(0, command, sizeof(command));
            if(n<=7){
                command[n-1] = '\0';
            } 
	    /*if these things don twork then just use strcat didnt realize that was a thing earlier*/
            if(strcmp("open", command)==0){
		    /*after mailbox is open we can just start reading other commands*/
                    printf("What mailbox would you like to open?\n");
		    int fixer = read(0,mailbox, sizeof(mailbox)); 
		    strcpy(payload,"OPNBX!");
		    strcat(payload,mailbox);
		    mailbox[fixer] = '\0';
		    /*this is where we send off to server and await reply*/
		    sendpackage(payload,socket,3,&run);
            }else if (strcmp("create", command)==0){
		    /*created mailbox but we still have to open one*/
                    printf("What would you like to call the mailbox?\n");
		    int fixer = read(0,mailbox, sizeof(mailbox));
		    printf("printing what is inside of mailbox and length \n");
		    mailbox[fixer]= '\0'; 
		    strcpy(payload,"CREAT!");
		    strcat(payload,mailbox);
		    printf("%s %d \n", payload,strlen(payload));
		    sendpackage(payload,socket,2,&run);
            }else if (strcmp("delete", command)==0){
		    /*expect to get somehting back from server but after that we are good*/
                    printf("Which mailbox would you like to delete?\n");
		    int fixer =read(0,mailbox, sizeof(mailbox));
		    mailbox[fixer] = '\0';
		    strcpy(payload,"DELBX!");
		    strcat(payload,mailbox);
		    sendpackage(payload,socket,6,&run);
		    /*add in error checker*/
            }else if (strcmp("close", command)==0){
		    /*similar to close box if we close it then we're good*/
		    printf("What mail box would you like to close");
		    int fixer = read(0,mailbox,sizeof(mailbox));
		    mailbox[fixer] = '\0';
		    strcpy(payload,"ClSBX!");
		    strcat(payload,mailbox);
		    sendpackage(payload,socket,7,&run);
            }else if (strcmp("next", command)==0){
		    /*mailbox handler should print next message that we are trying to get*/
		    strcpy(payload,"NXTMG");
		    sendpackage(payload,socket,4,&run);
		    /*run error checker and print msg*/
            }else if (strcmp("put", command)==0){
                    printf("What message would you like to put in the mailbox?\n");
		    int len = read(0,message,sizeof(message));
		    // if this doesnt work swtich to sprinf
		    message[len];
		    char snum[5]; 
		    sprintf(snum,"%d",len);
                    strcpy(payload, "PUTMG!");
		    strcat(payload,snum);
		    strcat(payload,"!");
		    strcat(payload,message);
		    sendpackage(payload,socket,5,&run);
            }else if (strcmp("quit", command)==0){
		    // have to edit this to make it work smoother
                    strcpy(payload, "GDBYE!");
		    sendpackage(payload,socket,1,&run);
                    //send load
            }else{
                    printf("Error: Unknown Command. Try again\n");
            }
        }



        return;
}

void readTillNewLine(int sock){
        char c = '0';
        while(c!='\0'){
            read(sock, &c, 1);
        }
}

/*method checks first three letters to see if we got an error or not*/
int checker(int socket,int command,int len){
	printf("at checker\n");
	char message[8];
	message[0] = 0;
	int total = read(socket,message,3);
	printf("at this point\n");
	if(message[0]==0 && command==1){
		printf("Successfully quitting");
		return 69;
	}
	if(total==0){
		printf("could not read message from server");
		return -1;
	}
	message[total] = '\0';
	printf("%s, %d",message,strlen(message));
	if(strcmp("HEL",message)==0 && command ==0){
		readTillNewLine(socket);
		return 420;
	}
	if(strcmp("OK!",message)==0){
		/*successful interaction with server now depending on what command it was figure it out*/
		/*important for when we are getting the next message must display that message*/
		if(command==6 || command == 2 || command == 7 ||command == 5){
			printf("Successfully processed command\n");
		}
		else if(command ==3){
			printf("Successsfully opened box, you now have exclusive accesss to it\n");
		}
		else if(command==4){
			//need to print message that we received for the user so go until we reach !
			char curr = '0';
			int count =0;
			char dig[8];
			while(curr!='!'){
				read(socket,&curr,1);
				dig[count] = curr;
				++count;
			}
			char next[len];
			read(socket,&next,len);
			printf("Next message received: %s \n",next);
		}	
		readTillNewLine(socket);
		return 0;
			
	}
	else{
		char substr[5];
		read(socket,&message[3],5);
		strcpy(substr,&message[3]);
		printf("%s\n",message);
		/*error happened now time to figure out what it was*/
		/* gd 1 , creat 2, 6 delbx 3 opnbx, 7 clsbx, 4 nxtmg, 5 putmg*/
		if(command==1){
			/* if we see anything then its an error*/
			if(substr[0]!='\0'){
				printf("Error, unable to close connection with the server\n");
			}
		}
		else if(command ==2){
			if(strcmp("EXIST",substr)==0){
				printf("Error, can not create a box with the name of one that already exits\n");
			}
			else{
				printf("Error, your message is broken or malformed\n");
			}
		}
		else if(command==3){
			if(strcmp("NEXST",substr)==0){
				printf("Error, cannot open a box that does not exist\n");
			}
			else if(strcmp("ALOPN",substr)==0){
				printf("Error, cannot open a box that is already openi\n");
			}
			else{
				printf("Error, box is opened by another user already, please try again at a later time\n");
			}
		}
		else if(command==4){
			if(strcmp("EMPTY",substr)==0){
				printf("Error, no messages left in the message box to retrieve\n");
			}
			else if(strcmp("NOOPN",substr)==0){
				printf("Error, no message box has been open yet\n");
			}
			else{
				printf("Error, your message is broken or malformed\n");
			}

		}
		else if(command==5){
			if(strcmp("NOOPN",substr)==0){
				printf("Error, no message box open to put messages in\n");
			}
			else{
				printf("Error, you rmessage is broken or malformed\n");
			}
		}
		else if(command ==6){
			if(strcmp("NEXST", substr)==0){
			printf("Error, the message box you are trying to delete does not exist\n");
			}	
			else if(strcmp("OPEND",substr)==0){
				printf("Error, box is currently open so we cant delete it\n");
			}
			else if(strcmp("NOTMT",substr)==0){
				printf("Error, the mail box youa re trying to delete is not empty and still has messages inside\n");
			}
			else{
				printf("Error, your message is broken or malformed\n");
			}
		}
		else if(command ==7){
			if(strcmp("NOOPN",substr)==0){
				printf("Error, you currently do not have that message box open so we cannot close it\n");
			}
			else{
				printf("Error, your message is broken or malformed\n");
			}

		}
		readTillNewLine(socket);
		return -1;
	}	
}

// helper method that sends the message and awaits the response from the socket
void sendpackage(char * payload, int socket,int command,int * num){
	if(send(socket, payload, strlen(payload), 0)<0){
		printf("Send Failed");
		return;
	}
	// helper method that chekcs all of the possible errors or prints out correct message
	int fquit = checker(socket,command,strlen(payload));
	if(fquit ==69){
		*num = 1;
	}
	if (fquit ==420){
		*num =1;
	}
	return;

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
                            readTillNewLine(1);
                    }
		    return;
}
