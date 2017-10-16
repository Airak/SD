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

// M is no. of nodes and N is no. of peers per ID
#define M 16
#define N 1

struct finger_table
{   // succ_ip_address[i], and IPS from nodes whose ID is (my_id + 2^i) mod M
    unsigned long index[N];
    char ip[N][IP_LEN];
};

struct node
{
    int index;
    char self_ip[IP_LEN];
    //time_t time;                /* time of last message received */
    //time_t reply_time;          /* time of last correct reply received */
    //time_t pinged_time;         /* time of last request */
    //int pinged;                 /* how many requests we sent since last reply */
    finger_table peers;
};

struct node *me = new node();




void chord_init(){
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
        me->peers.index[i] = atoi(line.substr(0, pos).c_str());
        line.erase(0, pos + delimiter.length());
        strcpy(me->peers.ip[i], line.c_str());
        //printf("\n\ni=%d, key=%d, copying %s\n\n", i, me->peers.index[i], line.c_str());
        i++;
    }
}



bool key_is_mine(int k){
    return k==me->index;
}


#endif
