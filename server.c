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

/* Socket API headers */

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>



/* Definations */

#define DEFAULT_BUFLEN 512

//#define PORT 



void do_job(int fd) {

int length,rcnt;

char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];

int  recvbuflen = DEFAULT_BUFLEN;



    // Receive until the peer shuts down the connection

    do {

        rcnt = recv(fd, recvbuf, recvbuflen, 0);

        if (rcnt > 0) {

            printf("Message: ");  //changed to show what recieved recvbuf

            printf(recvbuf);

            

            

		

        // Echo the buffer back to the sender  // hided because don't want echo

        //if (recvbuf == "admin") //new

        rcnt = send( fd, recvbuf, rcnt, 0 );

            if (rcnt < 0) {

                printf("Send failed:\n");

                close(fd);

                break;

            }

            //printf("Bytes sent: %d\n", rcnt);

            

        



        }

        else if (rcnt == 0)

            printf("Connection closing...\n");

        else  {

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
	//PORT = argv[4]; // this is not working, so I replaced every PORT with argv[4]
	/*

	printf("Enter Port Number: ");

	scanf("%d", PORT);   //for now enter it manually until I learn how to do it from the argument ./server -d ddd -p xxxx user.txt, aslo for directory it is left for now

	this did not work it gave me "Segmentation fault (core dumped) error"
	*/
	
	//change the directory

        int result = chdir(argv[2]);

        if (result == 0) {
            printf("Changed directory to %s\n", argv[2]);
        } else {
            printf("Unable to change directory to %s\n", argv[2]);
        }
        
        
    //print working directory in 3 lines, uses dirent.h library
	char cwd[1024]; 
    getcwd(cwd, sizeof(cwd)); 
    printf("Current working dir: %s\n", cwd); 
	
	
	//to read the file given in -u user.txt
	FILE *fp = fopen("user.txt", "r"); //open file
	
	char line[256]; //a variable to put a line inside of it
	while (fgets(line, sizeof(line), fp) != NULL) //read each line by fgets() function
	{   //to split id, and password: 
		char *idStr = strtok(line, ":");  //char *x = "hi"  ---> means x is a pointer to the string 'hi'
		char *password = strtok(NULL, ":");
		
		int id = atoi(idStr);
		
		printf("The id is %d", id);  
		printf("The password is %s", password);
	}
	fclose(fp);//close the file whew!
 	//reading the txt file with the format given in APPENDIX (last page of the project details) is DONE!
 	
 	
	





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

memset( &local_addr, 0, sizeof(local_addr) );

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



printf("Concurrent  socket server now starting on port %d\n", port); //here argv[4] or port,  two places to change it

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

        do_job(fd);

        printf("Child finished their job!\n");

        close(fd);

        exit(0);

    }



}



// Final Cleanup

close(server);



}
