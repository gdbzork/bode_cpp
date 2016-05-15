#include <stdio.h>
#include <string.h>

#include "sequence.h"

int main(int argc,char **argv) {
  sequence seq;
  sequence sub;
  char *test1 = "ACGTACGTACGT";
  char *test2 = "CACACACACA";
  char *test3 = "ACGTACGTACGTCACACACACA";
  char *res;

  seq = seq_allocbs(4);
  seq_assertSanity(seq);

  seq_setName(seq,"Naomi");
  seq_assertSanity(seq);
  seq_append(seq,test1,strlen(test1));
  seq_dump(stderr,seq);
  seq_assertSanity(seq);

  seq_print(stderr,seq);
  
  seq_append(seq,test2,4);
  seq_dump(stderr,seq);
  seq_assertSanity(seq);
  seq_append(seq,test2+4,6);
  seq_dump(stderr,seq);
  seq_assertSanity(seq);
  res = seq_string(seq);
  if (strcmp(res,test3) != 0) {
    fprintf(stderr,"mismatch: '%s'\n          '%s'\n",res,test3);
  }
  seq_assertSanity(seq);
  seq_print(stderr,seq);
  seq_printlw(stderr,seq,10);
  seq_printlw(stderr,seq,5);
  seq_printlw(stderr,seq,4);
  seq_printlw(stderr,seq,3);

  sub = seq_alloc();
  seq_assertSanity(sub);
  seq_substr(sub,seq,0,5);
  res = seq_string(sub);
  if (strcmp(res,"ACGTA")) {
    fprintf(stderr,"mismatch: '%s'\n          '%s'\n",res,"ACGTA");
  }
  seq_print(stderr,sub);
   
  seq_assertSanity(sub);
  seq_substr(sub,seq,2,9);
  res = seq_string(sub);
  if (strcmp(res,"GTACGTACG")) {
    fprintf(stderr,"mismatch: '%s'\n          '%s'\n",res,"GTACGTACG");
  }
  seq_print(stderr,sub);
   

  seq_free(seq);
  seq_free(sub);
}
