#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include "Ngram.h"
#include "tool.h"

#define BUFSIZE 4096

using namespace std;


void Print(vector<string> line){
    cout << "<s>";
    for(int i = 0 ; i < line.size() ; i++){
       cout << " " << line.at(i);
    }
    cout << " </s>" << endl;
}


int main(int argc, char *argv[]){

    map<string, set<string> > mapping;
    Vocab vocab;
    Ngram lm(vocab, 2); 
    if(argc != 5){
        cout << "./disambig -lm [language model] -m [mapping file]" << endl;
        return 0;
    }
    for(int i = 0 ; i < 5 ; i++){
        if(string("-lm") == argv[i]){
            File frLm(argv[i + 1], "r"); 
            lm.read(frLm);
            frLm.close();
        }
        if(string("-m")==argv[i]){
            FILE *fpMap = fopen(argv[i + 1], "r");
             ReadMappingFile(fpMap, mapping);
        }
	}

    char tmpstr[BUFSIZE];
    int k;
    while((k = scanf("%[^\n]%*c", tmpstr)) != EOF){
        vector<string> InputForLine = HandleInputLine(tmpstr);
        vector< set<string> > AllNodes = FindAllPossible(InputForLine, mapping);
        vector<string> BestLine = Viterbi(AllNodes, lm, vocab);
        Print(BestLine);
    }
}
