#include "middleware.h"
using namespace std;

#define NEXT "127.0.0.1"


int main(int argc, char const *argv[]) {
    int k = atoi(argv[1]);
    char *value = (char*) argv[2];

    ofstream file, keys_file;
    string line;
    int c;

    if(!key_is_mine(k)){
        char *command = (char*) malloc(sizeof(char)*15);
        sprintf(command, "./store %d %s", k, value);
        remote_call((char*) NEXT, command);
        exit(0);
    }
    if(i_have_this_key(k)){
        printf("Register alredy exists!");
        exit(0);
    }

    char filename[15];
    sprintf(filename, "%s%d.txt", DATA_FOLDER,k);
    file.open(filename, ofstream::out | ofstream::app);
    if (!file.is_open())
        error("Failure opening file");
    file << value;
    file.close();

    keys_file.open(KEYS_FILE, ofstream::app);
    if (!keys_file.is_open())
        error("Failure opening file");
    keys_file << k << endl;
    keys_file.close();

    printf("File %d stored successfully", k);

    return 0;
}
