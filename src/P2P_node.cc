#include "middleware.h"
#include <arpa/inet.h>

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
