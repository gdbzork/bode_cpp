#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>

typedef void *dictionary;

dictionary dict_alloc(char *legal);
void dict_free(dictionary dict);
void dict_add(dictionary dict,char *key,void *value);
int dict_contains(dictionary dict,char *key);
void *dict_get(dictionary dict,char *key);
void dict_stat(FILE *fd,dictionary dict);
int dict_count(dictionary dict);
char **dict_keys(dictionary dict);

#endif
