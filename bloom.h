//
// Created by tentas on 11/16/17.
//

#ifndef ANAPTUKSH_IWANNIDH_2017_BLOOM_H
#define ANAPTUKSH_IWANNIDH_2017_BLOOM_H


short int* Init_Bloom(void);
static size_t djb_hash(const char* cp);
static size_t fnv1a_hash(const char* cp);
unsigned int hash_bloom (const char* word);
unsigned int hash_bloom_1(char* s);
static unsigned long sdbm_hash(char *str);
int check_bloom(short int *bloom, char* n_gram);
void insert_bloom(short int *bloom, char* n_gram);

#endif //ANAPTUKSH_IWANNIDH_2017_BLOOM_H
