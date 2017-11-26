all: concurrency3_1 concurrency3_2

concurrency3_1: concurrency3_1.c
	gcc -std=c99 -pthread -o concurrency3_1 concurrency3_1.c mt19937ar.c 

concurrency3_2: concurrency3_2.c
	gcc -std=c99 -pthread -o concurrency3_2 concurrency3_2.c mt19937ar.c 

clean:
	rm -f concurrency3_1 concurrency3_2

