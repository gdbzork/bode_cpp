#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>

#include "dictionary.h"
#include "fastq.h"
#include "histogram.h"
#include "redblack.h"

#define UNUSED(x) (void)(x)
#define USAGE "usage: summarizeDuplicates [-l <linkerseq>] [-n nnn] inputfile\n"
#define OPTS "n:l:q"

int clusterThreshold = 2000;
char *linkerSeq = NULL;
int quiet = 0;

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
      case '?': fprintf(stderr,USAGE);
                exit(-1);
    }
  }
  return optind;
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
  dictionary dict;
  histogram h;
  ibp current;
  char **keylist;
  int ind,i,reads,doublecheck,dupes,linkerCount,linkerLen=0;
  dupStructP dupdata;
  redBlackTreeP rbt;

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
  dict = dict_alloc("ACGTN");
  reads = 0;
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
    current = (ibp) dict_get(dict,fq->data);
    if (current == NULL) {
      current = newIntbox();
      dict_add(dict,fq->data,(void *) current);
    }
    current->n += 1;
    reads++;
  }

  h = hist_new();
  keylist = dict_keys(dict);
  rbt = rb_new();
  i = 0;
  doublecheck = 0;
  dupes = 0;
  while (keylist[i] != NULL) {
    current = dict_get(dict,keylist[i]);
    hist_increment(h,current->n);
    if (current->n >= clusterThreshold) {
      rb_insert(rbt,current->n,keylist[i]);
    }
    i++;
  }
  dupdata = malloc(sizeof(struct dupStruct));
  dupdata->dups = 0;
  dupdata->tot = 0;
  hist_traverse(h,countDups,dupdata);
  printf("count: %d\n",reads);
  printf("linker: %d  rate: %4.1f%%\n",linkerCount,linkerCount * 100.0 / reads);
  printf("nonlinker: %d  rate: %4.1f%%\n",dupdata->tot,dupdata->tot * 100.0 / reads);
  printf("duplicates: %d  rate: %4.1f%% (of non-linker)\n",dupdata->dups,dupdata->dups * 100.0 / dupdata->tot);
  printf("histogram:\nclusterSize\tcount\n");
  hist_write(h);
  printf("high copy number reads:\n");
  rb_traverse(rbt,dumpHighRunners,NULL,1);
  return(0);
}
