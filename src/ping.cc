#include "ring.h"
#include "middleware.h"
using namespace std;

int main(int argc, char const *argv[]) {
    if (argc < 1) {
        printf("Usage: %s key", argv[0]);
        exit(1);
    }
    int k = atoi(argv[1]);
    k = k+1;
    printf("pong %d", k);
}