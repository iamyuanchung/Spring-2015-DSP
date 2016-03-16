#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

    FILE *fp1 = fopen("testing_answer.txt", "rb"),
         *fp2 = fopen("result1.txt", "rb"); 
    const int data_num = 2500;
    int correct_num = 0;
    for(int i = 0 ; i < data_num ; i++){
        char answer[32],
             my_answer[32],
             my_prob[32];
        fscanf(fp1, "%s", answer);
        fscanf(fp2, "%s%s", my_answer, my_prob);
        if(strcmp(answer, my_answer) == 0)
            correct_num += 1;
    }
    fclose(fp1);
    fclose(fp2);

    FILE *fp = fopen("acc.txt", "wb");
    fprintf(fp, "%lf", (double)correct_num / data_num);
    fclose(fp);

    return 0;
}
