#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <ctime>
#include <fstream>
#include <string>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

using namespace std;

#define BUFFER_SIZE 256
#define PORT_NUMBER 4563
#define FILE_SIZE 256

// Structure of the project
#define DATA_FOLDER "../data/"
#define LOG_FOLDER "../log/"
#define CONF_FOLDER "../conf/"
#define KEYS_FILE "../data/keys.txt"
#define LOG_FILE "../log/log.txt"

// coloring logs:
#define BOLD "\e[01;39m"
#define NO_COLOR "\e[00;39m"
#define RED "\e[31m"
#define GREEN "\e[32m"
#define LOG_MSG_INFO "\e[32m[INFO]:\e[00;39m"
#define LOG_MSG_DANGER "\e[01;39m[DANGER]:\e[00;39m"
#define LOG_MSG_ERROR "\e[01;31m[ERROR]:\e[00;39m"
#define LOG_LEVEL_INFO 0
#define LOG_LEVEL_DANGER 1
#define LOG_LEVEL_ERROR 2
#define PRINT

// Connection and log functions.
void log(int log_lvl, const char * msg);
void error(const char *msg);
int connectSocket(char *hostnameOrIp, int port_number);
void remote_call(char *hostnameOrIp, char *command);
int start_listening(int port);

// DHT functions
bool key_is_mine(int k);
bool i_have_this_key(int k);


void log(int log_lvl, const char * msg){
    #ifdef PRINT
        printf("[LOG] %s %s", ((log_lvl==0)? LOG_MSG_INFO : ((log_lvl==1)? LOG_MSG_DANGER : LOG_MSG_ERROR) ), msg);
    #endif
    ofstream file;
	file.open(LOG_FILE,ofstream::app);
    int i;
	if (file.is_open()) {
        if(log_lvl==0)
            file << LOG_MSG_INFO;
        else if(log_lvl==1)
            file << LOG_MSG_DANGER;
        else
            file << LOG_MSG_ERROR;
        for (i; msg[i]!='\0'; i++){
			file << msg[i];
		}
        file << '\n';
        file.close();
    }
}

void error(const char *msg)
{
    log(LOG_LEVEL_ERROR,msg);
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



void remote_call(char *hostnameOrIp, char *command){
    char buffer[FILE_SIZE];
    //printf("Connecting to server %s on port %d...\n", hostnameOrIp, PORT_NUMBER);

    int sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);
    if (sockfd == -1) {
        error("Could not connect with server");
    }else {
        // Sending command to server:
        int n = write(sockfd,command,strlen(command));
        if (n < 0){
             error("ERROR writing to socket");
        } else {
            // Receiving response:
            bzero(buffer,FILE_SIZE);
            while (read(sockfd,buffer,255) != 0) {
                printf("%s",buffer);
                bzero(buffer,FILE_SIZE);
            }
            printf("\n");
            close(sockfd);
        }
    }
}

#ifndef CHORD_H
    bool key_is_mine(int k){
        return true; // TODO
    }
#endif

bool i_have_this_key(int k){
    ifstream file;
    string line;
    int c;

    file.open(KEYS_FILE, ifstream::in);
    if (!file.is_open())
        error("Failure opening file");
	while (!file.eof()) {
		getline(file,line);
        if (!file.eof()) {
			c = stoi(line.substr(0, line.find(" ")));
            if(c==k) return true;
        }
    }
    return false;
}

#endif
