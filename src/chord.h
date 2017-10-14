/** Inspirado em:
https://github.com/ankitrohilla/chord-dht
https://github.com/orcaman/chord
https://github.com/jech/dht
*/

#include "middleware.h"
#include <string.h>

using namespace std;

// M is no. of nodes and N is no. of peers per ID
#define M 16
#define N 4

struct host {
    char ip[16];
    char port[8];
}

struct finger_table
{   // succ_ip_address[i], port[i] contains IPA, port of node whose ID is (my_id + 2^i) mod 16
    unsigned long index[N];
    struct host[N];
};

struct node
{
    int id;

    struct host self;
    //struct host pred;
    struct host succ;

    time_t time;                /* time of last message received */
    time_t reply_time;          /* time of last correct reply received */
    time_t pinged_time;         /* time of last request */
    int pinged;                 /* how many requests we sent since last reply */
    //finger_table peers;
};

struct node *me = new node();


void initHost(struct host *h){
    memset(h->ip, 0, 16);
    memset(h->port, 0, 8);
}

void initNode(struct node *node){
    node->id = 0;
    node->pinged = 0;
    node->self = new host();
    node->succ = new host();
    initHost(node->self);
    initHost(node->succ);
}
