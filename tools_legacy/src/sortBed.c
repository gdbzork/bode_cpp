#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>

#include "dictionary.h"
#include "pairlistAnnot.h"

#define LINELEN 1024

static char usage[] = "usage: sortBed <infile> <outfile>\n";

/***************************************************************/

/*
int chromNameComp(const void *a,const void *b) {
  int an = 0,bn = 0;
  char *ac,*bc;
  ac = (char *) *(char **)a;
  bc = (char *) *(char **)b;
  an = atoi(ac+3);
  bn = atoi(bc+3);
  if (an > 0 && bn > 0) {
    return (an < bn ? -1 : (an > bn ? 1 : 0));
  } else {
    return strcmp(ac,bc);
  }
}
*/

int chromNameComp(const void *a,const void *b) {
  int an = 0,bn = 0;
  char *ac,*bc;
  char *atrailing,*btrailing;
  ac = (char *) *(char **)a; /* void *a is actually char **, so get char * pointer from it */
  bc = (char *) *(char **)b;

  if (strncmp(ac,"chr",3) == 0) {
    an = strtol(ac+3,&atrailing,10);
  } else {
    an = strtol(ac,&atrailing,10);
  }
  if (strncmp(bc,"chr",3) == 0) {
    bn = strtol(bc+3,&btrailing,10);
  } else {
    bn = strtol(bc,&btrailing,10);
  }

  if (an > 0 && bn > 0) {
    if (an == bn) {
      return strcmp(atrailing,btrailing);
    } else {
      return (an < bn ? -1 : 1);
    }
  } else {
    return strcmp(ac,bc);
  }
}

void processBedLine(char *line,dictionary chromosomes) {
  char chrom[100];
  pairlistA pl;
  int left,right,tmp;
  int count;
  char *dup;
  if (!strncmp(line,"track",5)) {
    return;
  }
  count = sscanf(line,"%s %d %d ",chrom,&left,&right);
  if (count != 3) {
    fprintf(stderr,"WARNING: illegal line '%s' skipped",line);
    return;
  }
  if (!dict_contains(chromosomes,chrom)) {
    pl = pl_createA();
    dict_add(chromosomes,chrom,pl);
  } else {
    pl = dict_get(chromosomes,chrom);
  }
  if (left > right) {
    tmp = left;
    left = right;
    right = tmp;
  }
  dup = strdup(line);
  pl_addA(pl,left,right,dup);
}

void generateBed(FILE *fd,pairlistA pl) {
  int i;
  for (i=0;i<pl->index;i++) {
    fprintf(fd,pl->pairs[i].annot);
  }
}

void tidyString(char *s) {
  int fnlen,i;
  char *dotpos;
  char tmpname[LINELEN];
  char *basepos;
  strcpy(tmpname,s);
  basepos = basename(tmpname);
  strcpy(s,basepos);
  fnlen = strlen(s);
  if (!strcmp(s+fnlen-3,".gz")) {
    s[fnlen-3] = '\0';
    fnlen = fnlen - 3;
  }
  dotpos = strrchr(s,'.');
  if (dotpos != NULL) {
    *dotpos = '\0';
    fnlen = strlen(s);
  }
  for (i=0;i<fnlen;i++) {
    if (s[i] == '_') {
      s[i] = ' ';
    }
  }
}

/***************************************************************/

int main(int argc,char **argv) {
  dictionary chromosomes;
  char **chromnames;
  char *inFN,*outFN;
  FILE *inFD,*outFD;
  char inLine[LINELEN];
  int i,ccount;
  pairlistA pl;
  int total;
  void (*lineFunc)(char *line,dictionary chromosomes);

  if (argc != 3) {
    fprintf(stderr,usage);
  }
  inFN = argv[1];
  outFN = argv[2];

  chromosomes = dict_alloc("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

  lineFunc = &processBedLine; /* defaults to bed fmt... */
  inFD = fopen(inFN,"r");
  fgets(inLine,LINELEN,inFD);
  while (!feof(inFD)) {
    (*lineFunc)(inLine,chromosomes);
    fgets(inLine,LINELEN,inFD);
  }
  fclose(inFD);

  ccount = dict_count(chromosomes);
  chromnames = dict_keys(chromosomes);
  qsort(chromnames,ccount,sizeof(char *),&chromNameComp);
  total = 0;
  outFD = fopen(outFN,"w");
  for (i=0;i<ccount;i++) {
    pl = (pairlistA) dict_get(chromosomes,chromnames[i]);
    total += pl_countA(pl);
    pl_sortA(pl);
    generateBed(outFD,pl);
  }
  fprintf(stderr,"total\t%d\t%s\n",total,inFN);
  return 0;
}
