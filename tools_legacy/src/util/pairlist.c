#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pairlist.h"

#define INCREMENT 1000

pairlist pl_create() {
  pairlist p;
  p = (pairlist) malloc(sizeof(struct pairlist));
  p->pairs = (pairp) calloc(INCREMENT,sizeof(struct pair));
  p->size = INCREMENT;
  p->index = 0;
  return p;
}

void pl_add(pairlist p,int l,int r,int h) {
  if (p->index+1 == p->size) {
    p->pairs = (pairp) realloc(p->pairs,(p->size+INCREMENT)*sizeof(struct pair));
    assert(p->pairs != NULL);
    p->size = p->size+INCREMENT;
  }
  p->pairs[p->index].left = l;
  p->pairs[p->index].right = r;
  p->pairs[p->index].height = h;
  p->index++;
}

int pl__sortfunc(const void *a,const void *b) {
  pairp pa = (pairp) a;
  pairp pb = (pairp) b;
  int rv;
  if (pa->left != pb->left) {
    rv = pa->left < pb->left ? -1 : 1;
  } else {
    rv = pa->right < pb->right ? -1 : (pa->right > pb->right ? 1 : 0);
  }
  return rv;
}

void pl_sort(pairlist p) {
  qsort(p->pairs,p->index,sizeof(struct pair),&pl__sortfunc);
}

int pl_count(pairlist p) {
  return p->index;
}

pairp pl_get(pairlist p,int index) {
  return p->pairs+index;
}

void pl_dump(FILE *fd,pairlist p) {
  int i;
  fprintf(fd,"PL Dump:\n");
  for (i=0;i<p->index;i++) {
    fprintf(fd,"  %d-%d --> %d\n",p->pairs[i].left,p->pairs[i].right,p->pairs[i].height);
  }
}
