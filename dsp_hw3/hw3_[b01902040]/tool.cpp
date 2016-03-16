#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <stdio.h>

#include "Ngram.h"

using namespace std;


double getBigramProb(Vocab &voc, Ngram &lm, const char *w1, const char *w2){

    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if(wid1 == Vocab_None)  // OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  // OOV
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

void ReadMappingFile(FILE *fpmap, map<string, set<string> > &mapping){

    int pos, i;
    char tmpword[100];
    string zuyin, token, word;
    vector<string> result;
    while((i = fscanf(fpmap, "%s", tmpword)) != EOF){
        pos = 0;
        word = string(tmpword);
        fscanf(fpmap, "%s", tmpword);
        zuyin = string(tmpword);
        while(pos != -1){
            pos = zuyin.find("/");
            token = zuyin.substr(0, pos);
            string tmp = token.substr(0, 2);
            mapping[tmp].insert(word);
            zuyin = zuyin.substr(pos + 1);
        }
    }
}

vector<string> HandleInputLine(char *Line){

    vector<string> splitLine;
    string buf;
    string tmpLine = string(Line);
    int pos;
    while((pos = tmpLine.find(" ")) != -1){
       tmpLine.erase(pos, 1);
    }
    assert(tmpLine.size() % 2 == 0);
    for(int i = 0 ; i < tmpLine.size() ; i += 2){
       buf = tmpLine.substr(i, 2);
       splitLine.push_back(buf);
    }
    return splitLine;
}

vector< set<string> > FindAllPossible(vector<string> InputLine, map<string, set<string> > mapping){
    vector< set<string> > AllNodes;
    set<string> TmpSet;
    map<string, set<string> >::iterator map_iter;
    for(int i = 0 ; i < InputLine.size() ; i++){
        set<string> TmpSet;
        map_iter = mapping.find(InputLine.at(i));
        if(map_iter != mapping.end()){
            TmpSet = (map_iter->second);
        }
	    else{
            TmpSet.insert(InputLine.at(i));
        }
        AllNodes.push_back(TmpSet);
    }
    return AllNodes;
}

pair<vector<string>, double> FindBestBigram(vector< pair<vector<string>, double> > Pathes, string CurrentWord, Ngram &lm, Vocab &vocab){

    vector<string>::iterator iter;
    int MaxIndex = 0, i;
    double Prob, MaxProb = -1000.0;
    for(i = 0 ; i < Pathes.size() ; i++){
        iter = Pathes.at(i).first.end();
        iter -= 1;
        string PreWord = *(iter);
        double oldProb = Pathes.at(i).second;
        double newProb = getBigramProb(vocab, lm, PreWord.c_str(), CurrentWord.c_str());
        if((Prob = oldProb + newProb) > MaxProb){
            MaxProb = Prob;
            MaxIndex = i;
        }
    }
    assert(MaxIndex<Pathes.size());
    pair<vector<string>, double> BestPathes = Pathes.at(MaxIndex);
    BestPathes.first.push_back(CurrentWord);
    BestPathes.second = MaxProb;
    return BestPathes;
}

/*Nothing but debug*/
set<string> CopySet(set<string> set1){
    set<string> copyset;
    set<string>::iterator it;
    for(it = set1.begin() ; it != set1.end() ; it++){
        copyset.insert((*it));
    }
    return copyset;
}

vector<string> Viterbi(vector< set<string> > AllNodes, Ngram &lm, Vocab &vocab){

    vector< pair<vector<string>, double> > Pathes;
    set<string>::iterator node_iter = AllNodes.begin()->begin();

    for(node_iter ; node_iter != AllNodes.begin() -> end() ; node_iter++){
        vector<string> StartSym;
        StartSym.push_back((*node_iter));
        Pathes.push_back(make_pair(StartSym, 0.0));
    }

    for(int i = 1 ; i < AllNodes.size() ; i++){
        vector< pair<vector<string>, double> > NewPathes;
        set<string> PsbWords = CopySet(AllNodes.at(i));
        for(node_iter = PsbWords.begin() ; node_iter != PsbWords.end() ; node_iter++){
            string CurrentWord = (*node_iter);
            pair<vector<string>, double> best = FindBestBigram(Pathes, CurrentWord, lm, vocab);
            NewPathes.push_back(best);
        }
        Pathes.clear();
        Pathes = NewPathes;
    }

    double maxProb = -1000.0;
    int Index = 0, i;
    for(i = 0 ; i < Pathes.size() ; i++){
        if(Pathes.at(i).second > maxProb){
            Index = i;
            maxProb = Pathes.at(i).second;
        }
    }
    return Pathes.at(Index).first;
}
