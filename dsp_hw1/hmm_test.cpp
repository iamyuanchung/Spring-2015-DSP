#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "hmm.h"

#define MAX_FILE_NAME_LENGTH 64

int main(int argc, char *argv[]){

    char model_list[MAX_FILE_NAME_LENGTH + 1];
    char test_set[MAX_FILE_NAME_LENGTH + 1];
    char dump_file[MAX_FILE_NAME_LENGTH + 1];
    strcpy(model_list, argv[1]);    // modellist.txt
    strcpy(test_set, argv[2]);      // testing_data1.txt, testing_data2.txt
    strcpy(dump_file, argv[3]);     // result1.txt, result2.txt

	HMM hmms[5];
	load_models(model_list, hmms, 5);

    const int seq_len = 50;     // length of each sequence (testing sample)
    const int data_num = 2500;  // number of testing sequences (testing samples)
    const int state_num = 6;    // == hmm -> state_num
    const int observ_num = 6;   // == hmm -> observ_num

    int seq[data_num][seq_len];

    FILE *fp = fopen(test_set, "rb");
    for(int i = 0 ; i < data_num ; i++){
        char seq_str[seq_len];
        fscanf(fp, "%s", seq_str);
        for(int j = 0 ; j < seq_len ; j++)
            seq[i][j] = seq_str[j] - 65;
    }
    fclose(fp);

    fp = fopen(dump_file, "wb");
    for(int n = 0 ; n < data_num ; n++){
        // for each example,
        double max_P_all = 0.;
        int max_P_model = -1;
        for(int m = 0 ; m < 5 ; m++){
            // go through 5 models
            // ========== Viterbi Algorithm ==========
            double delta[state_num][seq_len] = {0.};
            // initialize delta
            for(int i = 0 ; i < state_num ; i++)
                delta[i][0] = hmms[m].initial[i] * hmms[m].observation[seq[n][0]][i];
            // recursion
            for(int t = 1 ; t < seq_len ; t++){
                for(int j = 0 ; j < state_num ; j++){
                    for(int i = 0 ; i < state_num ; i++){
                        double tmp = delta[i][t - 1] * hmms[m].transition[i][j];
                        if (tmp > delta[j][t])
                            delta[j][t] = tmp;
                    }
                    delta[j][t] *= hmms[m].observation[seq[n][t]][j];
                }
            }
            // termination
            double max_P = 0.;
            for(int i = 0 ; i < state_num ; i++)
                if(delta[i][seq_len - 1] > max_P)
                    max_P = delta[i][seq_len - 1];

            if(max_P > max_P_all){
                max_P_all = max_P;
                max_P_model = m;
            }
        }
        // print the result to the specified file
        fprintf(fp, "model_0%d.txt %e\n", max_P_model + 1, max_P_all);
    }
    fclose(fp);

	return 0;
}
