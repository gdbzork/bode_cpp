#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>

#include "fastq.h"
#include "quality.h"

static char *USAGE = "usage: solexa2phred [-c] fastq_in fastq_out";
static char *OPTS = "c";

int convert = 0;

int readOptions(int argc,char **argv) {
  char c;
  while ((c = getopt(argc,argv,OPTS)) != -1) {
    switch (c) {
      case 'c': convert = 1; break;
      case '?': fprintf(stderr,USAGE);
                exit(0);
    }
  }
  return optind;
}

int main(int argc,char **argv) {
  FILE *inFD,*outFD;
  fastqp seq = fq_alloc(1024); // number pulled out of thin air
  int seqlen,okay,ind;

  ind = readOptions(argc,argv);
  fprintf(stderr,"in: '%s'\n",argv[ind]);
  fprintf(stderr,"out: '%s'\n",argv[ind+1]);
  inFD = fopen(argv[ind],"r");
  outFD = fopen(argv[ind+1],"w");

  // first find out seq length
  okay = fq_next(seq,inFD);
  seqlen = strlen(seq->data);
  while (okay) {
    if (convert) {
      illumina2phred(seq->qual,seqlen);
    } else {
      fixOffset(seq->qual,seqlen);
    }
    fq_fastq(seq,outFD);
    okay = fq_next(seq,inFD);
  }
  fclose(inFD);
  fclose(outFD);
  return 0;
}
