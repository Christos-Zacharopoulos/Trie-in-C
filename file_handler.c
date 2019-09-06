//
// Created by antonis on 10/11/17.
//

#include "file_handler.h"

int Get_Arguments(int argc,char** argv,Arguments* args)
{

    args->init_file = NULL;
    args->query_file = NULL;

    int i;
    if(argc != 5)
    {
        printf("Usage: ./ngrams -i <init_file> -q <query_file>\n");
        return 0;
    }
    for(i = 0 ; i < argc ; i++)
    {
        if(strcmp(argv[i],"-i") == 0) /*If we have found -i argument we take next parameter*/
        {
            args->init_file = fopen(argv[i+1],"r");
            if(args->init_file == NULL) //If init file does not exist we terminate
            {
                perror("Error opening init file");
                return 0;
            }
        }
        else if(strcmp(argv[i],"-q") == 0) /*If we have found -q argument we take next parameter*/
        {
            args->query_file = fopen(argv[i+1],"r");
            if(args->query_file == NULL ) //If query file does not exist
            {
                perror("Error opening query file");
                return 0;
            }

        }
    }

    return 1;
}

int Close_Files(Arguments* args)
{
    fclose(args->query_file);
    fclose(args->init_file);
    free(args);
    return 1;
}

int Extract_From_Init(FILE* init_file, Trie* trie) /*Getting words from init file and adds them to trie*/
{
    char* line = NULL;
    size_t len = 0;

    if(getline(&line,&len,init_file)!=-1) {
        line[strcspn(line, "\n")] = 0;
        if(!strcmp(line,"STATIC")) {
            while(getline(&line,&len,init_file)!=-1) {
                line[strcspn(line, "\n")] = 0;

                Insert_Static(trie,line);
            }
            compress(trie);

            free(line);
            return 1;
        } else {
            while(getline(&line,&len,init_file)!=-1) {
                line[strcspn(line, "\n")] = 0;

                Insert_Ngram(trie,line,0); // version = 0

            }
            free(line);
            return 0;
        }
    }
    return -1;
}



