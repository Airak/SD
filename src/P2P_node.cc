#include "ring.h"
#include "middleware.h"
#include <arpa/inet.h>
#include <chrono>
#include <thread>         // std::thread

#define BOOTSTRAPPER_IP ""
#define FILE_TMP "../conf/peers.tmp"

void init_ring_with_bootstrapper();
void ask_ping();
void check_next_peer();
void redo_ring();

int main(int argc, char *argv[])
{
    int sockfd, listener;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int pid;

    init_ring_with_bootstrapper();
    ring_init();
    std::thread ping(ask_ping);

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

    ifstream fin(PEERS_FILE);
    ofstream fout(FILE_TMP);
    string newsuccessor(buffer);

    pos = newsuccessor.find(delimiter);
    int myindex = atoi(newsuccessor.substr(0, pos).c_str());
    line.erase(0, pos + delimiter.length());
    pos = line.find(delimiter);
    int succindex = atoi(newsuccessor.substr(0, pos).c_str());
    strcpy(buffer, newsuccessor.c_str());

    if (!fin.is_open())
        printf("ERRO ao abrir arquivo de pares");

    getline(fin, line);   // Discards first line (commented line)
    fout << line << "\n"; // keeps it in the file

    getline(fin,line);    // first ip is always self.
    pos = line.find(delimiter);
    line.erase(0, pos + delimiter.length());
    fout << myindex << " " << line << "\n"; // keeps ip in the file, changes ID

    getline(fin,line);    // successor's id and ip
    fout << succindex << " " << buffer; // replaces to new successor

    fin.close();
    fout.close();
    rename("example.tmp", "example.txt");

    close(sockfd);
}



void ask_ping(){
    while(1) {

        char buffer[BUFFER_SIZE];
        char buff2[BUFFER_SIZE];

        log(LOG_LEVEL_INFO, "Sending ping to peer %s on port %d...\n", me->peers[0].ip, PORT_NUMBER);

        int sockfd = connectSocket(me->peers[0].ip, PORT_NUMBER);
        if (sockfd == -1)
            error("Could not connect with peer");

        // Sending command to peer:
        strcpy(buffer, "./ping ");
        sprintf(buff2, "%d", ++(me->peers[0].pinged));
        strcat(buffer, buff2);
        int n = write(sockfd,buffer,strlen(buffer));
        if (n < 0)
            error("ERROR writing to socket");

        // Receiving response: pong n+1
        bzero(buffer,FILE_SIZE);
        sprintf(buffer, "%d", me->peers[0].pinged);
        strcpy(buff2, "pong ");
        strcat(buff2, buffer);
        bzero(buffer,FILE_SIZE);
        if(read(sockfd,buffer,255) != 0) {
            // deu ruim, o cara não respondeu. Tem de refazer o anel.
        } else if(strcmp(buff2,buffer)){
            // deu ruim, tem de refazer o anel
        }
        else { // Ok
            me->peers[0].pinged_time = time(0);
        }

        close(sockfd);
        std::chrono::seconds sec(2);
        std::this_thread::sleep_for(sec);
    }
}