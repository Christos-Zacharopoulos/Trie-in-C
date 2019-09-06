//
// Created by tentas on 10/11/17.
//

#ifndef PROJECT_TRIE_HEADER_H
#define PROJECT_TRIE_HEADER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "top.h"
#include "Linear_Hashing.h"
#include "bloom.h"


#define SIZE 2
#define STRING_TABLE_SIZE 6

typedef struct Trie_Node
{
    int fere;					//labeling
    char* word;					//strings saved in this node
    int is_final;				//is node final? boolean
    struct Trie_Node*  children; 		/*Array of pointers to children*/
    signed short int* string_table;		// table used to store each word's size and status as final
    struct Hash_Table* hash_table;
    int string_table_size;			// size of string table
    int size; 					//How many children this node has
    int add_version;
    int delete_version;
    int became_final;
    int became_non_final;
    int active;
} Trie_Node;


typedef struct Trie
{
    Trie_Node*  root;
    int depth;
}Trie;


typedef struct Delete_Pair {
    Trie_Node *father;
    Trie_Node *child;
    int position;
}Delete_Pair;


Trie_Node* New_Node(char* word,int is_final);

Trie* Init_Trie(void);

//Trie_Node* New_Node(char* word,char is_final);
void Insert_Ngram(Trie* trie,char* ngram,int);
char* Search_Ngram(Trie* trie,char* ngram,int count, Index* result_array,int version);
void Search_Substream(Trie_Node* root,char* ngram, char** result,int , short int*, Index*,char**,int version);
int binary_search(Trie_Node* current_node,int min,int max,char* word);
int Delete_Ngram(Trie_Node* current_node,char* ngram,int hash_count,int version);
void Print_Trie(Trie_Node root);
int first_depth_first(char* string,int depth);
int my_strcmp ( char * s1, char * s2);


void Insert_Static(Trie* trie, char* ngram);
void compress(Trie* trie);
void compress_bucket(Trie_Node* current);
void pull( Trie_Node* top_node);
char* Search_Static(Trie* trie,char* ngram,int count,Index* );
void Static_Substream(Trie_Node* root,char* ngram, char** result, char** on_going_ngram, short int*,Index* ,int count);
int Static_binary_search(Trie_Node* current_node,int min,int max,char* word);
int my_strncmp (char * s1,char * s2, int size);
int my_strlen(char *s);

void Destroy_Trie(Trie* trie);
void Destroy_bucket(Trie_Node* current);

#endif //PROJECT_TRIE_HEADER_H
