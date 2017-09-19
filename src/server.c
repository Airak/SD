#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 256
#define PORT_NUMBER 4563

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


int start_listening(int port){
    int listener, sockfd; // Socket file descriptors
    struct sockaddr_in serv_addr;
    // Opening socket to listen to connection:
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(listener, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(listener,5);
    return listener;
}


void execute_command(int sockfd){
    char buffer[BUFFER_SIZE];
    int n;
    
    // Reads command from opened socket:
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");

    // Prints out requested command:
    printf("Client request command: %s\n",buffer);

    // Sends command results to client:
    FILE *arq = popen(buffer, "r");
    bzero(buffer,256);
    while (fgets(buffer, sizeof(buffer), arq) != 0) {
        write(sockfd,buffer,strlen(buffer));
    }

    // Clears environment:
    bzero(buffer,256);
    printf("Command response successfully sent.\n");
    pclose(arq);
}



int main(int argc, char *argv[])
{
    int sockfd, listener;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int pid;

    // Just listens to socket:
    listener = start_listening(PORT_NUMBER);

    while(1){

        // Accepting Connection. Receiving command request:
        printf("Server's Physical layer listening to port %d...\n", PORT_NUMBER);
        sockfd = accept(listener, (struct sockaddr *) &client_addr, &client_len);
        if (sockfd < 0)
            error("ERROR on accept");
        printf("Connection stabilished from %s\n", inet_ntoa(client_addr.sin_addr));
        
        pid = fork();
        if(pid>0){
            close(sockfd); // Closes the accepted socket. and resume accepted loop
        } else {
            close(listener); // Closes listener socket
            execute_command(sockfd); // Do stuff with the accepted socket.
            close(sockfd); // When done, closes accepted socket
            return 0; // ends child process.
        }
        
    }
    return 0;
}
