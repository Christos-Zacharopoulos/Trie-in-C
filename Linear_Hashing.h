//
// Created by antonis on 11/14/17.
//

#ifndef _LINEAR_HASHING_H
#define _LINEAR_HASHING_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "trie.h"


typedef struct Hash_Node
{
    int size; /* Posa stoixeia xwraei */
    int elem_count; /* Posa stoixeia exei mesa */
    struct Trie_Node* my_bucket;
}Hash_Node;


typedef struct Hash_Table
{
    Hash_Node* hash_nodes;
}Hash_Table;


Hash_Table* Hash_Table_Init(void);
Hash_Node Hash_Node_Init(void);
void Hash_Node_Destroy(Hash_Node* hash_node);
void Hash_Table_Destroy(Hash_Table* bucket);


struct Trie_Node* Hash_Table_Insert(Hash_Table* hash_table,char* new_element,int is_final);
struct Trie_Node* Bucket_Insert(struct Trie_Node* bucket,int* bucket_count,struct Trie_Node* new_node);
void split(Hash_Table* hash_table);

struct Trie_Node* Hash_Table_Search(Hash_Table* hash_table,char* element,int mode);
int Hash_Table_Delete(Hash_Table* hash_table,char* element);
void Print_HT(Hash_Table* hash_table);


#endif
