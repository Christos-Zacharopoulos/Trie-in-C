//
// Created by tentas on 11/16/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "top.h"


int Top_binary_search(Index* result_array,int min,int max,char* word)
{
    int str_result;
    int mid = 0;
    while(max >= min)
    {
        mid = (max+min)/2;
        str_result = strcmp(word,result_array->index[mid].word);
        if(str_result == 0) return mid;
        else if(str_result > 0)
        {
            min = mid + 1;
        }
        else
        {
            max = mid - 1;
        }
    }
    return -1;

}

int Top_Binary_Insert(Index* result_array,int min,int max,char* word)
{
    int left,right,mid;

    left = 0;

    right = max;
    int str_result;
    while(left < right)
    {
        mid = (left+right)/2;
        str_result = strcmp(word,result_array->index[mid].word);

        if(str_result == 0)
        {
            //result_array->index[mid].word = strdup(word);
            result_array->index[mid].counter++;
            return mid;
        }
        if(str_result > 0) right = mid;
        else left = mid + 1;

    }

    if(((result_array->size)%100 == 0 ) && (result_array->size!=0))
    {
        result_array->index = realloc(result_array->index, (2*(result_array->size))* sizeof(Cell));
    }
    if(left < result_array->size)
    {
        memmove((&result_array->index[left+1]),(&result_array->index[left]),(result_array->size-left)* sizeof(Cell));
    }
    result_array->index[left].word = strdup(word);
    result_array->index[left].counter = 1;
    (result_array->size)++;

    return left;
}

int Top_Binary_Merge(Index* result_array,int min,int max,Cell* new_cell)
{
    int left,right,mid;

    left = 0;

    right = max;
    int str_result;
    while(left < right)
    {
        mid = (left+right)/2;
        str_result = strcmp(new_cell->word,result_array->index[mid].word);

        if(str_result == 0)
        {
            result_array->index[mid].counter += new_cell->counter;
            return mid;
        }
        if(str_result > 0) right = mid;
        else left = mid + 1;

    }

    if(((result_array->size)%100 == 0 ) && (result_array->size!=0))
    {
        result_array->index = realloc(result_array->index, 2*result_array->size*sizeof(Cell));
    }
    if(left != result_array->size)
    {
        memmove((&result_array->index[left+1]),(&result_array->index[left]),(result_array->size-left)* sizeof(Cell));
    }
    result_array->index[left].word = strdup(new_cell->word);
    result_array->index[left].counter = new_cell->counter ;
    (result_array->size)++;
}




Index* Init_Result_Array()
{
    Index* result_array = malloc(sizeof(Index));
    result_array->size = 0;

    result_array->index = malloc(100*sizeof(Cell));
    int i = 0;

    for(i = 0; i < 100; i ++){
        result_array->index[i].word = NULL;
        result_array->index[i].counter = 0;
    }

    return result_array;
}

void Free_Result_Array(Index* result_array)
{
    int i = 0;
    for(i = 0; i < result_array->size; i++)
    {
        free(result_array->index[i].word);
        result_array->index[i].counter = 0;
    }

    result_array->size = 0;
}


void Insert_Result_Array(Index *result_array,char* n_gram)
{
    int i = 0, found = 0, move = 0,position = -1;

    i = Top_Binary_Insert(result_array,0,result_array->size,n_gram);
}

void Print_Top_K(Index* result_array, int k)
{
    int top[k], i = 0, j = 0, top_iter = 0;
    Cell temp;
    
    for (i = 0; i < result_array->size && i < k; i++)
    {
        top_iter = i;
        for (j = i; j < result_array->size; j++)
        {
            if (result_array->index[j].counter > result_array->index[top_iter].counter)
            {
                top_iter = j;
            }
            else if(result_array->index[j].counter == result_array->index[top_iter].counter)
            {
                if(strcmp(result_array->index[j].word,result_array->index[top_iter].word) < 0)
                {
                    top_iter = j;
                }
            }
        }
        if(top_iter != i)
        {
            memcpy(&temp, (&result_array->index[top_iter]), sizeof(Cell));
            memcpy((&result_array->index[top_iter]), (&result_array->index[i]), sizeof(Cell));
            memcpy((&result_array->index[i]), &temp, sizeof(Cell));
        }
    }

    i = 0;
    if(result_array->size) printf("Top: ");
    while(i < k && i < result_array->size)
    {
        printf("%s",result_array->index[i].word);
        if((i != result_array->size - 1) && (i != k - 1)) printf("|");
        i++;
    }

    if(result_array->size) printf("\n");
}

int fasoula(Index** top_arrays,int number_of_arrays)
{
    int position;
    for(int k = 1 ; k < number_of_arrays ; k++)
    {
        for(int i = 0 ; i < top_arrays[k]->size; i++) /** Diatrexw 2o pinaka **/
        {
            Top_Binary_Merge(top_arrays[0],0,top_arrays[0]->size,&(top_arrays[k]->index[i]));
        }
    }


}
