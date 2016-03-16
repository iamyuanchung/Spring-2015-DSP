all:
	g++ hmm_train.cpp -o train
	g++ hmm_test.cpp -o test
	g++ compute_acc.cpp -o compute_acc
run:
	./train 800 model_init.txt seq_model_01.txt model_01.txt
	./train 800 model_init.txt seq_model_02.txt model_02.txt
	./train 800 model_init.txt seq_model_03.txt model_03.txt
	./train 800 model_init.txt seq_model_04.txt model_04.txt
	./train 800 model_init.txt seq_model_05.txt model_05.txt
	./test modellist.txt testing_data1.txt result1.txt
	./test modellist.txt testing_data2.txt result2.txt
	./compute_acc
clean:
	rm ./train
	rm ./test
