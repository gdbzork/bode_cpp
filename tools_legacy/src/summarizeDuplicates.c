#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include "redblackSN.h"
#include "redblack.h"
#include "fastq.h"
#include "histogram.h"
#include "redblack.h"

#define UNUSED(x) (void)(x)
#define USAGE "usage: summarizeDuplicates [-s <singletons.fa] [-l <linkerseq>] [-n nnn] inputfile\n"
#define OPTS "n:l:qs:"

int clusterThreshold = 2000;
char *linkerSeq = NULL;
int quiet = 0;
char *singletonFN = NULL;
FILE *singletonFD = NULL;

typedef struct intbox {
  int n;
} *ibp;

ibp newIntbox() {
  ibp p;
  p = malloc(sizeof(struct intbox));
  p->n = 0;
  return p;
}

int readOptions(int argc, char **argv) {
  char c;
  while ((c = getopt(argc,argv,OPTS)) != -1) {
    switch (c) {
      case 'n': clusterThreshold = atoi(optarg);
                break;
      case 'l': linkerSeq = strdup(optarg);
                break;
      case 's': singletonFN = strdup(optarg);
                singletonFD = fopen(singletonFN,"w");
                break;
      case '?': fprintf(stderr,USAGE);
                exit(-1);
    }
  }
  return optind;
}

typedef struct histStruct {
  histogram h;
  redBlackTreeP rbt;
} *histStruct;

void makeHisto(char *key,int value,void *data) {
  histStruct hs = (histStruct) data;
  hist_increment(hs->h,value);
  if (value > clusterThreshold) {
    rb_insert(hs->rbt,value,key);
  }
  if (singletonFD != NULL) {
    fprintf(singletonFD,">%s\n%s\n",key,key);
  }
}

typedef struct dupStruct {
  int dups;
  int tot;
} *dupStructP;


void countDups(int i,int c,void *data) {
  dupStructP dp = (dupStructP) data;
  dp->tot += i*c;
  dp->dups += (i-1) * c;
}

void dumpHighRunners(int i,void *str,void *d) {
  UNUSED(d);
  printf("%d\t%s\n",i,(char *)str);
}

int main(int argc,char **argv) {
  FILE *inFD;
  fastqp fq;
  rbsnTree dict;
  histogram h;
  int current;
  int ind,i,reads,doublecheck,dupes,linkerCount,linkerLen=0;
  dupStructP dupdata;
  redBlackTreeP rbt;
  histStruct hstr;
  int newreads;
  int distinct;

  ind = readOptions(argc,argv);
  if (ind < argc) {
    inFD = fopen(argv[ind],"r");
    if (inFD == NULL) {
      printf("error %d %s\n",errno,strerror(errno));
      return 0;
    }
  } else {
    fprintf(stderr,USAGE);
    exit(-1);
  }

  fq = fq_alloc(1024);
  dict = rbsn_new();
  reads = 0;
  newreads = 0;
  linkerCount = 0;
  if (linkerSeq != NULL) {
    linkerLen = strlen(linkerSeq);
  }
  while (fq_next(fq,inFD)) {
    if (linkerSeq != NULL) {
      if (!strncmp(linkerSeq,fq->data,linkerLen)) {
        linkerCount++;
        reads++;
        continue;
      }
    }
    current = rbsn_search(dict,fq->data);
    if (current == 0) {
      rbsn_insert(dict,strdup(fq->data),0);
      newreads++;
    }
    rbsn_update(dict,fq->data,current+1);
    reads++;
    if (reads % 100000 == 0) {
      fprintf(stderr,"%d/%d\r",reads,newreads);
    }
  }

  h = hist_new();
  rbt = rb_new();
  i = 0;
  doublecheck = 0;
  dupes = 0;
  hstr = malloc(sizeof(struct histStruct));
  hstr->h = h;
  hstr->rbt = rbt;
  rbsn_traverse(dict,makeHisto,(void *)hstr,0);

  dupdata = malloc(sizeof(struct dupStruct));
  dupdata->dups = 0;
  dupdata->tot = 0;
  hist_traverse(h,countDups,dupdata);
  distinct = dupdata->tot - dupdata->dups;
  printf("count: %d\n",reads);
  if (linkerSeq != NULL) {
    printf("linker: %d  rate: %4.1f%%\n",linkerCount,linkerCount * 100.0 / reads);
    printf("nonlinker: %d  rate: %4.1f%%\n",dupdata->tot,dupdata->tot * 100.0 / reads);
    printf("distinct: %d  rate: %4.1f%% (of non-linker)\n",distinct,distinct * 100.0 / dupdata->tot);
    printf("duplicates: %d  rate: %4.1f%% (of non-linker)\n",dupdata->dups,dupdata->dups * 100.0 / dupdata->tot);
  } else {
    printf("distinct: %d  rate: %4.1f%%\n",distinct,distinct * 100.0 / dupdata->tot);
    printf("duplicates: %d  rate: %4.1f%%\n",dupdata->dups,dupdata->dups * 100.0 / dupdata->tot);
  }
  printf("histogram:\nclusterSize\tcount\n");
  hist_write(h);
  printf("high copy number reads:\n");
  rb_traverse(rbt,dumpHighRunners,NULL,1);
  if (singletonFD != NULL) {
    fclose(singletonFD);
  }
  return(0);
}