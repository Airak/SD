#include "chord.h"
#include "middleware.h"
#include <arpa/inet.h>

#define BOOTSTRAPPER_IP ""

void init_ring_with_bootstrapper();
void send_heartbeat();
void receive_heartbeat();
void check_next_peer();

int main(int argc, char *argv[])
{
    int sockfd, listener;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int pid;

    init_ring_with_bootstrapper();

    // Just listens to socket:
    listener = start_listening(PORT_NUMBER);

    while(1){

        // Accepting Connection. Receiving command request:
        log(LOG_LEVEL_INFO, "Server's Physical layer listening to port %d...\n", PORT_NUMBER);
        sockfd = accept(listener, (struct sockaddr *) &client_addr, &client_len);
        if (sockfd < 0)
            error("ERROR on accept");
        log(LOG_LEVEL_INFO, "Connection stabilished from %s\n", inet_ntoa(client_addr.sin_addr));

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


void init_ring_with_bootstrapper(){

    ifstream fp;
    string line, space = " ";
    char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);

    fp.open(PEERS_FILE);
    if (!fp.is_open())
        printf("ERRO ao abrir arquivo de pares");

    getline(fp, line); // Discards first line (commented line)
    getline(fp,line); // first ip is always self.

    // Gets self ip
    size_t pos = 0;
    string delimiter = " ";
    pos = line.find(delimiter);
    line.erase(0, pos + delimiter.length());
    strcpy(buffer, line.c_str());

    strcpy(buffer, BOOTSTRAPPER_IP);
    int sockfd = connectSocket(buffer, PORT_NUMBER);

    if (sockfd == -1)
        error("Could not connect with boostrapper server");

    // Sending IP to bootstrap server:
    int n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");

    // Receiving response:
    bzero(buffer,FILE_SIZE);
    read(sockfd,buffer,255);
    printf("%s", buffer);

    close(sockfd);
}

