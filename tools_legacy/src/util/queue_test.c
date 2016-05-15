#include <stdio.h>

#include "queue.h"

int main(int argc,char **argv) {
  char *tmp;

  queue q = q_create(1);
  if (q_size(q) != 0) {
    fprintf(stderr,"test 1: returned non-zero q size: %d",q_size(q));
  }
  q_add(q,"zork");
  q_add(q,"thing");
  tmp = (char *) q_get(q);
  if (strcmp(tmp,"zork")) {
    fprintf(stderr,"test 2: got wrong item off queue: %s",tmp);
  }
  if (q_size(q) != 2) {
    fprintf(stderr,"test 3: returned non-2 q size: %d",q_size(q));
  }
  q_remove(q);
  tmp = (char *) q_get(q);
  if (strcmp(tmp,"thing")) {
    fprintf(stderr,"test 4: got wrong item off queue: %s",tmp);
  }
  if (q_size(q) != 1) {
    fprintf(stderr,"test 5: returned non-1 q size: %d",q_size(q));
  }
  q_remove(q);
  if (q_size(q) != 0) {
    fprintf(stderr,"test 6: returned non-1 q size: %d",q_size(q));
  }
}
