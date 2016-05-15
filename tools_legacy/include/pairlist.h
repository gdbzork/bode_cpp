#ifndef PAIRLIST_H
#define PAIRLIST_H

#include <stdio.h>

typedef struct pair {
  int left;
  int right;
  int height;
} *pairp;

typedef struct pairlist {
  pairp pairs;
  int index;
  int size;
} *pairlist;

pairlist pl_create();
void pl_add(pairlist p,int l,int r,int h);
void pl_sort(pairlist p);
int pl_count(pairlist p);
pairp pl_get(pairlist p,int index);
void pl_dump(FILE *fd,pairlist p);

#endif
