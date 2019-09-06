//
// Created by antonis on 10/11/17.
//

#ifndef PROJECT_ARGUMENTS_H
#define PROJECT_ARGUMENTS_H
#define FILE_ERROR 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "trie.h"

typedef struct Arguments
{
    FILE* init_file;
    FILE* query_file;
}Arguments;

int Get_Arguments(int argc,char** argv,Arguments* args); /*If returns 0 we have an error */

int Close_Files(Arguments*); /*Closing all open files*/
int Extract_From_Init(FILE*, Trie*); /*Getting words from init file and adds them to trie*/

#endif //PROJECT_ARGUMENTS_H
