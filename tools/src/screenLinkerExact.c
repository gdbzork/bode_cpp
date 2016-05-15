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

#include "fastq.h"

#define LINELENGTH 1024 /* maximum line length          */
#define min(a,b) (a<b?a:b)

int minLengthToKeep = 16;
int verbose = 0;

int foundLinker(char *seq,char *link,char *linkhead,int *st) {
  int found = 0;
  char *headpos;
  int linklen,leftlen,n;
  linklen = strlen(link);
  headpos = strstr(seq,linkhead);
  while (headpos != NULL && !found) {
    leftlen = strlen(headpos);
    n = min(leftlen,linklen);
    if (!strncmp(headpos,link,n)) {
      found = 1;
      *st = headpos - seq;
      if (verbose) {
        fprintf(stderr,"%s\n%*s%s\n",seq,*st," ",link);
      }
    }
    headpos = strstr(headpos+1,linkhead);
  }
  return found;
}

char scrap[1024];
int isLinker(char *seq,char *link,char *linkhead,int *position) {
  int start;

  if (foundLinker(seq,link,linkhead,&start)) {
    if (start < minLengthToKeep) {
      return 1; /* it's all linker */
    } else {
      *position = start;
      return 0;
    }
  } else {
    *position = -1;
    return 0;
  }
}


/* read a fastq file, screen for linker (may do quality scoring, filtering
 * as well). */
void processFastq(char *linker, char *linkhead, FILE *input, FILE *good, 
                  int *cTrimmed, int *cUnTrimmed, int *cLinker) {
  fastqp seq;
  int count,position;

  seq = fq_alloc(LINELENGTH);
  count = 0;
  while (fq_next(seq,input)) {
    if (isLinker(seq->data,linker,linkhead,&position)) {
      (*cLinker)++;
    } else {
      if (position != -1) {
        fq_fastq_trunc(seq,good,position);
        (*cTrimmed)++;
      } else {
        (*cUnTrimmed)++;
      }
    }
    count++;
    if (count % 100000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
  }
  return;
}

int main(int argc,char **argv) {
  FILE *input,*good = NULL;
  int cTrimmed=0,cUnTrimmed=0,cLinker=0;
  char *linker = "TCGTATGCCGTCTTCTGCTTG";
  char *linkhead = "TCG";

  input = fopen(argv[1],"r");
  good = fopen(argv[2],"w");

  processFastq(linker,linkhead,input,good,&cTrimmed,&cUnTrimmed,&cLinker);
  fprintf(stdout,"trimmed=%d  untrimmed=%d  linker=%d\n",cTrimmed,cUnTrimmed,cLinker);
  return 0;
}
