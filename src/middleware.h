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

#define BUFFER_SIZE 256
#define PORT_NUMBER 4563
#define FILE_SIZE 256

#define DATA_FOLDER "../data/"
#define LOG_FOLDER "../log/"
#define CONF_FOLDER "../conf/"
#define PEERS_FILE "../conf/peers.txt"


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

clock_t start, diff;
char *LOGFILE;



void log(const char * msg, int log_lvl){
    std::ofstream file;
	file.open(LOGFILE,std::ofstream::out);
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
    log(msg, LOG_LEVEL_ERROR);
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
    printf("%s\n\t\t\t---------- Connecting to %s ----------\n%s", BOLD, hostnameOrIp,NO_COLOR);

    // Connecting to server socket:
    printf("Connecting to server %s on port %d...\n", hostnameOrIp, PORT_NUMBER);
    int sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);

    if (sockfd == -1) {
        printf("Could not connect with server");
    }else {
        // Sending command to server:
        strcpy(buffer,"cat ");
        strcat(buffer, LOG_FOLDER);
        strcat(buffer, "maquina.");
        strcat(buffer,server_number);
        strcat(buffer,".log");
        int n = write(sockfd,buffer,strlen(buffer));
        if (n < 0){
             error("ERROR writing to socket");
        } else {
            // Receiving response:
            printf("%s\n\t\t\t---------- %s log: ----------\n%s", BOLD, &buffer[4], NO_COLOR);
            bzero(buffer,FILE_SIZE);
            while (read(sockfd,buffer,255) != 0) {
                printf("%s",buffer);
                bzero(buffer,FILE_SIZE);
            }
            printf("\n");
            close(sockfd);
            diff = clock() - diff;
            printf("%s\n\t\t\t---------- Connection info ----------\n%s", BOLD, NO_COLOR);
            int msec = diff * 1000 / CLOCKS_PER_SEC;
            printf("Time taken for message exchange: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);
        }
    }
}


void request_from_peers(){
    std::ifstream fp;
    std::string line;
    size_t len;
    ssize_t read;
    int server_number_int = 0;
    char server_number_string[256] = "0";

    fp.open(PEERS_FILE);
    if (!fp.is_open())
        error("ERRO ao abrir arquivo de pares");

    getline(fp, line); // Discards first line (commented line)
    while (!fp.eof())  {
        getline(fp,line); // removing line break
        sprintf(server_number_string, "%d", server_number_int++);
        simulate_remote_call((char*) line.c_str(),server_number_string);
    }
    fp.close();
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
