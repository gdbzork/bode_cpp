#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "pairlistAnnot.h"

#define INCREMENT 100000

pairlistA pl_createA() {
  pairlistA p;
  p = (pairlistA) malloc(sizeof(struct pairlistA));
  p->pairs = (pairAp) calloc(INCREMENT,sizeof(struct pairA));
  p->size = INCREMENT;
  p->index = 0;
  return p;
}

void pl_addA(pairlistA p,int l,int r,char *annot) {
  if (p->index+1 == p->size) {
    p->pairs = (pairAp) realloc(p->pairs,(p->size+INCREMENT)*sizeof(struct pairA));
    assert(p->pairs != NULL);
    p->size = p->size+INCREMENT;
  }
  p->pairs[p->index].left = l;
  p->pairs[p->index].right = r;
  p->pairs[p->index].annot = annot;
  p->index++;
}

int pl__sortfuncA(const void *a,const void *b) {
  pairAp pa = (pairAp) a;
  pairAp pb = (pairAp) b;
  int rv;
  if (pa->left != pb->left) {
    rv = pa->left < pb->left ? -1 : 1;
  } else {
    rv = pa->right < pb->right ? -1 : (pa->right > pb->right ? 1 : 0);
  }
  return rv;
}

void pl_sortA(pairlistA p) {
  qsort(p->pairs,p->index,sizeof(struct pairA),&pl__sortfuncA);
}

int pl_countA(pairlistA p) {
  return p->index;
}

pairAp pl_getA(pairlistA p,int index) {
  return p->pairs+index;
}
