#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hmm.h"

#define MAX_FILE_NAME_LENGTH 64

void train_model(HMM *hmm, char *train_set, int iteration){

    const int seq_len = 50;     // length of each sequence (sample)
    const int data_num = 10000; // number of sequences (samples)
    const int state_num = 6;    // == hmm -> state_num
    const int observ_num = 6;   // == hmm -> observ_num

    int seq[data_num][seq_len]; // array of samples

    FILE *fp = fopen(train_set, "rb");
    for(int i = 0 ; i < data_num ; i++){
        char seq_str[seq_len];
        fscanf(fp, "%s", seq_str);
        for(int j = 0 ; j < seq_len ; j++)
            seq[i][j] = seq_str[j] - 'A';
    }
    fclose(fp);

    for(int iter = 0 ; iter < iteration ; iter++){

        printf("iter #%d\n", iter + 1);

        double gamma_all[state_num][seq_len] = {0};     // keep in mind that you made some changes here !
        double epsilon_all[state_num][state_num] = {0};
        double observ_update[observ_num][state_num] = {0};

        for(int n = 0 ; n < data_num ; n++){
            // go through all samples and accumulate gamma and epsilon
            // ========== Forward Algorithm (compute alpha) ==========
            double alpha[state_num][seq_len] = {0.};
            // alpha initialization
            for(int i = 0 ; i < state_num ; i++)
                alpha[i][0] = hmm -> initial[i] * hmm -> observation[seq[n][0]][i];
            // alpha induction
            for(int t = 1 ; t < seq_len ; t++){
                for(int j = 0 ; j < state_num ; j++){
                    for(int i = 0 ; i < state_num ; i++)
                        alpha[j][t] += alpha[i][t - 1] * hmm -> transition[i][j];
                    alpha[j][t] *= hmm -> observation[seq[n][t]][j];
                }
            }

            // ========== Backward Algorithm (compute beta) ==========
            double beta[state_num][seq_len] = {0.};
            // beta initialization
            for(int i = 0 ; i < state_num ; i++)
                beta[i][seq_len - 1] = 1.;
            // beta induction
            for(int t = seq_len - 2 ; t >= 0 ; t--)
                for(int i = 0 ; i < state_num ; i++)
                    for(int j = 0 ; j < state_num ; j++)
                        beta[i][t] += hmm -> transition[i][j] * hmm -> observation[seq[n][t + 1]][j] * beta[j][t + 1];

            // calculate gamma
            double gamma[state_num][seq_len] = {0.};
            for(int t = 0 ; t < seq_len ; t++){
                double sum = 0.;
                for(int i = 0 ; i < state_num ; i++){
                    gamma[i][t] = alpha[i][t] * beta[i][t];
                    sum += gamma[i][t];
                }
                for(int i = 0 ; i < state_num ; i++)
                    gamma[i][t] /= sum;
            }

            // accumulate observ_update for update later ...
            for(int t = 0 ; t < seq_len ; t++)
                for(int i = 0 ; i < state_num ; i++)
                    observ_update[seq[n][t]][i] += gamma[i][t];

            // calculate epsilon
            double epsilon[seq_len - 1][state_num][state_num] = {0.};
            for(int t = 0 ; t < seq_len - 1 ; t++){
                double sum = 0.;
                for(int i = 0 ; i < state_num ; i++){
                    for(int j = 0 ; j < state_num ; j++){
                        epsilon[t][i][j] = alpha[i][t] * hmm -> transition[i][j] * hmm -> observation[seq[n][t + 1]][j] * beta[j][t + 1];
                        sum += epsilon[t][i][j];
                    }
                }
                for(int i = 0 ; i < state_num ; i++)
                    for(int j = 0 ; j < state_num ; j++)
                        epsilon[t][i][j] /= sum;
            }

            // accumulate gamma and epsilon (TA's slide p.17)
            // TA's warning: Accumulate epsilon and gamma through all samples,
            // not just all observations in one sample !
            // accumulate gamma
            for(int i = 0 ; i < state_num ; i++)
                for(int j = 0 ; j < seq_len ; j++)
                    gamma_all[i][j] += gamma[i][j];

            // accumulate epsilon
            for(int t = 0 ; t < seq_len - 1 ; t++)
                for(int i = 0 ; i < state_num ; i++)
                    for(int j = 0 ; j < state_num ; j++)
                        epsilon_all[i][j] += epsilon[t][i][j];
        }

        // re-estimate model parameters (FAQ.10)
        // re-estimate hmm -> initial
        for(int i = 0 ; i < state_num ; i++)
            hmm -> initial[i] = gamma_all[i][0] / data_num;
        // re-estimate hmm -> transition
        double gamma_vector[state_num] = {0};
        for(int t = 0 ; t < seq_len - 1 ; t++)
            for(int i = 0 ; i < state_num ; i++)
                gamma_vector[i] += gamma_all[i][t];
        for(int i = 0 ; i < state_num ; i++)
            for(int j = 0 ; j < state_num ; j++)
                hmm -> transition[i][j] = epsilon_all[i][j] / gamma_vector[i];
        // re-estimate hmm -> observation
        for(int i = 0 ; i < state_num ; i++)
            gamma_vector[i] += gamma_all[i][seq_len - 1];
        for(int j = 0 ; j < state_num ; j++)
            for(int k = 0 ; k < observ_num ; k++)
                hmm -> observation[k][j] = observ_update[k][j] / gamma_vector[j];
    }
}

int main(int argc, char *argv[]){

    int iteration = atoi(argv[1]);               // $iter
    char init_file[MAX_FILE_NAME_LENGTH + 1],    // model_init.txt
         train_file[MAX_FILE_NAME_LENGTH + 1],   // seq_model_01~05.txt
         dump_file[MAX_FILE_NAME_LENGTH + 1];    // model_01~05.txt
    strcpy(init_file, argv[2]);
    strcpy(train_file, argv[3]);
    strcpy(dump_file, argv[4]);

    HMM hmm;
    // initialize HMM for further update
    loadHMM(&hmm, init_file);
    // train HMM with trainging data (seq_model_01~05.txt)
    train_model(&hmm, train_file, iteration);
    // dump the training result (model parameters)
    FILE *fp = fopen(dump_file, "wb");
    dumpHMM(fp, &hmm);
    fclose(fp);

    return 0;
}
