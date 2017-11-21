#include "ring.h"
#include "middleware.h"
#include <string.h>

#define LOG_COMMAND "cat ../log/log.txt"

void log(){
    clock_t start, diff;

    std::ifstream fp;
    std::string line, buffer;
    size_t len;
    ssize_t read;
    int server_number_int = 0;

    fp.open(PEERS_FILE);
    if (!fp.is_open())
        error("ERRO ao abrir arquivo de pares");

    getline(fp, line); // Discards first line (commented line)
    while (!fp.eof())  {
        getline(fp,line); // removing line break
        if(fp.eof() || line.empty())
            continue;
        start = clock();
        printf("%s\n\n\t\t\t---------- %s's log: ----------\n%s", BOLD, line.c_str() , NO_COLOR);
        buffer = LOG_COMMAND;
        remote_call((char*)line.c_str(), (char*)buffer.c_str());
        printf("\n");
        diff = clock() - start;
        printf("%s\n\t\t\t---------- Connection info ----------\n%s", BOLD, NO_COLOR);
        int msec = diff * 1000 / CLOCKS_PER_SEC;
        printf("Time taken for message exchange: %d seconds %d milliseconds\n\n", msec/1000, msec%1000);
    }
    fp.close();
}

// ESSE ARQUIVO SERÁ ÚTIL PARA COLOCAR AS CHAMADAS FEITAS PELO TECLADO
int main(int argc, char const *argv[]) {
    log();
    return 0;
}
