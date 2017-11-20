#include "chord.h"
#include "middleware.h"
using namespace std;

int main(int argc, char const *argv[]) {

    char *ipdest = (char*) malloc(sizeof(char)*14);
    int portdest = 0;

    if(argc < 1){
        printf("Usage: %s key [ipdest portdest]", argv[0]);
        exit(1);
    } else if(argc==1) {
        ipdest = me->self_ip;
        portdest = me->self_port;
    } if(argc == 2){
        strcpy(ipdest, argv[2]);
        portdest = atoi(argv[3]);
    }

    int k = atoi(argv[1]);

    chord_init();

    ifstream file;
    string line;
    int c;

    if(!key_is_mine(k)){ // && !strcmp(ipdest, me->self_ip)){
        char *command = (char*) malloc(sizeof(char)*50);
        snprintf(command, sizeof(char)*50, "./find %d %s %d", k, ipdest, portdest);
        int next = who_should_handle_this_key(k);
        remote_call((char*) me->peers[next].ip, command);
        exit(0);
    }
    if(!i_have_this_key(k)){
        log(LOG_LEVEL_DANGER, "key %d not found!", k);
        printf("key %d not found!\n", k);
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
