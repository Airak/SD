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
#define PORT_NUMBER 4563


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
    int i, connection;
    char* text_port_additional;
    int int_port_additional = 0;

    // check if all parameters were entered
    if(argc < 2)
        error("Number of Servers Required!");
    if(argc < 3)
        error("Hostname Required!"); 

    // get parameter values
    text_port_additional = argv[1];
    hostnameOrIp = argv[2];

    // get integer value for port
    for(i=0; i<strlen(text_port_additional); i++){
        int_port_additional = int_port_additional * 10 + ( text_port_additional[i] - '0' );
    }

    for(connection = 0; connection < int_port_additional; connection++){
        // Connecting to server socket:
        sockfd = connectSocket(hostnameOrIp, PORT_NUMBER + connection);

        // send request for the specific log file
        char server_number[2];
        server_number[0] = (char) (connection + '0');
        server_number[1] = '\0';
        bzero(buffer,256);
        strcpy(buffer,"cat maquina.");
        strcat(buffer,server_number);
        strcat(buffer,".log");

        printf("%s: ",&buffer[4]);

        // Sending command to server:
        i = write(sockfd,buffer,strlen(buffer));
        if (i < 0)
             error("ERROR writing to socket");

        // Receiving response:
        bzero(buffer,256);
        while (read(sockfd,buffer,255) != 0) {
            printf("%s\n",buffer);
            bzero(buffer,256);
        }

        close(sockfd);
    }
    
    return 0;
}
