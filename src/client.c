#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#define BUFFER_SIZE 256
#define PORT_NUMBER 4562


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int connectSocket(char *hostnameOrIp, int port_number){
    int sockfd; // socket file descriptor
    struct hostent *server;
    struct sockaddr_in serv_addr;
    struct ifreq ifr;

    // Opening socket to start connection:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // resolving host:
    server = gethostbyname(hostnameOrIp);
    printf("\nHost solved!");
    if (server == NULL)
        error("ERROR, no such host\n");
    bzero((char *) &serv_addr, sizeof(serv_addr)); // cleans serv_addr

    // configuring and connecting socket:
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
    serv_addr.sin_port = htons(port_number);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}


int main(int argc, char *argv[])
{
    int sockfd; // socket file descriptor
    char *hostnameOrIp;
    char buffer[256];
    int n;

    if(argc < 2)
        error("Hostname Required!");
    hostnameOrIp = argv[1];

    // Connecting to server socket:
    printf("Connecting to server %s on port %d...\n", hostnameOrIp, PORT_NUMBER);
    sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);

    // receiving command from keyboard:
    printf("Please enter command: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);

    // Sending command to server:
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");

    // Receiving response:
    bzero(buffer,256);
    while (read(sockfd,buffer,255) != 0) {
        printf("%s\n",buffer);
        bzero(buffer,256);
    }

    close(sockfd);
    printf("\nDONE!\n\n\n");
    return 0;
}
