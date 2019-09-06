
//
// Created by antonis on 12/23/17.
//
#include "buffer.h"



char** results;


int finished = 0;
int start = 0;
int end = 0;
Buffer* buffer = NULL;
pthread_mutex_t buffer_mtx;
pthread_mutex_t trie_mtx;
pthread_cond_t cond_start;
pthread_cond_t cond_nonfinished;

Buffer* Buffer_Init(void)
{
    Buffer* new_buffer = malloc(sizeof(Buffer));
    new_buffer->jobs = malloc(STARTING_SIZE*sizeof(Job));
    new_buffer->start = 0;
    new_buffer->end = -1;
    new_buffer->count = 0;
    new_buffer->size = STARTING_SIZE;

    return new_buffer;
}

void Buffer_Destroy()
{
    for(int i = 0 ; i < buffer->end + 1; i++)
    {
        if(buffer->jobs[i].data) free(buffer->jobs[i].data);
    }
    free(buffer->jobs);
    free(buffer);
}

void Buffer_Clean()
{
    for(int i = 0 ; i < buffer->end + 1; i++)
    {
        if(buffer->jobs[i].data) free(buffer->jobs[i].data);
    }
}

int Buffer_isEmpty()
{
    if(buffer->count == 0) return 1;
    return 0;
}

int Buffer_isFull()
{
    if(buffer->count == buffer->size) return 1;
    return 0;
}


void Buffer_Insert_Job(char* job,int line,int version)
{

    buffer->end = (buffer->end+1) % STARTING_SIZE;

    if(job != NULL)
    {
        buffer->jobs[buffer->end].data = strdup(job);

    }
    else buffer->jobs[buffer->end].data = NULL;

    buffer->jobs[buffer->end].current_version = version;

    buffer->jobs[buffer->end].id = line;
    buffer->count++;


}

Job* Buffer_Get_Job()
{
    if(Buffer_isEmpty()) return NULL;
    Job* new_job = &(buffer->jobs[buffer->start]); /** Read new job **/
    buffer->count--;
    buffer->start = (buffer->start+1) % STARTING_SIZE;
    return new_job;
}

void* static_worker(void* arguments)
{
    Worker_Args* args = (Worker_Args*)arguments;


    int temp_id;
    Job* job = NULL;

    while(1)
    {

        pthread_mutex_lock(&buffer_mtx);

        while(start == 0)
        {
            pthread_cond_wait(&cond_start,&buffer_mtx);
        }


        job = Buffer_Get_Job();




        if(job == NULL)
        {
            pthread_cond_signal(&cond_nonfinished);
            pthread_mutex_unlock(&buffer_mtx);
            if(end) break;
            else continue;
        }
        else
        {
            pthread_mutex_unlock(&buffer_mtx); /** Kane unlock wste na diavasoun k alloi */

            if(job->data != NULL) results[job->id] = Search_Static(args->trie,job->data,1,args->result_array);
            else results[job->id] = NULL;

        }

        pthread_mutex_lock(&buffer_mtx);
        finished++;
        pthread_mutex_unlock(&buffer_mtx);
    }


}


void* dynamic_worker(void* arguments)
{
    Worker_Args* args = (Worker_Args*)arguments;


    int temp_id;
    Job* job = NULL;

    while(1)
    {

        pthread_mutex_lock(&buffer_mtx);

        while(start == 0)
        {
            pthread_cond_wait(&cond_start,&buffer_mtx);
        }

        job = Buffer_Get_Job();


        if(job == NULL)
        {
            pthread_cond_signal(&cond_nonfinished);
            pthread_mutex_unlock(&buffer_mtx);
            if(end) break;
            else continue;
        }
        else
        {
            pthread_mutex_unlock(&buffer_mtx); /** Kane unlock wste na diavasoun k alloi */

            if(job->data != NULL) results[job->id] = Search_Ngram(args->trie,job->data,1,args->result_array,job->current_version);
            else results[job->id] = NULL;

        }

        pthread_mutex_lock(&buffer_mtx);
        finished++;
        pthread_mutex_unlock(&buffer_mtx);
    }
    return NULL;
}



int Extract_From_Query_Static(FILE* query_file,Trie* trie)
{
    results = malloc(RESULT_SIZE*sizeof(char*));


    Index** top_array;


    top_array = malloc(THREADS*sizeof(Index*));

    for(int i = 0 ; i < THREADS ; i++)
    {
        top_array[i] = Init_Result_Array();
    }

    int result_size = RESULT_SIZE;
    int number_of_lines = 0;
    pthread_t* alania = malloc(THREADS*sizeof(pthread_t));

    pthread_mutex_init(&buffer_mtx,NULL);

    pthread_cond_init(&cond_nonfinished, 0);
    pthread_cond_init(&cond_start, 0);

    buffer = Buffer_Init();
    char *line = NULL;
    char *job = NULL;
    char *work = NULL;
    char* save;
    int check;
    int k = -1;
    int version = 0;

    size_t len = 0;

    Worker_Args* args = malloc(THREADS*sizeof(Worker_Args));

    for(int i = 0 ; i < THREADS ; i++)
    {
        args[i].trie = trie;
        args[i].thread_id = i;
        args[i].result_array = top_array[i];
        pthread_create(&alania[i],NULL,static_worker,&args[i]);
    }
    check = getline(&line, &len, query_file);
    while(!feof(query_file))
    {
        pthread_mutex_lock(&buffer_mtx);


        line[strcspn(line, "\n")] = 0;

        job = strtok_r(line," ",&save);

        work = strtok_r(NULL,"\n",&save);


        while(finished < number_of_lines)
        {
            pthread_cond_wait(&cond_nonfinished,&buffer_mtx);
        }

        for(int i = 0 ; i < number_of_lines ;i++)
        {
            if(results[i] != NULL) puts(results[i]);
            else printf("-1\n");
        }

        for(int i = 0 ; i < number_of_lines ; i++)
        {
            free(results[i]);
        }
        if(number_of_lines)
        {

            if(k != -1)
            {
                fasoula(top_array,THREADS);

                Print_Top_K(top_array[0],k);

            }

            for(int i = 0 ; i < THREADS ; i++)
            {
                Free_Result_Array(top_array[i]);

            }
        }
        number_of_lines = 0;
        finished = 0;


        while(*job != 'F')
        {

            Buffer_Insert_Job(work,number_of_lines,version);
            number_of_lines++;

            check = getline(&line, &len, query_file);
            line[strcspn(line, "\n")] = 0;
            job = strtok_r(line," ",&save);
            work = strtok_r(NULL,"\n",&save);

        }
        if(work != NULL) k = atoi(work);
        else k = -1;
        start = 1;
        if(number_of_lines > RESULT_SIZE) results = realloc(results,number_of_lines*sizeof(char*));
        pthread_cond_broadcast(&cond_start);
        pthread_mutex_unlock(&buffer_mtx);

        check = getline(&line, &len, query_file);


    }

    end = 1;


    for (int i = 0; i < THREADS; i++)
        pthread_join(alania[i], 0);

    for(int i = 0 ; i < number_of_lines ;i++)
    {
        if(results[i] != NULL) puts(results[i]);
        else printf("-1\n");
    }

    if(k != -1)
    {
        fasoula(top_array,THREADS);

        Print_Top_K(top_array[0],k);

    }


    for(int i = 0 ; i < THREADS ; i++)
    {
        Free_Result_Array(top_array[i]);

    }

    for(int i = 0 ; i < number_of_lines ; i++)
    {
        free(results[i]);
    }
    free(results);

    for(int i = 0 ; i < THREADS ; i++)
    {
        Free_Result_Array(top_array[i]);
        free(top_array[i]->index);
        free(top_array[i]);

    }

    free(top_array);

    free(alania);

    free(line);
    pthread_mutex_destroy(&buffer_mtx);
    pthread_cond_destroy(&cond_start);
    pthread_cond_destroy(&cond_nonfinished);


    Buffer_Destroy();

    free(args);
}



int Extract_From_Query_Dynamic(FILE* query_file,Trie* trie)
{

    results = malloc(RESULT_SIZE*sizeof(char*));

    Index** top_array;
    top_array = malloc(THREADS*sizeof(Index*));
    for(int i = 0 ; i < THREADS ; i++)
    {
        top_array[i] = Init_Result_Array();
    }

    int result_size = RESULT_SIZE;
    int number_of_lines = 0;
    pthread_t* alania = malloc(THREADS*sizeof(pthread_t));

    pthread_mutex_init(&buffer_mtx,NULL);

    pthread_cond_init(&cond_nonfinished, 0);
    pthread_cond_init(&cond_start, 0);

    buffer = Buffer_Init();
    char *line = NULL;
    char *job = NULL;
    char *work = NULL;
    char* save;
    int check;
    int k = -1;
    int type_of_job = 0; // 0---> A|D  1----> Q
    int current_version = 0;

    size_t len = 0;

    Worker_Args* args = malloc(THREADS*sizeof(Worker_Args));

    for(int i = 0 ; i < THREADS ; i++)
    {
        args[i].trie = trie;
        args[i].thread_id = i;
        args[i].result_array = top_array[i];
        pthread_create(&alania[i],NULL,dynamic_worker,&args[i]);
    }
    check = getline(&line, &len, query_file);
    while(!feof(query_file))
    {
        pthread_mutex_lock(&buffer_mtx);

        while(finished < number_of_lines)
        {
            pthread_cond_wait(&cond_nonfinished,&buffer_mtx);
        }

        for(int i = 0 ; i < number_of_lines ;i++)
        {
            if(results[i] != NULL) puts(results[i]);
            else printf("-1\n");
        }

        for(int i = 0 ; i < number_of_lines ; i++)
        {
            if(results[i]) free(results[i]);
        }
        if(number_of_lines)
        {

            if(k != -1)
            {
                fasoula(top_array,THREADS);

                Print_Top_K(top_array[0],k);

            }

            for(int i = 0 ; i < THREADS ; i++)
            {
                Free_Result_Array(top_array[i]);
            }

        }

        number_of_lines = 0;
        finished = 0;


        line[strcspn(line, "\n")] = 0;

        job = strtok_r(line," ",&save);

        work = strtok_r(NULL,"\n",&save);



        while(*job != 'F')
        {
            if(*job == 'A')
            {
                if(type_of_job) current_version++;

                Insert_Ngram(trie,work,current_version);

                type_of_job = 0;
            }
            else if(*job == 'Q')
            {

                Buffer_Insert_Job(work,number_of_lines,current_version);
                number_of_lines++;
                type_of_job = 1;


            }
            else if(*job == 'D')
            {
                if(type_of_job) current_version++;

                Delete_Ngram(trie->root,work,0,current_version);

                type_of_job = 0;
            }



            check = getline(&line, &len, query_file);
            line[strcspn(line, "\n")] = 0;
            job = strtok_r(line," ",&save);
            work = strtok_r(NULL,"\n",&save);

        }
        if(work != NULL) k = atoi(work);
        else k = -1;
        start = 1;
        if(number_of_lines > RESULT_SIZE) results = realloc(results,number_of_lines*sizeof(char*));
        pthread_cond_broadcast(&cond_start);
        pthread_mutex_unlock(&buffer_mtx);

        check = getline(&line, &len, query_file);


    }

    end = 1;


    for (int i = 0; i < THREADS; i++)
        pthread_join(alania[i], 0);

    for(int i = 0 ; i < number_of_lines ;i++)
    {
        if(results[i] != NULL) puts(results[i]);
        else printf("-1\n");
    }

    if(k != -1)
    {
        fasoula(top_array,THREADS);

        Print_Top_K(top_array[0],k);

    }

    for(int i = 0 ; i < number_of_lines ; i++)
    {
         free(results[i]);
    }
    free(results);

    for(int i = 0 ; i < THREADS ; i++)
    {
        Free_Result_Array(top_array[i]);
        free(top_array[i]->index);
        free(top_array[i]);

    }

    free(top_array);

    free(alania);

    free(line);
    pthread_mutex_destroy(&buffer_mtx);
    pthread_cond_destroy(&cond_start);
    pthread_cond_destroy(&cond_nonfinished);


    Buffer_Destroy();

    free(args);




}
