/*

  This code represent basic Concurrent Echo Server for linux platform. It's used to teach how basic server  running.

For compile code : # gcc concurrent.c -o concurrent

  -----------------------Copyright Devrim Seral---------------------------------

*/

/* C header files */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>

/* Socket API headers */

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>



/* Definations */

#define DEFAULT_BUFLEN 512

//#define PORT 



void do_job(int fd) {
//printf("\n dojob worked \n");  //this function only works once for every new connection
int length,rcnt;

char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];

int  recvbuflen = DEFAULT_BUFLEN;
bool is_auth = false;

const char* welcome_msg = "+OK My Chat Server v0.1 Ready."; //page 3 of the project, Server and Client comms
rcnt = send(fd, welcome_msg, strlen(welcome_msg), 0);//done it is working, but the client should send a message first, why ?


//variables for authentication
char user[64]; //password
int user_id; //real id
char password[64]; //useless

    // Receive until the peer shuts down the connection
	//you are always inside this do until finishing the comms
    do {

        rcnt = recv(fd, recvbuf, recvbuflen, 0);
        
		
		rcnt = send( fd, recvbuf, rcnt, 0 ); //this must reply first so the client recive any other thing, why??..
		
		
		
		if (strncmp(recvbuf, "USER", 4) == 0) //check if the user used USER command 
		// this part took me more than 8 hours, it can now authenticate by USER command, but there are extra letters, but it is working
		{
			
            
            sscanf(recvbuf, "USER %d %s", &user_id, user);
            
            //sscanf(recvbuf, "myPass %s", password);
            
            printf("user_id: %d\n", user_id);
				printf("user: %s\n", user);
					printf("password : %s\n", password);
            
            //to read the file given in -u user.txt, and check for username and password
			FILE *fp = fopen("user.txt", "r"); //open file

			char line[256]; //a variable to put a line inside of it
			while (fgets(line, sizeof(line), fp) != NULL) //read each line by fgets() function
			{   //to split id, and password: 
			//	char *idStr = strtok(line, ":");  //char *x = "hi"  ---> means x is a pointer to the string 'hi'
			//	char *password1 = strtok(NULL, ":");
				
			//	char pass1[strlen(password1) + 1];
			//	strcpy(pass1, password1);
				
				char col1[256], col2[256]; 
				sscanf(line, "%[^:]:%s", col1, col2);
				
				  
			//	printf ("password1:  %s",password1); 
				//int id = atoi(idStr); //atoi convert char string to int
				int id = atoi(col1);
				if (user_id == id  ) //check user and password (compare)
				{
					if (strcmp(user, col2) == 0)
					{
						is_auth = true;
						printf("nice");
					}
					else
					{
						printf("not the same");
					}	
			  	}
			  	else
			  	{
			  		is_auth = false; //must handle if the user changed the username without the password
				}
			}
			fclose(fp);//close the file whew!
		 	//reading the txt file with the format given in APPENDIX (last page of the project details) is DONE!
        }
        
        if (strncmp(recvbuf, "LIST", 4) == 0) //listing the messages in the subdirectory for each user
        //after two days of working, done reading the file name and display it in the required format
        {
        	printf("list is working");
        	//change to the subdirectory
        	
        	char subD[100];
        	sprintf(subD, "%d", user_id); //int to string
			printf("%s\n", subD);
			
        	int result = chdir(subD);
        	if (result == 0) 
			{
				printf("Changed directory to %s\n", subD);
				
				char cwd[1024]; 
			    getcwd(cwd, sizeof(cwd)); 
			    printf("Current working dir: %s\n", cwd); 
			    
				
			    char *ptr;
			    char file_name[50];
			    
			    DIR *dir;
			    dir = opendir(cwd); // i think it should be equal to 
			    int counter = 100, i=0;
			    struct dirent *entry;
				while( (entry = readdir( dir )) != NULL ) {
				    // copy the filename into a new variable
				    char *filename = strdup(entry->d_name);
				
				    // extract info from filename
				    int num = i++;
				    int from_id, unix_time_stamp, octets;
				    sscanf(filename, "%d_%d_%d.msg", &from_id, &unix_time_stamp, &octets);
				
				    // print the extracted info
				    printf("%d %d %d %d\n", num, from_id, unix_time_stamp, octets);
				
				    // free the memory allocated for the new variable
				    free(filename);
				    counter--;
				}
			    
			    
			    
			    
			    /*
			    struct dirent *entry;
			    int counter = 100, i=0;
				while( counter > 0 ) {
					printf("workiiiiiiiiiiing");
					
					
				    char *filename = entry->d_name;
				    
				    
				        // extract info from filename
				    int num = i++;
				    int from_id, unix_time_stamp, octets;
				    sscanf(filename, "%d_%d_%d.msg", &from_id, &unix_time_stamp, &octets);
				
				    // print the extracted info
				    printf("%d %d %d %d\n", num, from_id, unix_time_stamp, octets);

				    
				    /*
				    // extract info from filename
					char *p;
					p = strtok(filename, "_");
					int num = atoi(p);
					
					p = strtok(NULL, "_");
					int from_id = atoi(p);
					
					p = strtok(NULL, "_");
					int unix_time_stamp = atoi(p);
					
					p = strtok(NULL, ".msg");
					int octets = atoi(p);
					/*
					// print the extracted info
					printf("%d %d %d %d\n", num, from_id, unix_time_stamp, octets);
					*/
					
				}
				
				
			    
			    /*
				for(;;) 
				{
			        sprintf(file_name, "%d.msg", num);
			        if(access(file_name, F_OK) != -1) 
					{
						printf("workiiiiiiiiiiing");
			            ptr = strtok(file_name, "_");
			            printf("%d %s ", num, ptr);
			            ptr = strtok(NULL, "_");
			            printf("%s ", ptr);
			            ptr = strtok(NULL, "_");
			            printf("%s ", ptr);
			            ptr = strtok(NULL, ".");
			            printf("%s\n", ptr);
			            num++;
			        } 
					else 
					{
			            break;
			        }
			    }
				*/
				
       		
			else 
			{
            	printf("Unable to change directory to %s\n", subD);
       		}
       		
       	
       		
		}
        
        
		
		
        if (rcnt > 0) 
		{

            printf("Message: ");  //changed to show what recieved recvbuf

            printf(recvbuf);
        // Echo the buffer back to the sender  // hided because don't want echo

        //if (recvbuf == "admin") //new

        //rcnt = send( fd, recvbuf, rcnt, 0 ); // without this, a lot is not working, like wolcome message

           /* if (rcnt < 0) {

                printf("Send failed:\n");

                close(fd);

                break;

            }*/

            //printf("Bytes sent: %d\n", rcnt);
		}//print what received

        else if (rcnt == 0)

            printf("Connection closing...\n");

        else  
		{

            printf("Receive failed:\n");

            close(fd);

            break;

        }
        

    } while (rcnt > 0);

}








int main(int argc, char *argv[])  //how to take arguments, argv is array of string (the arguments), argc is thenumber of arguments

{
	
	
	
	printf("argc: %d\n", argc); //all numbers of the arguments   //this should be a loop but to understand it better, I used one by one, also it must be hided later on
	printf("argv[0]=%s\n", argv[0]); // name of the program " server "
	printf("argv[1]=%s\n", argv[1]);// first argument -d
	printf("argv[2]=%s\n", argv[2]);// second argument diretory
	printf("argv[3]=%s\n", argv[3]);// third argument -p ..
	printf("argv[4]=%s\n", argv[4]);
	printf("argv[5]=%s\n", argv[5]);// -u
	printf("argv[6]=%s\n", argv[6]);// user.txt
	int port = atoi(argv[4]);//change port from string to int
	
	
	
	//change the directory

        int result = chdir(argv[2]);

        if (result == 0) {
            printf("Changed directory to %s\n", argv[2]);
        } else {
            printf("Unable to change directory to %s\n", argv[2]);
        }
        
        
    //print working directory in 3 lines, uses dirent.h library, to check where we are, must be hided later
	char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("Current working dir: %s\n", cwd); 
	
	

 	
 	
	


int server, client;

struct sockaddr_in local_addr;

struct sockaddr_in remote_addr;

int length,fd,rcnt,optval;

pid_t pid;



/* Open socket descriptor */

if ((server = socket( AF_INET, SOCK_STREAM, 0)) < 0 ) { 

    perror("Can't create socket!");

    return(1);

}





/* Fill local and remote address structure with zero */

memset( &local_addr, 0, sizeof(local_addr) ); //limit amount of data sent or received

memset( &remote_addr, 0, sizeof(remote_addr) );



/* Set values to local_addr structure */

local_addr.sin_family = AF_INET;

local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

local_addr.sin_port = htons(port); //here argv[4] or ' port ' as int



// set SO_REUSEADDR on a socket to true (1):

optval = 1;

setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);



if ( bind( server, (struct sockaddr *)&local_addr, sizeof(local_addr) ) < 0 )

{

    /* could not start server */

    perror("Bind error");

    return(1);

}



if ( listen( server, SOMAXCONN ) < 0 ) {

		

        perror("listen");

        exit(1);

}



printf("Concurrent  socket server now starting on port %d\n", port); //here argv[4] or port,  two places to change it, the other before

printf("Wait for connection\n");



while(1) {  // main accept() loop
	

    length = sizeof remote_addr;
    



    if ((fd = accept(server, (struct sockaddr *)&remote_addr, \

          &length)) == -1) {

          perror("Accept Problem!");

          continue;

    }

	
	
    printf("Server: got connection from %s\n", \

            inet_ntoa(remote_addr.sin_addr));
            
	


    /* If fork create Child, take control over child and close on server side */
	
    if ((pid=fork()) == 0) {
		
        close(server);

        do_job(fd); //the problem is this function does not work until the client send a message first

        printf("Child finished their job!\n");

        close(fd);

        exit(0);
    }



}



// Final Cleanup

close(server);



}
