#ifndef PAIRLISTA_H
#define PAIRLISTA_H

typedef struct pairA {
  int left;
  int right;
  char *annot;
} *pairAp;

typedef struct pairlistA {
  pairAp pairs;
  int index;
  int size;
} *pairlistA;

pairlistA pl_createA();
void pl_addA(pairlistA p,int l,int r,char *annot);
void pl_sortA(pairlistA p);
int pl_countA(pairlistA p);
pairAp pl_getA(pairlistA p,int index);

#endif
