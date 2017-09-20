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

// perfumaria
#define BOLD "\e[01;39m"
#define NO_COLOR "\e[00;39m"

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


void simulate_remote_call(char *hostnameOrIp, char server_number[]){
    char buffer[256];
    printf(BOLD"\n\t\t\t---------- Connecting to %s ----------\n"NO_COLOR, hostnameOrIp);

    // Connecting to server socket:
    printf("Connecting to server %s on port %d...\n", hostnameOrIp, PORT_NUMBER);
    int sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);
    
    // Sending command to server:
    strcpy(buffer,"cat maquina.");
    strcat(buffer,server_number);
    strcat(buffer,".log");
    int n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");

    // Receiving response:
    printf(BOLD"\n\t\t\t---------- %s log: ----------\n"NO_COLOR, &buffer[4]);
    bzero(buffer,256);
    while (read(sockfd,buffer,255) != 0) {
        printf("%s",buffer);
        bzero(buffer,256);
    }
    printf("\n");
    close(sockfd);
}


void request_from_peers(){
    FILE * fp;
    char *line;
    size_t len;
    ssize_t read;
    int server_number_int = 0;
    char server_number_string[256] = "0";

    fp = fopen("peers.txt", "r");
    if (fp == NULL)
        error("ERRO ao abrir arquivo de pares");

    line = malloc(256);
    bzero(line, 256);
    while ((read = getline(&line, &len, fp)) != -1)  {
        sprintf(server_number_string, "%d", server_number_int++);
        printf("Retrieved peer %s:\n", line);
        simulate_remote_call(line,server_number_string);
    }

    fclose(fp);
    if (line)
        free(line);
}


int main(int argc, char *argv[])
{
    request_from_peers();
    return 0;
}