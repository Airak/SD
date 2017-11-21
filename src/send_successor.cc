//
// Created by aluno on 21/11/17.
//

#define FILE_TMP "../conf/peers.tmp"

#include "ring.h"
using namespace std;


int main(int argc, char const *argv[]) {
    ifstream fin(PEERS_FILE);
    ofstream fout(FILE_TMP);
    string line = "", newsuccessor = " ";

    if(argc < 1){
        printf("Usage: %s id_successor ip_successor", argv[0]);
        exit(1);
    }

    int id_suc = atoi(argv[1]);
    char *ip_suc = (char*) malloc(sizeof(char)*IP_LEN);
    newsuccessor += id_suc;
    newsuccessor += " ";
    newsuccessor.append(ip_suc);

    if (!fin.is_open())
        printf("ERRO ao abrir arquivo de pares");


    getline(fin, line);   // Discards first line (commented line)
    fout << line << "\n"; // keeps it in the file
    getline(fin,line);    // first ip is always self.
    fout << line << "\n"; // keeps it in the file
    getline(fin,line);    // successor's id and ip
    fout << newsuccessor; // replaces to new successor

    fin.close();
    fout.close();
    rename("example.tmp", "example.txt");

    printf("%s", line.c_str());         // sends successor id and ip to bootstrapper.
    return 0;
}