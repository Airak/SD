#include "chord.h"
#include "middleware.h"
using namespace std;

int main(int argc, char const *argv[]) {
    if(argc < 1){
        printf("Usage: %s key", argv[0]);
        exit(1);
    }
    int k = atoi(argv[1]);

    chord_init();

    ifstream file;
    string line;
    int c;

    if(!key_is_mine(k)){
        char *command = (char*) malloc(sizeof(char)*30);
        snprintf(command, sizeof(char)*30, "./find %d", k);
        remote_call((char*) me->peers.ip[0], command);
        exit(0);
    }
    if(!i_have_this_key(k)){
        log(LOG_LEVEL_DANGER, "key %d not found!", k);
        printf("key %d not found!", k);
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
