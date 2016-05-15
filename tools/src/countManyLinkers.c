/*
 * Fast linker screening, version 1.0 by Gord Brown.
 *
 * This code is in the public domain.  Do whatever you like with it, but
 * please do not remove my name.  Unless a bug in the code causes a terrible
 * tragedy in which thousands perish.  Then please do remove my name.
 *
 * No guarantees, expressed or implied.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#include "calign.h"
#include "fastq.h"
#include "fasta.h"
#include "util.h"

#define LINELENGTH 1024 /* maximum line length          */
#define LINKERS 33 /* number of linker sequences */

int minLengthToKeep = 16;
int minLengthToCallLinker = 5;
int matchLen = 15;
float identity = 0.80;
int filterAvg = 13;
int filterPrefix = 25;
char *badOutputFile = NULL;
char *missingLinkerOutputFile = NULL;
int verbose = 0;
int overwrite = 0;
int justCount = 0;

void getLinker(char *fn,fastap *linkers) {
  FILE *lfd;
  int i;
  lfd = fopen(fn,"r");
  for (i=0;i<LINKERS;i++) {
    linkers[i] = fa_alloc(LINELENGTH);
    fa_next(linkers[i],lfd);
  }
  fclose(lfd);
  return;
}

/* read a fastq file, screen for linker (may do quality scoring, filtering
 * as well). */
int processFastq(fastap *linkers, FILE *input, int *counts) {
  fastqp seq;
  int count,position;
  int i;
  int mscore;
  int mpos;

  seq = fq_alloc(LINELENGTH);
  count = 0;
  while (fq_next(seq,input)) {
    mscore = 0;
    mpos = -1;
    for (i=0;i<LINKERS;i++) {
      int s = calignEndsFree(linkers[i]->data,seq->data);
      if (s > mscore) {
        mscore = s;
        mpos = i;
      }
    }
/*    fprintf(stdout,"%d\n",mscore); */
    if (mpos >= 0 && mscore > 50) {
      counts[mpos]++;
    }

    count++;
    if (count % 10000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
  }
  return count;
}

int main(int argc,char **argv) {
  FILE *input,*good,*bad = NULL,*missing=NULL;
  fastap *linkers;
  int *count;
  int ind,i;
  int total;
  int wantProgressMeter = 1;
  int cTrimmed=0,cUnTrimmed=0,cLinker=0,cQual=0;

  
  linkers = (fastap *) calloc(LINKERS,sizeof(fastap));
  getLinker(argv[1],linkers);
  count = (int *) calloc(LINKERS,sizeof(int));
  
  input = fopen(argv[2],"r");

  /* set up the alignment stuff */
  calignInit(LINELENGTH,2,-2,-6,-1);

  total = processFastq(linkers,input,count);
  good = fopen(argv[3],"w");
  for (i=0;i<LINKERS;i++) {
    fprintf(good,"%d\t%s\n",count[i],linkers[i]->id);
  }
  fprintf(good,"%d\ttotal\n",total);
  fclose(good);
  return 0;
}
