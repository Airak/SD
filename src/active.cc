#include "middleware.h"
#include "chord.h"

#define LOG_COMMAND "cat ../log/log.txt"

int main(string args[]){
    clock_t start, diff;
    //error("SORRY, NOT YET IMPLEMENTED!!!");

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
        start = clock();
        char buffer[FILE_SIZE];
        printf("%s\n\n\t\t\t---------- %s's log: ----------\n%s", BOLD, line , NO_COLOR);
        remote_call((char*) line.c_str(), LOG_COMMAND);
        printf("\n");
        diff = clock() - start;
        printf("%s\n\t\t\t---------- Connection info ----------\n%s", BOLD, NO_COLOR);
        int msec = diff * 1000 / CLOCKS_PER_SEC;
        printf("Time taken for message exchange: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);
    }
    fp.close();
}
