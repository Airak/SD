#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <time.h>

#define BUFFER_SIZE 256
#define PORT_NUMBER 4563
#define FILE_SIZE 256

// perfumaria
#define BOLD "\e[01;39m"
#define NO_COLOR "\e[00;39m"

clock_t start, diff;

void error(const char *msg)
{
    perror(msg);
}


int connectSocket(char *hostnameOrIp, int port_number){
    int sockfd; // socket file descriptor
    struct hostent *server;
    struct sockaddr_in serv_addr;
    struct ifreq ifr;

    // Opening socket to start connection:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    } else {
        // resolving host:
        server = gethostbyname(hostnameOrIp);
        if (server == NULL) {
            error("ERROR, no such host\n");
        } else {
            bzero((char *) &serv_addr, sizeof(serv_addr)); // cleans serv_addr

            // configuring and connecting socket:
            serv_addr.sin_family = AF_INET;
            bcopy( (char *)server->h_addr,
                   (char *)&serv_addr.sin_addr.s_addr,
                   server->h_length);
            serv_addr.sin_port = htons(port_number);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
                error("ERROR connecting");
            } else {
                return sockfd;
            }
        }
    }
    return -1;
}


void simulate_remote_call(char *hostnameOrIp, char server_number[]){
    start = clock();
    char buffer[FILE_SIZE];
    printf(BOLD"\n\t\t\t---------- Connecting to %s ----------\n"NO_COLOR, hostnameOrIp);

    // Connecting to server socket:
    printf("Connecting to server %s on port %d...\n", hostnameOrIp, PORT_NUMBER);
    int sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);
    
    if (sockfd == -1) {
        printf("Could not connect with server");
    }else {
        // Sending command to server:
        strcpy(buffer,"cat maquina.");
        strcat(buffer,server_number);
        strcat(buffer,".log");
        int n = write(sockfd,buffer,strlen(buffer));
        if (n < 0){
             error("ERROR writing to socket");
        } else {
            // Receiving response:
            printf(BOLD"\n\t\t\t---------- %s log: ----------\n"NO_COLOR, &buffer[4]);
            bzero(buffer,FILE_SIZE);
            while (read(sockfd,buffer,255) != 0) {
                printf("%s",buffer);
                bzero(buffer,FILE_SIZE);
            }
            printf("\n");
            close(sockfd);
            diff = clock() - diff;
            printf(BOLD"\n\t\t\t---------- Connection info ----------\n"NO_COLOR);
            int msec = diff * 1000 / CLOCKS_PER_SEC;
            printf("Time taken for message exchange: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);
        }
    }
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
        line[strlen(line) - 1] = '\0'; // removing line break
        sprintf(server_number_string, "%d", server_number_int++);
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