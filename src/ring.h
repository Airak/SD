/** Inspirado em:
https://github.com/ankitrohilla/chord-dht
https://github.com/orcaman/chord
https://github.com/jech/dht
*/
#ifndef CHORD_H
#define CHORD_H

#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
using namespace std;


#define PEERS_FILE "../conf/peers.txt"
#define IP_LEN 16
#define PORT_LEN 8

// M is no. of nodes and N is no. of peers per ID
int M = 4;
#define N 2

struct finger_table
{   // succ_ip_address[i], and IPS from nodes whose ID is (my_id + 2^i) mod M
    unsigned long index;
    char ip[IP_LEN];
    //char port[PORT_LEN];
    //time_t time;                /* time of last message received */
    //time_t reply_time;          /* time of last correct reply received */
    //time_t pinged_time;         /* time of last request */
    int pinged;                 /* how many requests we sent since last reply */

};

struct node
{
    int index;
    char self_ip[IP_LEN];
    finger_table peers[N];
    //finger_table pings[N];
};

struct node *me = new node();




void ring_init(){
    ifstream fp;
    string line, space = " ";

    fp.open(PEERS_FILE);
    if (!fp.is_open())
        printf("ERRO ao abrir arquivo de pares");

    getline(fp, line); // Discards first line (commented line)
    getline(fp,line); // first ip is always self.

    // Gets self index and ip
    size_t pos = 0;
    string delimiter = " ";
    pos = line.find(delimiter);
    me->index = atoi(line.substr(0, pos).c_str());
    line.erase(0, pos + delimiter.length());
    strcpy(me->self_ip, line.c_str());

    // get peers index and ip
    int i=0;
    while (!fp.eof() && i<N)  {
        getline(fp,line);
        if(fp.eof())
            continue;
        pos = line.find(delimiter);
        me->peers[i].index = atoi(line.substr(0, pos).c_str());
        line.erase(0, pos + delimiter.length());
        strcpy(me->peers[i].ip, line.c_str());
        //printf("\n\ni=%d, key=%d, copying %s\n\n", i, me->peers.index[i], line.c_str());
        i++;
    }
}

int who_should_handle_this_key(int k){
    int last_i = 0;
    for(int i=0; i<N; i++)
        if((k%M)==me->peers[i].index)
            return i;
        else if((k%M) > me->peers[i].index)
            return last_i;
        else last_i = i;
    return last_i;
}

bool key_is_mine(int k){
    return (k%M)==me->index;
}


#endif
