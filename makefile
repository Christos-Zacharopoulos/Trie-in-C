all:
	gcc -O3 -g3 -o ngrams *.c -lpthread
clean:
	rm *.o
