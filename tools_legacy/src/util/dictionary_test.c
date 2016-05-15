#include <stdio.h>

#include "dictionary.h"

int main(int argc,char **argv) {
  dictionary d;
  char **ks;
  int i;

  d = dict_alloc("abcdefghijklmnopqrstuvwxyz");

  dict_stat(stderr,d);
  dict_add(d,"zork",(void *) "thing");
  dict_stat(stderr,d);
  fprintf(stderr,"dict %s zork\n",(dict_contains(d,"zork")?"contains":"does not contain"));
  fprintf(stderr,"value of zork is %s\n",(char *)dict_get(d,"zork"));
  fprintf(stderr,"value of zorn is %s\n",(char *)dict_get(d,"zorn"));
  fprintf(stderr,"value of zorky is %s\n",(char *)dict_get(d,"zorky"));
  fprintf(stderr,"value of zor is %s\n",(char *)dict_get(d,"zor"));
  dict_add(d,"sample",(void *)"bingo bongo");
  dict_stat(stderr,d);
  ks = dict_keys(d);
  i = 0;
  while (ks[i] != NULL) {
    fprintf(stderr,"  k[%d] = %s\n",i,ks[i]);
    i++;
  }
  dict_add(d,"sample",(void *)NULL);
  dict_stat(stderr,d);
  dict_add(d,"samplde",(void *)NULL);
  dict_stat(stderr,d);
  
  return 0;
}
