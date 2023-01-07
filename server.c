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
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/* Socket API headers */

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>



/* Definations */

#define DEFAULT_BUFLEN 512
char cwd[1024]; //this is original working directory, it has user.txt and subdirectories for each user





void do_job(int fd) {
//printf("\n dojob worked \n");  //this function only works once for every new connection
int length,rcnt;

char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];

int  recvbuflen = DEFAULT_BUFLEN;
bool is_auth = false;

rcnt = recv(fd, recvbuf, recvbuflen, 0);
const char* welcome_msg = "+OK My Chat Server v0.1 Ready."; //page 3 of the project, Server and Client comms
rcnt = send(fd, welcome_msg, strlen(welcome_msg), 0);//done it is working, but the client should send a message first, why ?
//rcnt = send( fd, recvbuf, rcnt, 0 ); //this must reply first so the client recive any other thing, why??.. finally fixed it from the client side

//variables for authentication
char user[64]; //password
int user_id; //real id on top because used a lot
char password[64]; //useless

bool checkList = false;
bool checkRET = false;
bool checkDEL = false;



    // Receive until the peer shuts down the connection
	//you are always inside this do until finishing the comms
    do {
    	
		if (is_auth)
		{
			char menu[1000] = "\n\nPlease choose your option:\n1) Read/Delete Messages\n2) Write Message to User\n3) Change config parameters\n4) Quit\nOption->";
			rcnt = send(fd, menu, strlen(menu), 0);
		}
        rcnt = recv(fd, recvbuf, recvbuflen, 0);
        
		//rcnt = send( fd, recvbuf, rcnt, 0 ); //this must reply first so the client recive any other thing, why??..
		
		
		
		if (strncmp(recvbuf, "USER", 4) == 0 ) //check if the user used USER command 
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
						printf("nice"); //hide this later
						
						
						
						
						char authentication[1000] = "Welcome "; 
						strcat(authentication, col1);
						
						
						rcnt = send(fd, authentication, strlen(authentication), 0);
						
						
						//create unique directory for each user "Subdirectory"
						char subD[100];
						sprintf(subD, "%d", user_id); //int to string
						char fullPath[250];
						sprintf(fullPath, "%s/%s", cwd, subD);
						mkdir(fullPath, 0700);
						
						
					}
						
			  	}
			  	
			}
			fclose(fp);//close the file whew!
		 	//reading the txt file with the format given in APPENDIX (last page of the project details) is DONE!
        }
        
        else if (strncmp(recvbuf, "LIST", 4) == 0 && is_auth) //done listing perfectly with date order
        {
        	printf("list is working");
        	
        	//change to the subdirectory
        	
        	char subD[100];
        	sprintf(subD, "%d", user_id); //int to string
			printf("%s\n you are in: ", subD);
			
			sprintf(subD, "%d", user_id);
        	chdir(subD);
        	
        	int count = 1;
	
			//open the current directory
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			
			if (d)
			{
				//create an array to save the data
				int data[100][4];
				int i = 0;
				
				while ((dir = readdir(d)) != NULL)
				{
					
					//list all .msg files
					if (strstr(dir->d_name, ".msg") != NULL && dir->d_name[0] != '.' && dir->d_type == DT_REG)
					{
						checkList = true;
						//split the filename
						char *token;
						token = strtok(dir->d_name, "_");
						
						int unique_id = count;
						int from_user_id;
						int time_stamp;
						int txt_size;
						
						int j = 1;
						while (token != NULL) 
						{
							if (j == 1)
								time_stamp = atoi(token);
							else if (j == 2)
								from_user_id = atoi(token);
							else if (j == 3)
								txt_size = atoi(token);
							
							token = strtok(NULL, "_");
							j++;
						}
						
						//save the data to the array
						data[i][0] = unique_id;
						data[i][1] = from_user_id;
						data[i][2] = time_stamp;
						data[i][3] = txt_size;
						
						count++;
						i++;
					}
				}
						closedir(d);
					if (checkList) //to make sure it is not empty
					{
						//sort the data based on time_stamp
						int j;
						for (i = 0; i < count-1; i++)
						{
							for (j = i+1; j < count-1; j++)
							{
								if (data[i][2] > data[j][2])
								{
									int temp[4];
									memcpy(temp, data[i], sizeof(temp));
									memcpy(data[i], data[j], sizeof(temp));
									memcpy(data[j], temp, sizeof(temp));
								}
							}
						}
						
						//display the result// hide it later
						for (i = 0; i < count-1; i++)
						{
							printf("%d %d %d %d\n", data[i][0], data[i][1], data[i][2], data[i][3]);
						}
						
						
						//store the result into a string
						char list[1000] = "";
						char x[100] = "+OK listing follows. \n";
						strcat(list, x);
						for (i = 0; i < count-1; i++)
						{
							char to_append[30];
							sprintf(to_append, "%d %d %d %d\n", data[i][0], data[i][1], data[i][2], data[i][3]);
							strcat(list, to_append);
						}
						
						rcnt = send(fd, list, strlen(list), 0);// send it
					}
			
				}
				if (!checkList)
				{
					char list[1000] = "-ERR there are no message";
					rcnt = send(fd, list, strlen(list), 0);// send it
				}
		}//LIST
		
        else if (strncmp(recvbuf, "RET", 3) == 0 && is_auth) //read message
        {
        	//printf("ret working");
        	int num;
        	sscanf(recvbuf, "RET %d", &num);
        //	printf("num is: %d",num);
        	
        //	char msg[1000];
			//read_msg(num);
		//	printf("the msg is: %s",msg);
			//rcnt = send(fd, msg, strlen(msg), 0);
        	
        	
        //	printf("list is working");
        	
        	//change to the subdirectory
        	
        	char subD[100];
        	sprintf(subD, "%d", user_id); //int to string
		//	printf("%s\n you are in: ", subD);
			
		//	sprintf(subD, "%d", user_id);
        	chdir(subD);
        	
        	int count = 1;
	
			//open the current directory
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			
			if (d)
			{
				//create an array to save the data
				int data[100][4];
				int i = 0;
				
				while ((dir = readdir(d)) != NULL)
				{
					
					//list all .msg files
					if (strstr(dir->d_name, ".msg") != NULL && dir->d_name[0] != '.' && dir->d_type == DT_REG)
					{
						checkRET = true;
						//split the filename
						char *token;
						token = strtok(dir->d_name, "_");
						
						int unique_id = count;
						int from_user_id;
						int time_stamp;
						int txt_size;
						
						int j = 1;
						while (token != NULL) 
						{
							if (j == 1)
								time_stamp = atoi(token);
							else if (j == 2)
								from_user_id = atoi(token);
							else if (j == 3)
								txt_size = atoi(token);
							
							token = strtok(NULL, "_");
							j++;
						}
						
						//save the data to the array
						data[i][0] = unique_id;
						data[i][1] = from_user_id;
						data[i][2] = time_stamp;
						data[i][3] = txt_size;
						
						count++;
						i++;
					}
				}
						closedir(d);
					if (checkRET) //to make sure it is not empty
					{
						checkRET = false;//programming tricks
						//sort the data based on time_stamp
						int j;
						for (i = 0; i < count-1; i++)
						{
							for (j = i+1; j < count-1; j++)
							{
								if (data[i][2] > data[j][2])
								{
									int temp[4];
									memcpy(temp, data[i], sizeof(temp));
									memcpy(data[i], data[j], sizeof(temp));
									memcpy(data[j], temp, sizeof(temp));
								}
							}
						}
						
						//display the result// hide it later
						for (i = 0; i < count-1; i++)
						{
							if (num == i+1) //when you arrive to the index, read and send it
							{
								checkRET = true;
								//read the content of the file
								char file_content[1000]="";
								
								char buf[1000] = "";
								int size = 0;
								
								//open the file
								sprintf(buf, "%d_%d_%d.msg", data[i][2], data[i][1], data[i][3]);
								FILE *fptr;
								fptr = fopen(buf, "rb");
								
								if (fptr == NULL)
								{
									printf("Error!");
									exit(1);
								}
								
								//read the file
								fseek(fptr, 0, SEEK_END);
								size = ftell(fptr);
								fseek(fptr, 0, SEEK_SET);
								fread(file_content, 1, size, fptr);
								
								char x[1000] = "+OK message follows \n";
								strncat(x, file_content, strlen(file_content)); // strncat() vs. strcat()   I love programming
								
								//send the content
								rcnt = send(fd, x, strlen(x), 0);
								
								fclose(fptr);
								
							}
							
						}
						
						
						
					}
			
			}
			if (!checkRET)
			{
				char ret[1000] = "-ERR no such message.";
				rcnt = send(fd, ret, strlen(ret), 0);// send it
			} 	
		}//RET
		
		else if (strncmp(recvbuf, "DEL", 3) == 0 && is_auth) //read message
        {
        	//printf("ret working");
        	int num;
        	sscanf(recvbuf, "DEL %d", &num);
        //	printf("num is: %d",num);
        	
        //	char msg[1000];
			//read_msg(num);
		//	printf("the msg is: %s",msg);
			//rcnt = send(fd, msg, strlen(msg), 0);
        	
        	
        //	printf("list is working");
        	
        	//change to the subdirectory
        	
        	char subD[100];
        	sprintf(subD, "%d", user_id); //int to string
		//	printf("%s\n you are in: ", subD);
			
			//sprintf(subD, "%d", user_id);
        	chdir(subD);
        	
        	int count = 1;
	
			//open the current directory
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			
			if (d)
			{
				//create an array to save the data
				int data[100][4];
				int i = 0;
				
				while ((dir = readdir(d)) != NULL)
				{
					
					//list all .msg files
					if (strstr(dir->d_name, ".msg") != NULL && dir->d_name[0] != '.' && dir->d_type == DT_REG)
					{
						checkDEL = true;
						//split the filename
						char *token;
						token = strtok(dir->d_name, "_");
						
						int unique_id = count;
						int from_user_id;
						int time_stamp;
						int txt_size;
						
						int j = 1;
						while (token != NULL) 
						{
							if (j == 1)
								time_stamp = atoi(token);
							else if (j == 2)
								from_user_id = atoi(token);
							else if (j == 3)
								txt_size = atoi(token);
							
							token = strtok(NULL, "_");
							j++;
						}
						
						//save the data to the array
						data[i][0] = unique_id;
						data[i][1] = from_user_id;
						data[i][2] = time_stamp;
						data[i][3] = txt_size;
						
						count++;
						i++;
					}
				}
					closedir(d);
				if (checkDEL) //to make sure it is not empty
				{
					checkDEL = false;//programming tricks
					//sort the data based on time_stamp
					int j;
					for (i = 0; i < count-1; i++)
					{
						for (j = i+1; j < count-1; j++)
						{
							if (data[i][2] > data[j][2])
							{
								int temp[4];
								memcpy(temp, data[i], sizeof(temp));
								memcpy(data[i], data[j], sizeof(temp));
								memcpy(data[j], temp, sizeof(temp));
							}
						}
					}
					
					//display the result// hide it later
					for (i = 0; i < count-1; i++)
					{
						if (num == i+1) //when you arrive to the index, DELETE and send it
						{
							checkDEL = true;
							//read the content of the file
							char buf[1000] = "";
							
							//choose the file
							sprintf(buf, "%d_%d_%d.msg", data[i][2], data[i][1], data[i][3]);
							
							//delete the file
							remove(buf);
							
							//send the content
							rcnt = send(fd, "+OK message deleted\n", strlen("+OK message deleted\n"), 0);
							
						}
					}	
				}
			}
			if (!checkDEL)
			{
				char ret[1000] = "-ERR no such message.";
				rcnt = send(fd, ret, strlen(ret), 0);// send it
			} 	
		}//DEL
		
		else if (strncmp(recvbuf, "SEND", 4) == 0 )
		{
			
			printf("SEND WORKING");
			int id;
			char msg[1000];
			sscanf(recvbuf, "SEND %d %s", &id, msg);
			
			int txtSize = strlen(msg);
			
		    // create message filename
		    char filename[1000];
		    time_t t = time(NULL);
		    struct tm *tm = localtime(&t);
		    
		    char subD[100];
        	sprintf(subD, "%d", id); //int to string
		    
		    sprintf(filename, "%ld_%s_%d.msg", t, subD, txtSize);
		    
		    printf("filename = %s\n", filename);
		    
		    
		    
			
		    char fullPath2[250];
			sprintf(fullPath2, "%s/%s", cwd, subD);
        	printf("fullPath2 = %s\n", fullPath2);
        	// create directory if it doesn't exist
			 /*struct stat st;
   			 if (stat(fullPath2, &st) == 0 && S_ISDIR(st.st_mode)) /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				printf("NNNNNNNNNNNN");
		        mkdir(fullPath2, 0700);
		    }*/
		    //change the directory
			chdir(subD);
			
			
			 
		    
			
		    // create full path for message file
			char fullPath[250];
			sprintf(fullPath, "%s/%s", fullPath2, filename);
		    
		    printf("full path = %s\n", fullPath);
		    
			// open message file
		    FILE *fp = fopen(fullPath, "w");
		    if (fp == NULL) {
		        printf("Error: Unable to open message file\n");
		        return -1;
		    }
		
		    // save text message to file
		    fprintf(fp, "%s", msg);
		
		    // close file
		    fclose(fp);
		    
		   
		}//SEND
		
		
		
		else if (strncmp(recvbuf, "QUIT", 4) == 0 )
		{
			rcnt = send(fd, "+OK Bye <UserID>. \n", strlen("+OK Bye <UserID>. \n"), 0);
			close(fd);
		}
		
		
		else if(!is_auth)
		{
			rcnt = send(fd, "\n Please Use USER  UserID Password command to login\n", strlen("\n Please Use USER  UserID Password command to login\n"), 0);
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
