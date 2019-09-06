//
// Created by antonis on 12/23/17.
//
#ifndef __BUF__
#define __BUF__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "trie.h"


#define STARTING_SIZE 512
#define RESULT_SIZE 512
#define THREADS 8

typedef struct Worker_Args
{
    Trie* trie;
    int thread_id;
    Index* result_array;
}Worker_Args;



typedef struct Job
{
    char* data;
    int id;
    int current_version;
}Job;
typedef struct Buffer
{
	Job* jobs;
	int start;
	int end;
	int count;
	int size;
}Buffer;



Buffer* Buffer_Init(void);
int Buffer_isEmpty();
int Buffer_isFull();
void Buffer_Insert_Job(char* job,int line,int version);
Job* Buffer_Get_Job();

void* static_worker(void* args);
int Extract_From_Query_Static(FILE* query_file,struct Trie*);
int Extract_From_Query_Dynamic(FILE* query_file,Trie* trie);


#endif