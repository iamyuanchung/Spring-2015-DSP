g++ hmm_train.c -o train
./train 155 model_init.txt seq_model_01.txt model_01.txt
./train 155 model_init.txt seq_model_02.txt model_02.txt
./train 155 model_init.txt seq_model_03.txt model_03.txt
./train 155 model_init.txt seq_model_04.txt model_04.txt
./train 155 model_init.txt seq_model_05.txt model_05.txt

g++ hmm_test.c -o test
./test modellist.txt testing_data1.txt result1.txt

g++ compute_acc.c -o compute_acc
./compute_acc

cat acc.txt
