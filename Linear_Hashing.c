//
// Created by antonis on 11/14/17.
//


#include "Linear_Hashing.h"
#include "file_handler.h"
#include "buffer.h"


int bucket_size = 2;
int table_size = 4;

int m = 4;

int split_round = 0;
int split_index = 0;

int main(int argc,char** argv)
{
    Arguments* file_list = malloc(sizeof(Arguments));

    if(Get_Arguments(argc,argv,file_list) == 0) return FILE_ERROR;

    Trie* trie = Init_Trie();

    int init = Extract_From_Init(file_list->init_file, trie);




    if(init == -1) return FILE_ERROR;
    else if(init == 1) {
        if(Extract_From_Query_Static(file_list->query_file,trie) == 0) return FILE_ERROR;
        else {
            //Destroy_Trie(trie);
            Hash_Table_Destroy(trie->root->hash_table);
        }
    } else if(init == 0){
        if(Extract_From_Query_Dynamic(file_list->query_file, trie) == 0) return FILE_ERROR;
        else {
            //Destroy_Trie(trie);
            Hash_Table_Destroy(trie->root->hash_table);
        }
    }
    free(trie->root->children);
    free(trie->root);
    free(trie);


    Close_Files(file_list);





}



/*Searching for word in a bucket*/
int binary_search_bucket(Trie_Node* current_node,int min,int max,char* word)
{
        int mid;
        while(max >= min)
        {
            mid = (max+min)/2;
            int str_result = my_strcmp(word,current_node[mid].word);
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
int static_binary_search_bucket(Trie_Node* current_node,int min,int max,char* word) {
    int mid, str_result, len, i = 0;
    char* temp = NULL;


    while (max >= min) {
        mid = (max + min) / 2;

        len = current_node[mid].string_table[0];

        if (len < 0) len = -len;

        str_result = my_strncmp(word, current_node[mid].word, len);


        if (str_result == 0 && len == my_strlen(word) ) {
            return mid;
        }
        else if(str_result == 0 && len < my_strlen(word))
        {
            min = mid + 1;
        }
        else if (str_result > 0) min = mid + 1;
        else max = mid - 1;
    }
    return -1;

}

unsigned int hash_function (char* word)
{
    unsigned int hash = 0;
    for (int i = 0 ; word[i] != '\0' && word[i] != ' ' ; i++)
    {
        hash = 31*hash + word[i];
    }
    return (hash>=0) ? hash : (-hash);
}


/* x^n */
int power(int x , int n)
{
    if(n == 0) return 1;
    int prod = 1;
    for(int i = 1 ; i <= n ; i++) prod*=x;
    return prod;

}
int hash(char* word , int i)
{
    int mod = power(2,i) * m;
    return  (hash_function(word) % (power(2,i)*m));
}

Trie_Node* New_Node(char* word,int is_final)
{
    Trie_Node* new_node = malloc(sizeof(Trie_Node));/// Creating new node

    memset(new_node,'\0',sizeof(Trie_Node));

    new_node->word = malloc((strlen(word) + 1) * sizeof(char));
    strcpy(new_node->word, word);

    new_node->is_final = is_final;

    new_node->fere = 0;

    new_node->hash_table = NULL;

    new_node->size = 0;
    new_node->active = 0;

    new_node->string_table = malloc(STRING_TABLE_SIZE*sizeof(signed short int));
    memset(new_node->string_table,'\0',STRING_TABLE_SIZE*sizeof(signed short int));

    if(is_final == 1) new_node->string_table[0] = strlen(word);
    else new_node->string_table[0] = -strlen(word);

    new_node->string_table_size = 1;

    new_node->children = malloc(SIZE*sizeof(Trie_Node));

    memset(new_node->children,'\0',SIZE*sizeof(Trie_Node));

    new_node->add_version = -1;
    new_node->delete_version = -1;


    return new_node;
}





Hash_Table* Hash_Table_Init(void)
{
    Hash_Table* hash_table = malloc(sizeof(Hash_Table));

    hash_table->hash_nodes = malloc(table_size*sizeof(Hash_Node));

    for(int i = 0 ; i < table_size ; i++)
    {
        hash_table->hash_nodes[i].my_bucket =  malloc(bucket_size*sizeof(Trie_Node));

        for(int k = 0 ; k < bucket_size ; k++)
        {
            memset(&(hash_table->hash_nodes[i].my_bucket[k]),'\0',sizeof(Trie_Node));
        }
        hash_table->hash_nodes[i].size = bucket_size;
        hash_table->hash_nodes[i].elem_count = 0;
    }

    return hash_table;

}

void Hash_Table_Destroy(Hash_Table* hash_table)
{

    for(int i = 0 ; i < table_size ; i++)
    {
        for(int k = 0 ; k < hash_table->hash_nodes[i].elem_count ; k++)
        {
            Destroy_bucket(&(hash_table->hash_nodes[i].my_bucket[k]));
        }
        free(hash_table->hash_nodes[i].my_bucket);
    }
    free(hash_table->hash_nodes);
    free(hash_table);
}

Trie_Node* Hash_Table_Insert(Hash_Table* hash_table,char* new_element,int is_final)
{
    Trie_Node* my_node =  Hash_Table_Search(hash_table,new_element,0);
    if(my_node != NULL) /*If string exists on hashtable we return where we found it*/
    {
        if(my_node->is_final == 0)
        {
            if(is_final == 1) my_node->string_table[0] = my_node->string_table[0] < 0 ? -my_node->string_table[0]:my_node->string_table[0];
            my_node->is_final = is_final;
        }
        return my_node;
    }
    /*If we havent found string on HT we have to create a new node and insert it*/
    /*Find right position*/
    int bucket_index = hash(new_element,split_round);

    if(bucket_index < split_index) bucket_index = hash(new_element,split_round+1);

    Hash_Node* node_to_insert = &(hash_table->hash_nodes[bucket_index]);
    /*If bucket has available space for new element*/
    if(node_to_insert->elem_count <  node_to_insert->size)
    {


        /*Create new node*/
        Trie_Node* new_node = New_Node(new_element,is_final);
        my_node = Bucket_Insert(node_to_insert->my_bucket,&(node_to_insert->elem_count),new_node);
        free(new_node);
        return my_node; //Return location of our new node*/

    }
    else /*If we have an overflow bucket,we extend our bucket,extend our table and split the right bucket*/
    {

        /*Increase size of bucket*/
        int old_size = node_to_insert->size;

        /*Reallocate bucket*/
        node_to_insert->size += bucket_size;
        node_to_insert->my_bucket = realloc(node_to_insert->my_bucket,
                                            (node_to_insert->size)*sizeof(Trie_Node));


        memset(&(node_to_insert->my_bucket[old_size]) ,'\0',sizeof(Trie_Node));

        /*Create new node and insert it*/
        Trie_Node* new_node = New_Node(new_element,is_final);

        my_node = Bucket_Insert(node_to_insert->my_bucket,&(node_to_insert->elem_count),new_node);



        /*Reallocate hash table (+1) */
        hash_table->hash_nodes = realloc(hash_table->hash_nodes,(table_size+1)*sizeof(Hash_Node));

        hash_table->hash_nodes[table_size].my_bucket =  malloc(bucket_size*sizeof(Trie_Node));

        for(int k = 0 ; k < bucket_size ; k++)
        {
            memset(&(hash_table->hash_nodes[table_size].my_bucket[k]),'\0',sizeof(Trie_Node));
        }
        hash_table->hash_nodes[table_size].size = bucket_size;
        hash_table->hash_nodes[table_size].elem_count = 0;

        int overflow_split = 0;
        if(bucket_index == split_index) overflow_split = 1;
        /*Split bucket pointed by split_index*/
        split(hash_table);
        split_index++;
        table_size++;

        free(new_node);

        /*When we are done with our split_round we move to next*/
        if( (table_size % (power(2,split_round)*m) ) == 0)
        {
            split_index = 0;
            split_round++;
        }

        if(overflow_split)
        {
            return Hash_Table_Search(hash_table,new_element,1);
        }
        return my_node;


    }
}


void split(Hash_Table* hash_table)
{
    Hash_Node* split_node = &(hash_table->hash_nodes[split_index]);
    Hash_Node* last_node = &(hash_table->hash_nodes[table_size]);
    int i = 0;
    while( i < split_node->elem_count)
    {

        int index = hash(split_node->my_bucket[i].word,split_round+1);

        if( index != split_index)
        {
            if(last_node->elem_count == last_node->size)
            {
                int old_size = last_node->size;

                last_node->size += bucket_size;

                last_node->my_bucket = realloc(last_node->my_bucket, last_node->size*sizeof(Trie_Node));

                memset(&(last_node->my_bucket[old_size]),'\0',sizeof(Trie_Node));
            }


            Trie_Node* new_node = malloc(sizeof(Trie_Node));
            memcpy(new_node,&(split_node->my_bucket[i]), sizeof(Trie_Node));



            Bucket_Insert(last_node->my_bucket,&(last_node->elem_count),new_node);
            free(new_node);
            
            memmove(&(split_node->my_bucket[i]),&(split_node->my_bucket[i+1]),(split_node->elem_count-1-i)*sizeof(Trie_Node));


            memset(&(split_node->my_bucket[split_node->elem_count-1]),'\0',sizeof(Trie_Node));


            (split_node->elem_count)--;


        }
        else i++;


    }
}


Trie_Node* Bucket_Insert(Trie_Node* bucket,int* bucket_count,Trie_Node* new_node)
{
	int left,right,mid;

    left = 0;

    right = *bucket_count;
    int str_result;
    while(left < right)
    {
        mid = (left+right)/2;
        str_result = strcmp(bucket[mid].word,new_node->word);

        if(str_result > 0) right = mid;
        else left = mid + 1;

    }

    if(left < *bucket_count)
    {
        memmove(&(bucket[left+1]),
                &(bucket[left]),(*bucket_count - left )*sizeof(Trie_Node));
    }
    memmove(&(bucket[left]),new_node,sizeof(Trie_Node));
    (*bucket_count)++;
    return &(bucket[left]);



}
/*mode = 1 ---> STATIC else DYNAMIC */
Trie_Node* Hash_Table_Search(Hash_Table* hash_table,char* element,int mode)
{
    Trie_Node* temp;
    int index = hash(element, split_round);
    if(index < split_index) index = hash(element,split_round+1);
    int val = -1;
    if(mode == 1) val = static_binary_search_bucket(hash_table->hash_nodes[index].my_bucket,0,hash_table->hash_nodes[index].elem_count-1,element);
    else val = binary_search_bucket(hash_table->hash_nodes[index].my_bucket,0,hash_table->hash_nodes[index].elem_count-1,element);
    if(val == -1) return NULL;


    return &(hash_table->hash_nodes[index].my_bucket[val]);



}

void Print_HT(Hash_Table* hash_table)
{
    for(int i = 0 ; i < table_size ; i++)
    {
        for(int k = 0 ; k < hash_table->hash_nodes[i].elem_count ; k++)
        {
            printf("%s ",hash_table->hash_nodes[i].my_bucket[k].word);
        }
        if(hash_table->hash_nodes[i].elem_count) printf("\n");
    }
}
int Hash_Table_Delete(Hash_Table* hash_table,char* element)
{
    /*Find right bucket*/
    int index = hash(element, split_round);
    if(index < split_index) index = hash(element,split_round+1);

    int val = binary_search_bucket(hash_table->hash_nodes[index].my_bucket,0,hash_table->hash_nodes[index].elem_count-1,element);
    if(val != -1) return 1; /*WE MADE IT*/
    else return  0; /*Klasthke h diagrafh sou bro*/


}




