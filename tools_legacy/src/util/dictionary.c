#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dictionary.h"

#define BYTESIZE 256
#define KEYSIZE 1024

typedef struct link {
  struct link *next;
  void *data;
} *linkp;

typedef struct dict {
  int *index;
  char *legal;
  int legallen;
  long count;
  long blocks;
  linkp map;
  char *current;
} *dictp;

dictp dalloc(char *legalchars) {
  int i;
  dictp d;
  d = (dictp) malloc(sizeof(struct dict));
  if (d == NULL) {
    return NULL;
  }
  d->legallen = strlen(legalchars);
  d->index = calloc(BYTESIZE,sizeof(int));
  d->legal = strdup(legalchars);
  d->current = (char *) malloc(KEYSIZE * sizeof(char));
  d->current[0] = '\0';
  for (i=0;i<d->legallen;i++) {
    d->index[(int)legalchars[i]] = i;
  }
  d->count = 0;
  d->blocks = 1;
  /* calloc initializes space to 0, so we get null pointers for free */
  d->map = calloc(d->legallen,sizeof(struct link));
  return d;
}

void dadd(dictp head,linkp dict,char *key,void *value) {
  linkp node = dict+head->index[(int)key[0]];
  if (key[1] == '\0') { /* looking at last character */
    if (node->data == NULL) {
      head->count++; /* adding a new item */
    } else if (value == NULL) {
      head->count--; /* delete an item */
    }
    node->data = value;
  } else {
    if (node->next == NULL) {
      node->next = (linkp) calloc(head->legallen,sizeof(struct link));
      head->blocks++;
    }
    dadd(head,node->next,key+1,value);
  }
}

void *dget(dictp head,linkp dict,char *key) {
  if (dict == NULL) {
    return NULL;
  } else if (key[1] == '\0') { /* end of the key */
    return (dict[head->index[(int)key[0]]].data);
  } else {
    return dget(head,dict[head->index[(int)key[0]]].next,key+1);
  }
}

int dtraversenames(dictp d,linkp m,int depth,char **names,int index) {
  int i;
  d->current[depth+1] = '\0';
  for (i=0;i<d->legallen;i++) {
    d->current[depth] = d->legal[i];
    if (m[i].data != NULL) {
      names[index++] = strdup(d->current);
    }
    if (m[i].next != NULL) {
      index = dtraversenames(d,m[i].next,depth+1,names,index);
    }
  }
  d->current[depth] = '\0';
  return index;
}

dictionary dict_alloc(char *legal) {
  dictp d;
  d = dalloc(legal);
  return (dictionary) d;
}

void dict_free(dictionary dict) {
  free(dict);
}

void dict_add(dictionary dict,char *key,void *value) {
  dictp d = (dictp) dict;
  dadd(d,d->map,key,value);
}

int dict_contains(dictionary dict,char *key) {
  dictp d = (dictp) dict;
  return dget(d,d->map,key) != NULL;
}

void *dict_get(dictionary dict,char *key) {
  dictp d = (dictp) dict;
  return dget(d,d->map,key);
}

int dict_count(dictionary dict) {
  dictp d = (dictp) dict;
  return d->count;
}

char **dict_keys(dictionary dict) {
  dictp d = (dictp) dict;
  char **results;
  int i = 0;
  results = (char **) calloc(d->count+1,sizeof(char *));
  i = dtraversenames(d,d->map,0,results,0);
  assert(i==d->count);
  results[i] = NULL;
  return results;
}

void dict_stat(FILE *fd,dictionary dict) {
  dictp d = (dictp) dict;
  fprintf(fd,"dictionary: %ld items, %ld blocks, %ld space\n",
             d->count,d->blocks,d->blocks * d->legallen * sizeof(struct link));
}
