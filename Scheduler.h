//
// Created by antonis on 12/23/17.
//
#ifndef __SCH__
#define  __SCH__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"


typedef struct Job_Scheduler
{
    int number_of_threads;
    int total_jobs; // per batch
    int finished_jobs;
    int start;
    int end;
    Buffer* buffer;
    pthread_t* alania;

    pthread_mutex_t buffer_mtx;
    pthread_mutex_t trie_mtx;
    pthread_cond_t cond_start;
    pthread_cond_t cond_nonfinished;

}Job_Scheduler;

Job_Scheduler* Job_Scheduler_Init(int n)
{
    Job_Scheduler* job_scheduler = malloc(sizeof(Job_Scheduler));
    job_scheduler->start = 0;
    job_scheduler->end = 0;
    job_scheduler->finished_jobs = 0;
    job_scheduler->total_jobs = 0;

    job_scheduler->number_of_threads = n;

    job_scheduler->alania = malloc(n*sizeof(pthread_t));


    pthread_mutex_init(&job_scheduler->buffer_mtx,NULL);
    pthread_cond_init(&job_scheduler->cond_nonfinished, 0);
    pthread_cond_init(&job_scheduler->cond_start, 0);

    job_scheduler->buffer = Buffer_Init();

    return job_scheduler;

}

void Job_Scheduler_Submit_Job(Job_Scheduler* job_scheduler,Job* new_job)
{
    pthread_mutex_lock(&(job_scheduler->buffer_mtx));

    while(job_scheduler->finished_jobs < job_scheduler->total_jobs)
    {
        pthread_cond_wait(&(job_scheduler->cond_nonfinished),&(job_scheduler->buffer_mtx));
    }

    pthread_mutex_unlock(&(job_scheduler->buffer_mtx));



}



#endif