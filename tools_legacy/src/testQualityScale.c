#include <stdio.h>
#include <string.h>

#include "fastq.h"
#include "quality.h"

void checkNReads(int count,FILE *fd,int *phredOffset,int *phredScaled) {
  int i,slen,isPO,isPS;
  fastqp seq;

  seq = fq_alloc(1024);
  for (i=0;i<count;i++) {
    if (fq_next(seq,fd)) {
      slen = strlen(seq->qual);
      isPO = isPhredOffset(seq->qual,slen);
      isPS = isPhredScaled(seq->qual,slen,isPO);
      *phredOffset += isPO ? 1 : 0;
      *phredScaled += isPS ? 1 : 0;
    }
  }
  return;
}

int main(int argc,char** argv) {
  char *fn;
  FILE *fd;
  int po,ps;

  po = 0;
  ps = 0;
  fn = argv[1];
  fd = fopen(fn,"r");
  checkNReads(10000,fd,&po,&ps);
  fprintf(stderr,"%s\tpo=%d\tps=%d\n",fn,po,ps);
  fclose(fd);
  return 0;
}
