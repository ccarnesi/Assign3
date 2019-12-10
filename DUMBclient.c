#include "DUMBheader.h"
#include <string.h>

void readTillNewLine();
void sendpackage(),runner(),mailboxHandler(char type[], char mailbox[]);


int main(int argc, char* argv[]){
	int i = 0;
	/*this is where we attempt to establish the connection three times*/
	int sock = 0, valread;
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM,0))<0){
		printf("\n socket creation error \n");
		return -1;
	}
	serv_addr.sin_family= AF_INET;
    	serv_addr.sin_addr.s_addr = inet_addr("128.6.13.144");
	serv_addr.sin_port = htons(6969);
	char hello[] = "HELLO";
	while(i<3){
		if(connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
			++i;
		}
		else{
			// we reached the server now lets see if hello works
		       sendpackage(hello,socket,0);
		       
		}
	}
	if(i==3){
		printf("Error, unable to connect to the server, terminating program");
		return -1;
	}

	
	/*if successfull call runner*/
	/*
	runner();
	*/
}

void runner(int socket){
	int run =0;
        while (run == 0){
            char command[8];
            char mailbox[26];
	    char message[2000];
	    char payload[2048];
            write(1, ">", 1);
	    printf("Welcome! Please designate which message box you would like to open\n");
            int n = read(0, command, sizeof(command));
            if(n<=7){
                command[n-1] = '\0';
            } 
            if(strcmp("open", command)==0){
		    /*after mailbox is open we can just start reading other commands*/
                    printf("What mailbox would you like to open?\n");
		    read(0,mailbox, sizeof(mailbox)); 
		    strcpy(payload,"OPNBX!");
		    strcpy(&payload[6],mailbox);
		    /*this is where we send off to server and await reply*/
		    sendpackage(payload,socket,3);
            }else if (strcmp("create", command)==0){
		    /*created mailbox but we still have to open one*/
                    printf("What would you like to call the mailbox?\n");
		    read(0,mailbox, sizeof(mailbox));
		    strcpy(payload,"CREAT!");
		    strcpy(&payload[6],mailbox);
		    sendpackage(payload,socket,2);
            }else if (strcmp("delete", command)==0){
		    /*expect to get somehting back from server but after that we are good*/
                    printf("Which mailbox would you like to delete?\n");
		    read(0,mailbox, sizeof(mailbox));
		    strcpy(payload,"DELBX!");
		    strcpy(&payload[6],mailbox);
		    sendpackage(payload,socket,6);
		    /*add in error checker*/
            }else if (strcmp("close", command)==0){
		    /*similar to close box if we close it then we're good*/
		    printf("What mail box would you like to close");
		    read(0,mailbox,sizeof(mailbox));
		    strcpy(payload,"ClSBX!");
		    strcpy(&payload[6],mailbox);
		    sendpackage(payload,socket,7);
            }else if (strcmp("next", command)==0){
		    /*mailbox handler should print next message that we are trying to get*/
		    strcpy(payload,"NXTMG");
		    sendpackage(payload,socket,4);
		    /*run error checker and print msg*/
            }else if (strcmp("put", command)==0){
                    printf("What message would you like to put in the mailbox?\n");
		    read(0,message,sizeof(message));
		    strcpy(&payload[6],message);
                    strcpy(payload, "PUTMG!");
		    sendpackage(payload,socket,5);
            }else if (strcmp("quit", command)==0){
		    // have to edit this to make it work smoother
                    strcpy(payload, "GDBYE!");
		    sendpackage(payload,socket,1);
                    //send load
                    run = 1;
            }else{
                    printf("Error: Unknown Command. Try again\n");
            }
        }



        return;
}

void readTillNewLine(){
        char c = '0';
        while(c!='\n'){
            read(1, &c, 1);
        }
}

/*method checks first three letters to see if we got an error or not*/
int checker(int socket,int command,int len){
	char message[8];
	read(socket,message,3);
	if(strcmp("OK!",message)==0){
		/*successful interaction with server now depending on what command it was figure it out*/
		/*important for when we are getting the next message must display that message*/
		if(command==1){
			printf("Succesfully communicated with server\n");
			return 0;
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
			return 0;
		}	
	}
	else{
		char substr[5];
		read(socket,&message[3],5);
		strcpy(substr,&message[3]);
		/*error happened now time to figure out what it was*/
		/* gd 1 , creat 2, 6 delbx 3 opnbx, 7 clsbx, 4 nxtmg, 5 putmg*/
		if(command==1){
			/* if we see anything then its an error*/
			if(substr[0]!='\0'){
				printf("Error, unable to close connection with the server");
				return -1;
			}
		}
		if(command ==2){
			if(strcmp("EXIST",substr)==0){
				printf("Error, can not create a box with the name of one that already exits\n");
				return -1;
			}
			else{
				printf("Error, your message is broken or malformed\n");
				return -1;
			}
		}
		if(command==3){
			if(strcmp("NEXST",substr)==0){
				printf("Error, cannot open a box that does not exist\n");
				return -1;
			}
			else if(strcmp("ALOPN",substr)==0){
				printf("Error, cannot open a box that is already openi\n");
				return -1;
			}
			else{
				printf("Error, box is opened by another user already, please try again at a later time\n");
				return -1;
			}
		}
		if(command==4){
			if(strcmp("EMPTY",substr)==0){
				printf("Error, no messages left in the message box to retrieve\n");
				return -1;
			}
			else if(strcmp("NOOPN",substr)==0){
				printf("Error, no message box has been open yet\n");
				return -1;
			}
			else{
				printf("Error, your message is broken or malformed\n");
				return -1;
			}

		}
		if(command==5){
			if(strcmp("NOOPN",substr)==0){
				printf("Error, no message box open to put messages in\n");
				return -1;
			}
			else{
				printf("Error, you rmessage is broken or malformed\n");
				return -1;
			}
		}
		if(command ==6){
			if(strcmp("NEXST", substr)==0){
			printf("Error, the message box you are trying to delete does not exist\n");
				return -1;
			}	
			else if(strcmp("OPEND",substr)==0){
				printf("Error, box is currently open so we cant delete it\n");
				return -1;
			}
			else if(strcmp("NOTMT",substr)==0){
				printf("Error, the mail box youa re trying to delete is not empty and still has messages inside\n");
				return -1;
			}
			else{
				printf("Error, your message is broken or malformed\n");
				return -1;
			}
		}
		if(command ==7){
			if(strcmp("NOOPN",substr)==0){
				printf("Error, you currently do not have that message box open so we cannot close it\n");
				return -1;
			}
			else{
				printf("Error, your message is broken or malformed\n");
				return -1;
			}

		}
	}	
}

// helper method that sends the message and awaits the response from the socket
void sendpackage(char * payload, int socket,int command){
	if(send(socket, payload, strlen(payload), 0)<0){
		printf("Send Failed");
		return;
	}
	// helper method that chekcs all of the possible errors or prints out correct message
	checker(socket,command,strlen(payload));
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
		    return;
}
