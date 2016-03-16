#include <iostream>
#include <vector>
#include <set>
#include <map>

#include "Ngram.h"

using namespace std;


double getBigramProb(const char *w1, const char *w2);	// from TA
double getTrigramProb(const char *w1, const char *w2, const char *w3);	// from TA
void ReadMappingFile(FILE *fpmap, map<string, set<string> > &mapping);
vector<string> HandleInputLine(char *tmpstr);
vector< set<string> > FindAllPossible(vector<string> InputLine, map<string, set<string> > mapping);
vector<string> Viterbi(vector< set<string> > AllNodes, Ngram &lm, Vocab &vocab);
