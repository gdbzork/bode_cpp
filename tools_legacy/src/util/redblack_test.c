#include <stdio.h>
#include <stdlib.h>

#include "redblack.h"

int main() {
  int i,found;
  double c;
  redBlackTreeP rbt;
  srand48(123);
  double sa[501];
  int saind;
  
  rbt = rb_new();
  saind = 0;
  for (i=0;i<50000000;i++) {
    c = drand48();
    rb_insert(rbt,c);
    if (i % 100000 == 0) {
      fprintf(stderr,"%09d\r",i);
      sa[saind++] = c;
    }
  }

  found = 0;
  for (i=0;i<saind;i++) {
    if (rb_search(rbt,sa[i])) {
      found++;
    }
  }
  fprintf(stderr,"saved %d, found %d\n",saind,found);

  found = 0;
  for (i=0;i<saind;i++) {
    c = drand48();
    if (rb_search(rbt,c)) {
      found++;
    }
  }
  fprintf(stderr,"generated %d, found %d\n",saind,found);
    
  return 0;
}
