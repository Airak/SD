#include "middleware.h"
using namespace std;

#define NEXT "127.0.0.1"


int main(int argc, char const *argv[]) {
    int k = atoi(argv[1]);

    ifstream file;
    string line;
    int c;
    if(!key_is_mine(k)){
        char *command = (char*) malloc(sizeof(char)*15);
        sprintf(command, "./find %d", k);
        remote_call((char*) NEXT, command);
        exit(0);
    }
    if(!i_have_this_key(k)){
        printf("key not found!");
        exit(0);
    }

    char filename[15];
    sprintf(filename, "%s%d.txt", DATA_FOLDER,k);
    file.open(filename, ifstream::in);
    if (!file.is_open())
        error("Failure opening file");

	while (!file.eof()) {
		getline(file,line);
        printf("%s\n", line.c_str());
    }

    return 0;
}
