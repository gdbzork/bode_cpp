#include <stdio.h>
#include <stdlib.h>

#include "sequence.h"
#include "fastareader.h"

int main(int argc,char **argv) {
  sequence s1,s2;
  fastareader fr;

  if (argc < 2) {
    fprintf(stderr,"usage: %s <fasta file>\n",argv[0]);
    exit(-1);
  }
  fr = fr_open(argv[1]);
  s1 = fr_next(fr);
  seq_printlw(stdout,s1,70);
  s2 = seq_alloc();
  if (s2 == NULL) {
    fprintf(stderr,"s2 == null\n");
    exit(-1);
  }
  while ((s2 = fr_nextseq(fr,s2))) {
    seq_printlw(stdout,s2,70);
  }
  fr_close(fr);
  return 0;
}
