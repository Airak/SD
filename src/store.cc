#include "chord.h"
#include "middleware.h"
using namespace std;

int main(int argc, char const *argv[]) {
    if(argc < 2){
        printf("Usage: %s key value", argv[0]);
        exit(1);
    }
    int k = atoi(argv[1]);
    char *value = (char*) argv[2];

    chord_init();

    ofstream file, keys_file;
    string line;
    int c;

    if(!key_is_mine(k)){
        char *command = (char*) malloc(sizeof(char)*30);
        snprintf(command, sizeof(char)*30, "./store %d %s", k, value);
        remote_call((char*) me->peers.ip[0], command);
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
