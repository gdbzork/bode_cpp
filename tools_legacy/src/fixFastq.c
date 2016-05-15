#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#include "fastq.h"

static char *USAGE = "usage: fixFastq fastq_in fastq_out";

int main(int argc,char **argv) {
  FILE *inFD,*outFD;
  fastqp seq = fq_alloc(1024); // number pulled out of thin air
  int okay;

  fprintf(stderr,"in: '%s'\n",argv[1]);
  fprintf(stderr,"out: '%s'\n",argv[2]);
  inFD = fopen(argv[1],"r");
  outFD = fopen(argv[2],"w");

  // first find out seq length
  okay = fq_next(seq,inFD);
  while (okay) {
    fq_fastq(seq,outFD);
    okay = fq_next(seq,inFD);
  }
  fclose(inFD);
  fclose(outFD);
  return 0;
}
