/*
Ahmed Esmail    21906571

Today 20/11/2022 I have started understanding the project, it is simple, there is client and server functions,
client has has 6 commands, 1- User, 2- LIST, 3- RET, 4-DEL, 5- SEND, 6- QUIT
each with different arguments, but the user will not use them, instead they will use a menue interface with numbers.


  This code represent basic Concurrent Echo Server for linux platform. It's used to teach how basic server  running.
For compile code : # gcc concurrent.c -o concurrent
  -----------------------Copyright Devrim Seral---------------------------------


  This is simple tcp client. It is demostrate how a tcp client work.
  You should compile this application with gcc tcpclient.c -o tcpclient
  This application take two arguments. These arguments are -p for define port
  and -s for target server. Example usage;
  ./tcpclient -s 127.0.0.1 -p 1888

*/





#include <iostream>
#include <iostream>
#include<string>
#include<string.h>
#include<iostream>
#include<stdio.h>
#include<fstream>
#include<cstdio>
#include<sstream>
using namespace std;


/* C header files */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Socket API headers */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Definations */
#define DEFAULT_BUFLEN 512
#define PORT XXXX(Change me!)


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

#define PORT 1080
#define BUF_LEN  2000
#define STDIN 0
#define SADDR "127.0.0.1"
#define TV_SEC 10


void test_fds(fd_set testfds, int sock) {
int nread;
char buffer[BUF_LEN];

  if (FD_ISSET(STDIN, &testfds)) {
ioctl(STDIN,FIONREAD,&nread);
if (nread == 0) {
        printf("Keyboard done!\n");
  exit(0);
}
        nread = read(STDIN, buffer, nread);
buffer[nread] = '\0';
write(sock, buffer, nread);
      }

      if (FD_ISSET(sock, &testfds)) {
ioctl(sock,FIONREAD,&nread);
if (nread == 0) {
        printf("Socket done!\n");
  exit(0);
}
        nread = read(sock, buffer, nread);
buffer[nread] = '\0';
        printf("%s", buffer);
      }
}

int main(int argc, char **argv)
{
    int sock, bytes_recieved;
    char send_data[BUF_LEN],recv_data[BUF_LEN];
    struct sockaddr_in server_addr;
    fd_set readfds,testfds;
    struct timeval tv;
int maxfd,stdeof,c,ret;
char *saddr = NULL, *sport = NULL;


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
    }

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
        server_addr.sin_port = htons(PORT);

    /* Remote ip address defined? */
    if ( saddr != NULL )
        inet_pton(AF_INET, saddr, &(server_addr.sin_addr));
    else
        inet_pton(AF_INET, SADDR , &(server_addr.sin_addr));


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
    }
  }
}

void do_job(int fd) {
int length,rcnt;
char recvbuf[DEFAULT_BUFLEN],bmsg[DEFAULT_BUFLEN];
int  recvbuflen = DEFAULT_BUFLEN;

    // Receive until the peer shuts down the connection
    do {
        rcnt = recv(fd, recvbuf, recvbuflen, 0);
        if (rcnt > 0) {
            printf("Bytes received: %d\n", rcnt);

        // Echo the buffer back to the sender
        rcnt = send( fd, recvbuf, rcnt, 0 );
            if (rcnt < 0) {
                printf("Send failed:\n");
                close(fd);
                break;
            }
            printf("Bytes sent: %d\n", rcnt);

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



/*int main()
{
int server, client;
struct sockaddr_in local_addr;
struct sockaddr_in remote_addr;
int length,fd,rcnt,optval;
pid_t pid;

/* Open socket descriptor
if ((server = socket( AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Can't create socket!");
    return(1);
}*/


/* Fill local and remote address structure with zero */
memset( &local_addr, 0, sizeof(local_addr) );
memset( &remote_addr, 0, sizeof(remote_addr) );

/* Set values to local_addr structure */
local_addr.sin_family = AF_INET;
local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
local_addr.sin_port = htons(PORT);

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

printf("Concurrent  socket server now starting on port %d\n",PORT);
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



