//
// Created by tentas on 10/11/17.
//


#include "trie.h"
#include "bloom.h"
#include <time.h>

extern int table_size;

int my_strcmp ( char * s1, char * s2) {
    int len1 = my_strlen(s1);
    int len2 = strlen(s2);
    int size;

    if(len1 > len2) size = len1;
    else size = len2;

    for (int i = 0; i < size && (s1[i] != ' ' || s1[i] != '\n'); ++i) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        else if (!s1[i])
            return 0;
    }
    return 0;
}

int binary_search(Trie_Node* current_node,int min,int max,char* word)
{
    int str_result;
    int mid = 0;
    while(max >= min)
    {
        mid = (max+min)/2;
        str_result = my_strcmp(word,current_node->children[mid].word);
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

Trie* Init_Trie(void)
{
    Trie* trie = malloc(sizeof(Trie));

    trie->root = malloc(sizeof(Trie_Node));

    trie->depth = 0;

    trie->root->word = NULL;
    trie->root->string_table = NULL;
    trie->root->string_table_size = 0;
    trie->root->fere = 0;

    trie->root->size = 0;

    trie->root->is_final = 0;

    trie->root->children = malloc(SIZE*sizeof(Trie_Node));

    memset(trie->root->children,'\0',SIZE*sizeof(Trie_Node));

    trie->root->hash_table = Hash_Table_Init();

    return trie;
}

void Insert_Ngram(Trie* trie,char* ngram,int version) {
    int local_depth = 0;
    Trie_Node *current_node = trie->root;

    char *current_word = strtok(ngram, " \n");  // Get first word of current ngram

    char *remaining_ngram = strtok(NULL, "\n");  // Get the rest ngram

    int i , position,node_size;

    if(remaining_ngram != NULL) current_node = Hash_Table_Insert(trie->root->hash_table,current_word,0);
    else current_node = Hash_Table_Insert(trie->root->hash_table,current_word,1);

    if(current_node->add_version == -1 && current_node->delete_version == -1) /* Valame molis sto HT */
    {
        current_node->add_version = version;
        current_node->became_non_final = -1;
        current_node->became_final = -1;
    }

    if(current_node->delete_version != -1 && current_node->delete_version <= version) /* An eixe diagrafei se prohgoumenh riph kai to ksanavazw */
    {
        current_node->delete_version = -1; // den einai pleon diagrammeno */
        current_node->add_version = version;
    }
    if(remaining_ngram == NULL) /* An prepei na ginei final,ananewnw to final version */
    {
        current_node->became_final = version;
    }



    local_depth++;

    current_word = strtok(remaining_ngram, " \n"); ///Getting next word if exists.

    remaining_ngram = strtok(NULL,"\n"); /// Getting the rest of the ngram if exists.

    int left,right,mid,found;
    int str_result;

    while (current_word != NULL) {

        left = 0;
        found = 0;
        right = current_node->size;

        while(left < right && found == 0)
        {
            mid = (left+right)/2;
            //printf(" mid = %d  size = %d psaxnontas to  %s sto %s\n", mid, current_node->size, current_word, current_node->children[mid].word);
            str_result = strcmp(current_word,current_node->children[mid].word);

            if(str_result == 0)
            {

                if (remaining_ngram == NULL)
                {
                    current_node->children[mid].is_final = 1;
                    if(current_node->children[mid].became_final == -1) /* An den exei ginei final pio prin */
                    {
                        current_node->children[mid].became_final = version;
                        current_node->children[mid].became_non_final = -1;
                    }
                }
                if(current_node->children[mid].delete_version != -1 && current_node->children[mid].delete_version <= version) /* An eixe diagrafei se prohgoumenh riph kai to ksanavazw */
                {
                    current_node->children[mid].delete_version = -1; // den einai pleon diagrammeno */
                    current_node->children[mid].add_version = version;
                    current_node->children[mid].active += 1;
                }

                current_node = &current_node->children[mid];

                found = 1;

            }
            if(str_result < 0) right = mid;
            else left = mid + 1;

        }
        if(!found) {

            node_size = current_node->size;
            if (((current_node->size) % SIZE == 0) && (current_node->size != 0)) {
                //printf("ekana realloc prin eixa size %d \n", current_node->size);
                current_node->children = realloc(current_node->children, (node_size*2)*sizeof(Trie_Node));
            }

            if (left < current_node->size) {
                memmove(&(current_node->children[left+1]),
                        &(current_node->children[left]),
                        (node_size - left)*sizeof(Trie_Node));
            }

            Trie_Node* new_node = malloc(sizeof(Trie_Node));

            new_node->word = malloc((strlen(current_word)+1)*sizeof(char));
            strcpy(new_node->word, current_word);

            if(remaining_ngram == NULL)
            {
                new_node->is_final = 1;
                new_node->became_final = version;
                new_node->became_non_final = -1;
            }
            else
            {
                new_node->is_final = 0;
                new_node->became_non_final = -1;
                new_node->became_final = -1;
            }

            new_node->string_table = NULL;
            new_node->fere = 0;
            new_node->size = 0;
            new_node->active = 0;
            new_node->add_version = version;
            new_node->delete_version = -1;


            new_node->children = malloc(SIZE*sizeof(Trie_Node));

            memset(new_node->children,'\0',SIZE*sizeof(Trie_Node));

            //printf(" left = %d  size = %d bazontas to %s sto %p\n", left, current_node->size, current_word, &(current_node->children[left].word));
            memcpy(&(current_node->children[left]),new_node, sizeof(Trie_Node));
            free(new_node);

            current_node->size += 1;
            current_node->active += 1;
            current_node = &current_node->children[left];
        }


        current_word = strtok(remaining_ngram, " \n"); ///Getting next word if exists.

        remaining_ngram = strtok(NULL,"\n"); /// Getting the rest of the ngram if exists.

    }
    if(local_depth > trie->depth) trie->depth = local_depth;
}

void Search_Substream(Trie_Node* root,char* ngram, char** result,int count, short int *bloom, Index* result_array,char** on_going_ngram,int version) {
    Trie_Node *current_node = root;

    int i, position, node_size;
    while (ngram != NULL) {
        position = -1;

        node_size = current_node->size;
        i = 0;

        position = binary_search(current_node, 0, node_size - 1, ngram);
        ///Checking if current word exists in current_nodes children


        if(position != -1 && (current_node->children[position].add_version <= version && (current_node->children[position].delete_version == -1 || current_node->children[position].delete_version > version))){
            strcat(*on_going_ngram, " ");
            strcat(*on_going_ngram, current_node->children[position].word);

            if (current_node->children[position].is_final &&
                    (current_node->children[position].became_final != -1 && current_node->children[position].became_final <= version
                     && (current_node->children[position].became_non_final == -1 || current_node->children[position].became_non_final > version ))) {
                if (strlen(*result) == 0) {
                    strcpy(*result, *on_going_ngram);
                    insert_bloom(bloom, *on_going_ngram);
                    Insert_Result_Array(result_array, *on_going_ngram);
                } else if (check_bloom(bloom, *on_going_ngram)) {
                    strcat(*result, "|");
                    strcat(*result, *on_going_ngram);
                    Insert_Result_Array(result_array, *on_going_ngram);
                }

            }

            current_node = &current_node->children[position];

            ngram = strchr(ngram, ' ');
            if (ngram) {
                ngram++;
                while (*ngram == ' ') ngram++;
            }
        }
        else return;

    }
    return;
}

char* Search_Ngram(Trie* trie,char* ngram,int count, Index* result_array,int version) {
    char *save = ngram;
    char *iter_save;

    char *result = malloc(((strlen(ngram)) + 1) * sizeof(char));
    memset(result, '\0', (strlen(ngram) + 1) * sizeof(char));
    ///Init bloom filter.
    short int *bloom = Init_Bloom();

    char *on_going_ngram = malloc(2048);

    int len = 0, max_depth = trie->depth + 1;
    char *token;
    Trie_Node *root_node;
    while (ngram != NULL) {
        iter_save = ngram;


        root_node = Hash_Table_Search(trie->root->hash_table, ngram,0);

        if (root_node != NULL && (root_node->add_version <= version && (root_node->delete_version == -1 || root_node->delete_version > version))) {


            memset(on_going_ngram, '\0', 2048);
            strcpy(on_going_ngram, root_node->word);
            if (root_node->is_final == 1 &&
                    (root_node->became_final != -1 && root_node->became_final <= version && (root_node->became_non_final == -1 || root_node->became_non_final > version ))) {

                if (strlen(result) == 0) {
                    strcpy(result, on_going_ngram);
                    insert_bloom(bloom, on_going_ngram);
                    Insert_Result_Array(result_array, on_going_ngram);
                } else if (check_bloom(bloom, on_going_ngram)) {
                    strcat(result, "|");
                    strcat(result, on_going_ngram);
                    Insert_Result_Array(result_array, on_going_ngram);
                }
            }

            if (ngram != NULL) {
                ngram = strchr(ngram, ' ');
                if (ngram) {
                    ngram++;
                    while (*ngram == ' ') ngram++;
                }
                Search_Substream(root_node, ngram, &result, count, bloom, result_array, &on_going_ngram,version);
            }

        }

        ngram = iter_save;
        ngram = strchr(ngram, ' ');
        if (ngram) {
            ngram++;
            while (*ngram == ' ') ngram++;
        }

    }

    free(bloom);
    free(on_going_ngram);
    
    if (strlen(result) == 0) {
        return NULL;
    } else {
        return result;
    }
}

int Delete_Ngram(Trie_Node* current_node,char* ngram,int hash_count,int version) {


    char *current_word = strtok(ngram, " \n");
    char *remaining_ngram = strtok(NULL, "\n");

    if (hash_count == 0) {
        Trie_Node *pos_next_node = Hash_Table_Search(current_node->hash_table, current_word,0);

        if (pos_next_node != NULL) {

            if (Delete_Ngram(pos_next_node, remaining_ngram, ++hash_count,version)) {
                Hash_Table_Delete(current_node->hash_table, current_word);

            }

            return 1;

        } else return 0;
    }


    int position = -1;

    if (current_word == NULL) {

        if (current_node->is_final == 1) {

            current_node->became_non_final = version;

            if (current_node->active == 0) {
                current_node->delete_version = version;

                return 1;
            }
            return 0;
        }
    } else {
        position = binary_search(current_node, 0, current_node->size - 1, current_word);
        if (position == -1) {
            return 0;
        } else {
            if (Delete_Ngram(&(current_node->children[position]), remaining_ngram, ++hash_count,version)) {

                current_node->active -= 1;

                if ((current_node->active == 0) && current_node->is_final == 0 ) {
                    current_node->delete_version = version;

                    return 1;
                }
                return 0;
            }
        }
    }

    return 0;
}

int my_strlen(char *s) {
    int i = 0;
    for (i = 0; s[i] != ' ' && s[i] != '\0'; i++);
    return i;
}


int my_strncmp (char * s1, char * s2, int size) {
    for (int i = 0; i < size && (s1[i] != ' ' || s1[i] != '\n'); ++i) {
        if (s1[i] < s2[i])
            return -1;
        else if (s1[i] > s2[i])
            return 1;
        else if (!s1[i])
            return 0;
    }
    return 0;
}


int Static_binary_search(Trie_Node* current_node,int min,int max,char* word) {
    int mid, str_result, len, i = 0;
    char* temp = NULL;

    while (max >= min) {
        mid = (max + min) / 2;

        len = current_node->children[mid].string_table[0];

        if (len < 0) len = -len;

        str_result = my_strncmp(word, current_node->children[mid].word, len);

        if(str_result == 0 && len < my_strlen(word))
        {
            min = mid + 1;
        }

        else if (len == my_strlen(word) && str_result == 0) {
            if(temp) free(temp);
            return mid;
        }
        else if (str_result > 0) min = mid + 1;
        else max = mid - 1;
    }
    if(temp) free(temp);
    return -1;

}

void Insert_Static(Trie* trie, char* ngram) {
    Trie_Node *current_node = trie->root;
    //Get first word of current ngram
    char *current_word = strtok(ngram, " \n");
    //Get the rest ngram
    char *remaining_ngram = strtok(NULL, "\n");

    if(remaining_ngram != NULL) current_node = Hash_Table_Insert(trie->root->hash_table,current_word,0);
    else current_node = Hash_Table_Insert(trie->root->hash_table,current_word,1);
    
    int i = 0, position, node_size, right_position, local = 1;

    current_word = strtok(remaining_ngram, " \n"); ///Getting next word if exists.

    remaining_ngram = strtok(NULL,"\n"); /// Getting the rest of the ngram if exists.

    int left,right,mid,str_result,found;

    while (current_word != NULL) {

        left = 0;
        found = 0;
        right = current_node->size;

        while(left < right && found == 0)
        {
            mid = (left+right)/2;
            str_result = strcmp(current_word,current_node->children[mid].word);

            if(str_result == 0)
            {

                if (remaining_ngram == NULL && current_node->children[mid].is_final == 0) {
                    current_node->children[mid].is_final = 1;
                    current_node->children[mid].string_table[0] = -(current_node->children[mid].string_table[0]);
                }
                current_node = &current_node->children[mid];

                found = 1;

            }
            if(str_result < 0) right = mid;
            else left = mid + 1;

        }
        if(!found) {

            node_size = current_node->size;
            if (((current_node->size) % SIZE == 0) && (current_node->size != 0)) {
                current_node->children = realloc(current_node->children, (node_size*2)*sizeof(Trie_Node));
            }

            if (left < current_node->size) {
                memmove(&(current_node->children[left+1]),
                        &(current_node->children[left]),
                        (node_size - left)*sizeof(Trie_Node));
            }


            current_node->children[left].word = strdup(current_word);

            current_node->children[left].string_table = calloc(STRING_TABLE_SIZE , sizeof(signed short int));

            //Assigning the right values to the final identifiers
            if (remaining_ngram == NULL) {
                current_node->children[left].is_final = 1;
                current_node->children[left].string_table[0] = strlen(current_word);

            } else {
                current_node->children[left].is_final = 0;
                current_node->children[left].string_table[0] = -(strlen(current_word));
            }

            current_node->children[left].string_table_size = 1;

            //Init of labeling and children table variables
            current_node->children[left].fere = 0;
            current_node->children[left].size = 0;

            //Allocating children array and setting value to zero.
            current_node->children[left].children = malloc(SIZE * sizeof(Trie_Node));

            memset(current_node->children[left].children, '\0', SIZE * sizeof(Trie_Node));

            current_node->size += 1;

            //Passing new node as current to keep inserting the rest of the ngram
            current_node = &(current_node->children[left]);
        }
        //Getting next word if exists.
        current_word = strtok(remaining_ngram, " \n");
        //Getting the rest of the ngram if exists.
        remaining_ngram = strtok(NULL, "\n");
    }

    if(local > trie->depth) trie->depth = local;
}

void  pull( Trie_Node* top_node) {

    Trie_Node *temp = NULL;

    while (top_node->size == 1) {

        temp = top_node->children;

        top_node->fere = top_node->children[0].fere;   // coping fere
        top_node->is_final = top_node->children[0].is_final; // coping is final
        if (top_node->word == NULL) {                               // Initialising or concatenatin strings

            top_node->word = malloc((strlen(top_node->children[0].word) + 1) * sizeof(char));
            strcpy(top_node->word, top_node->children[0].word);

        } else {
            top_node->word = realloc(top_node->word,
                                     (strlen(top_node->word) + strlen(top_node->children[0].word) + 1) * sizeof(char));
            strcat(top_node->word, top_node->children[0].word);
        }
        if(top_node->string_table_size % STRING_TABLE_SIZE == 0 && top_node->string_table_size != 0){
            top_node->string_table = realloc(top_node->string_table, (top_node->string_table_size*2)*sizeof(int));
        }
        top_node->string_table[top_node->string_table_size] = top_node->children[0].string_table[0]; // copying string table value and size
        top_node->string_table_size++;


        if (top_node->children == NULL) {                            // Won't somebody think of the children???
            top_node->children = malloc((top_node->children[0].size) *
                                        sizeof(Trie_Node));                    //sizeof(current_node->children)+sizeof(Trie_Node));

        } else {
            top_node->size = top_node->children[0].size;
            top_node->children = (top_node->children[0].children);
            free(temp[0].word);
            free(temp[0].string_table);
            free(temp);
        }

    }
}
void compress(Trie* trie)
{
    Hash_Table* hash_table = trie->root->hash_table;

    for(int i = 0 ; i < table_size ; i++)
    {
        for(int k = 0 ; k < hash_table->hash_nodes[i].elem_count ; k++)
        {
            compress_bucket(&(hash_table->hash_nodes[i].my_bucket[k]));
        }
    }
}


void compress_bucket(Trie_Node* current) {
    if(current) {
        if(current->size == 1)
        {
            pull(current);
        }
        int i = 0;
        for (i = 0; i < current->size; i++) {
            if (current->children[i].size == 1) {
                pull(&(current->children[i]));
                if (current->children[i].size > 1) {
                    compress_bucket(&(current->children[i]));
                }
            } else if (current->children[i].size > 1) {
                compress_bucket(&(current->children[i]));
            }
        }
    }
}

void Static_Substream(Trie_Node* root,char* ngram, char** result, char** on_going_ngram, short int *bloom, Index* result_array,int count) {

    Trie_Node *current_node = root;                //search starting from root

    char *save;

    char *check_word;

    char *temp = NULL;
    int i, position, node_size, len;


    while (ngram != NULL) {
        position = -1;            // storing the position of the  word among children nodes

        node_size = current_node->size;
        i = 0;

        position = Static_binary_search(current_node, 0, node_size - 1, ngram);
        ///Checking if current word exists in current_nodes children


        if (position == -1) /// If we haven't found the word in current_node's children
        {
            //if(on_going_ngram) free(on_going_ngram);
            if (temp) free(temp);
            return;
        }
        else
        {
            if (strlen(*on_going_ngram) == 0) strncpy(*on_going_ngram, ngram, my_strlen(ngram));
            else
            {
                strcat(*on_going_ngram, " ");
                strncat(*on_going_ngram, ngram, my_strlen(ngram));
            }
            if(current_node->children[position].string_table[0] > 0)
            {
                if (strlen(*result) == 0)
                {
                    strcpy(*result, *on_going_ngram);
                    insert_bloom(bloom, *on_going_ngram);
                    Insert_Result_Array(result_array, *on_going_ngram);
                }
                else if (check_bloom(bloom, *on_going_ngram)) {
                    strcat(*result, "|");
                    strcat(*result, *on_going_ngram);
                    Insert_Result_Array(result_array, *on_going_ngram);
                }
            }

            check_word = strdup(current_node->children[position].word);

            temp = check_word;


            len = current_node->children[position].string_table[0];

            if (len < 0) len = -len;

            check_word += len;

            for (i = 1; i < current_node->children[position].string_table_size; i++) {

                ngram = strchr(ngram, ' ');
                if (ngram) {
                    ngram++;
                    while (*ngram == ' ') ngram++;
                }

                len = current_node->children[position].string_table[i];

                if (len < 0) len = -len;

                if(ngram == NULL || len != my_strlen(ngram))
                {
                    if (temp) free(temp);
                    return;
                }

                if (!my_strncmp(check_word, ngram, len))
                {
                    strcat(*on_going_ngram, " ");
                    strncat(*on_going_ngram, ngram, len);

                    if (current_node->children[position].string_table[i] > 0)
                    {
                        if (strlen(*result) == 0)
                        {
                            strcpy(*result, *on_going_ngram);
                            insert_bloom(bloom, *on_going_ngram);
                            Insert_Result_Array(result_array, *on_going_ngram);
                        }
                        else if (check_bloom(bloom, *on_going_ngram))
                        {
                            strcat(*result, "|");
                            strcat(*result, *on_going_ngram);
                            Insert_Result_Array(result_array, *on_going_ngram);
                        }
                    }
                }
                else
                {
                    if (temp) free(temp);
                    return;
                }


                check_word += len;
            }
            current_node = &(current_node->children[position]);

        }
        if(temp) free(temp);
        temp = NULL;

        ngram = strchr(ngram, ' ');
        if (ngram) {
            ngram++;
            while (*ngram == ' ') ngram++;
        }
    }

    return;
}








char* Search_Static(Trie* trie,char* ngram,int count, Index* result_array)
{
    char **save = &ngram;
    short int *bloom = Init_Bloom();

    char *result = malloc(((strlen(ngram)) + 1) * sizeof(char)); //allocating memory for result string
    memset(result, '\0', (strlen(ngram) + 1) * sizeof(char));      //arxikopoihsh tou result
    char *check_sub = NULL;                    // string used for storing each substream before searching
    int len = 0, max_depth = trie->depth + 1;
    char *cs_copy;
    char *token;
    char *on_going_ngram;
    char* temp = NULL;
    char* check_word;
    Trie_Node *root_node;
    char *iter_save;
    on_going_ngram = malloc(2048);

    while (ngram != NULL)        //until we have search  each possible substream of ngram
    {
        iter_save = ngram;
 
        root_node = Hash_Table_Search(trie->root->hash_table, ngram,1);

        if (root_node != NULL) {

            memset(on_going_ngram, '\0', 2048);

            strncat(on_going_ngram, ngram, my_strlen(ngram));

            if (root_node->string_table[0] > 0) {

                if (strlen(result) == 0) {
                    strcpy(result, on_going_ngram);
                    insert_bloom(bloom, on_going_ngram);
                    Insert_Result_Array(result_array, on_going_ngram);
                } else if (check_bloom(bloom, on_going_ngram)) {
                    strcat(result, "|");
                    strcat(result, on_going_ngram);
                    Insert_Result_Array(result_array, on_going_ngram);
                }
            }

            char* check_word = strdup(root_node->word);


            int flag = 0;

            temp = check_word;


            len = root_node->string_table[0];

            if (len < 0) len = -len;

            check_word += len;


            for (int i = 1; i < root_node->string_table_size; i++) {

                ngram = strchr(ngram, ' ');
                if (ngram) {
                    ngram++;
                    while (*ngram == ' ') ngram++;
                }

                len = root_node->string_table[i];

                if (len < 0) len = -len;

                if(ngram == NULL || len != my_strlen(ngram))
                {
                    if (temp) free(temp);
                    temp = NULL;
                    flag = 1;
                    break;
                }

                if (!my_strncmp(check_word, ngram, len))
                {
                    strcat(on_going_ngram, " ");
                    strncat(on_going_ngram, ngram, my_strlen(ngram));

                    if (root_node->string_table[i] > 0)
                    {
                        if (strlen(result) == 0)
                        {
                            strcpy(result,on_going_ngram);
                            insert_bloom(bloom, on_going_ngram);
                            Insert_Result_Array(result_array, on_going_ngram);
                        }
                        else if (check_bloom(bloom, on_going_ngram))
                        {
                            strcat(result, "|");
                            strcat(result, on_going_ngram);
                            Insert_Result_Array(result_array, on_going_ngram);
                        }
                    }
                }
                else
                {
                    if (temp) free(temp);
                    temp = NULL;

                    flag = 1;
                    break;
                }

                check_word += len;
            }
            if(temp) free(temp);

            if (ngram != NULL && flag == 0)
            {
                ngram = strchr(ngram, ' ');
                if (ngram) {
                    ngram++;
                    while (*ngram == ' ') ngram++;
                }
                Static_Substream(root_node, ngram, &result, &on_going_ngram, bloom, result_array, count);        //search check sub in trie
            }

        }


        ngram = iter_save;
        ngram = strchr(ngram, ' ');
        if (ngram) {
            ngram++;
            while (*ngram == ' ') ngram++;
        }
    }
    free(check_word);
    free(bloom);

    if (strlen(result) == 0)            // if not found
    {
        free(result);
        free(on_going_ngram);
        free(*save);
        return NULL;
    } else {
        free(on_going_ngram);
        free(*save);
        return result;                //else print the result
    }

}

void Destroy_Trie(Trie* trie)
{
    Hash_Table* hash_table = trie->root->hash_table;
    for(int i = 0 ; i < table_size ; i++)
    {
        for(int k = 0 ; k < hash_table->hash_nodes[i].elem_count ; k++)
        {
            Destroy_bucket(&(hash_table->hash_nodes[i].my_bucket[k]));
        }
    }
}



void Destroy_bucket(Trie_Node* current) {
    if(current) {
        int i = 0;
        for (i = 0; i < current->size; i++){
            Destroy_bucket(&(current->children[i]));
        }

        if(current->string_table) free(current->string_table);
        free(current->word);
        free(current->children);
    }
}
