#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "fastq.h"

int countSeqs(char *fn) {
  FILE *fd;
  fastqp fq;
  int count;

  count = 0;
  fq = fq_alloc(1024);
  fd = fopen(fn,"r");
  while (fq_next(fq,fd)) {
    count++;
  }
  fclose(fd);
  return count;
}

int sample(int n,int t,char *in,char *out) {
  FILE *fdIN,*fdOUT;
  fastqp fq;
  int count,kept,nth;

  count = 0;
  kept = 0;
  nth = t / n;

  fq = fq_alloc(1024);
  fdIN = fopen(in,"r");
  fdOUT = fopen(out,"w");
  while (fq_next(fq,fdIN)) {
    count++;
    if (count % nth == 0) {
      fq_fastq(fq,fdOUT);
      kept++;
    }
    if (kept == n) {
      break;
    }
  }
  fclose(fdIN);
  fclose(fdOUT);
  return kept;
}

int main(int argc,char **argv) {
  int count,total,kept;
  char *infn,*outfn;
  clock_t start,middle,end;

  (void) argc;
  count = atoi(argv[1]);
  infn = strdup(argv[2]);
  outfn = strdup(argv[3]);

  start = clock();
  total = countSeqs(infn);
  middle = clock();
  fprintf(stderr,"count reads: %ld secs\n",(middle-start)/CLOCKS_PER_SEC);
  kept = sample(count,total,infn,outfn);
  end = clock();
  fprintf(stderr,"sample reads: %ld secs\n",(end-middle)/CLOCKS_PER_SEC);
  fprintf(stderr,"kept %d reads\n",kept);
  fprintf(stdout,"%d\t%d\n",total,kept);
  return 0;
}
