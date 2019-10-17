#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

// the port client will be connecting to
#define PORT 8192
// max number of bytes we can get at once
#define MAXDATASIZE 300
 
int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct hostent *he;
    // connector�s address information
    struct sockaddr_in their_addr;
     
    // if no command line argument supplied
    if(argc != 2)
    {
        fprintf(stderr, "Client-Usage: %s the_client_hostname\n", argv[0]);
        // just exit
        exit(1);
    }
     
    // get the host info
    if((he=gethostbyname(argv[1])) == NULL)
    {
        perror("gethostbyname()");
        exit(1);
    }
    else
        printf("Client-The remote host is: %s\n", argv[1]);
     
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket()");
        exit(1);
    }
    else
        printf("Client-The socket() sockfd is OK...\n");
     
    // host byte order
    their_addr.sin_family = AF_INET;
    // short, network byte order
    printf("Server-Using %s and port %d...\n", argv[1], PORT);
    their_addr.sin_port = htons(PORT);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    // zero the rest of the struct
    memset(&(their_addr.sin_zero), '\0', 8);
     
    if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect()");
        exit(1);
    }
    else
        printf("Client-The connect() is OK...\n");
     
    if((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("recv()");
        exit(1);
    }
    else
        printf("Client-The recv() is OK...\n");
     
    buf[numbytes] = '\0';
    printf("Client-Received: %s", buf);
    char inBuf[1024], buffer[1024];
	int bread = 0;
    int pid = fork();
    if (pid < 0)
    {
	printf("fork failed");
	exit(1);
    }
    if (pid > 0)
    while (1)
    {
	for (int i = 0; i < 1024; i++) buffer[i] = '\0';
	int bread = read(sockfd, buffer, sizeof(buffer));
	if (strcmp(buffer, "QUIT") == 0)
	{
		printf("got a quit signal");
		break;
	}
	printf("%s", buffer);
	strcpy(buffer, "");
    }
    if (pid == 0)
    while (1)
    {
	for (int i = 0; i < 1024; i++) inBuf[i] = '\0';
	FILE* fp = stdin;
	fgets(inBuf, 1024,fp);
	write(sockfd, inBuf, sizeof(inBuf));
	if (strcmp(inBuf, "QUIT") == 0) exit(1);
    }
    printf("Client-Closing sockfd\n");
    close(sockfd);
    return 0;
}
