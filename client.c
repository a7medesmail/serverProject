/*
  This is simple tcp client. It is demostrate how a tcp client work.
  You should compile this application with gcc tcpclient.c -o tcpclient
  This application take two arguments. These arguments are -p for define port
  and -s for target server. Example usage;
  ./tcpclient -s 127.0.0.1 -p 1888

*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>

//#define PORT 6571 hide it because take an argument
#define BUF_LEN  2000
#define STDIN 0
//#define SADDR "127.0.0.1" //I think this is -h, okay there is bugs, it is working, but if you connect right ip 127.0.0.1 then connect wrong, it will also connect,
//but if you connected wrong first it will not accepts it, you have to reset the server so it will not save the connection data
//also if you connect to 127.10.0.1 it will work IDK how --> BUT IT IS WORKING!
#define TV_SEC 10


void test_fds(fd_set testfds, int sock) {
int nread;
char buffer[BUF_LEN];



  if (FD_ISSET(STDIN, &testfds)) 
    {
    	ioctl(STDIN,FIONREAD,&nread);
		if (nread == 0) 
		{
       		printf("Keyboard done!\n");
  			exit(0);
		}
        nread = read(STDIN, buffer, nread);
		buffer[nread] = '\0';
		write(sock, buffer, nread);     //write means sending	
	}

    if (FD_ISSET(sock, &testfds)) 
	{
		ioctl(sock,FIONREAD,&nread);
		if (nread == 0)
		{
			printf("Socket done!\n");
			exit(0);
		}
	    nread = read(sock, buffer, nread); //read means receiving
		buffer[nread] = '\0';
        printf("%s", buffer);
    }
      
      
}//end

int main(int argc, char **argv)
{
    int sock, bytes_recieved;
    char send_data[BUF_LEN],recv_data[BUF_LEN];
    struct sockaddr_in server_addr;
    fd_set readfds,testfds;
    struct timeval tv;
	int maxfd,stdeof,c,ret;
	char *saddr = NULL, *sport = NULL;
	
	
	// ./smsclient -h server -p port -u user -p password   
	//take arguments, for explanation check server application
	printf("argc: %d\n", argc); //all numbers of the arguments   //this should be a loop but to understand it better, I used one by one, also it must be hided later on
	
	printf("argv[0]=%s\n", argv[0]); // name of the program " client "
	printf("argv[1]=%s\n", argv[1]);// -h
	printf("argv[2]=%s\n", argv[2]);// server IP
	printf("argv[3]=%s\n", argv[3]);// third argument -p ..
	printf("argv[4]=%s\n", argv[4]); // port
	printf("argv[5]=%s\n", argv[5]);//  -u
	printf("argv[6]=%s\n", argv[6]);// username
	printf("argv[7]=%s\n", argv[7]);// -p
	printf("argv[8]=%s\n", argv[8]);// password
	
	int port = atoi(argv[4]);//change port from string to int


/* hide this part because I take argumetns and use it elsewhere
while ((c = getopt (argc, argv, "p:s:")) != -1)
    switch (c)
    {
        case 's':
            saddr = optarg;
            break;
        case 'p':
            sport = optarg;
            break;
        case '?':
            fprintf(stderr,"Unrecognized option: -%c\n", optopt);
            return 1;
        default:
            abort ();
    }*/
//wow, this is how to manipulate args, and else where check it if sport, or port, Amazing!



    /* Open socket descriptor */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket");
        exit(1);
    }

    /* Fill structure with 0 */
    bzero(&(server_addr.sin_zero),sizeof server_addr);
    server_addr.sin_family = AF_INET;

    /* Port number defined? */
    if ( sport != NULL )
        server_addr.sin_port = htons(atoi(sport));
    else
        server_addr.sin_port = htons(port);

    /* Remote ip address defined? */
    /*
    if ( saddr != NULL )
        inet_pton(AF_INET, saddr, &(server_addr.sin_addr));
    else*/
        inet_pton(AF_INET, argv[2] , &(server_addr.sin_addr));  //here changed SADDR to argv[2]  


    /* Connect to remote server */
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("Connect");
        exit(1);
    }

    FD_ZERO(&readfds);
    FD_SET(STDIN, &readfds);
    FD_SET(sock, &readfds);

while (1) {
    testfds=readfds;

    tv.tv_sec = TV_SEC;
    tv.tv_usec = 0;

// don't care about writefds and exceptfds:
    ret=select(FD_SETSIZE, &testfds, (fd_set *)NULL, (fd_set *)NULL, &tv);
    test_fds(testfds,sock);//
/*
    switch (ret) {
case 0:
          printf("Timed out.\n");
  break;
case -1:
          printf("Select problem.\n");
          exit(1);
    default:
      test_fds(testfds,sock);
  break;
    }*/
    
  }//while(1)
}
