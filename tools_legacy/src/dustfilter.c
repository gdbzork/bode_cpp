/*
 * Fast dust filtering, version 1.0 by Gord Brown.
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
#include <ctype.h>

#include "fastareader.h"
#include "sequence.h"
#include "util.h"

double filterThreshold = 0.3;
int filterLength = 190;
int lineLength = 200;
char *dumpDustyOnes = NULL;
int filterN = 0;

static char opts[] = "nd:t:l:f:";
int readOptions(int argc,char **argv) {
  char c;
  while ((c = getopt(argc,argv,opts)) != -1) {
    switch (c) {
      case 'd': dumpDustyOnes = strdup(optarg);
                break;
      case 'f': filterLength = atoi(optarg);
                break;
      case 'l': lineLength = atoi(optarg);
                break;
      case 't': filterThreshold = atof(optarg);
                break;
      case 'n': filterN = 1;
                break;
    }
  }
  return optind;
}

int countLowerCase(char *str) {
  int slen;
  int i;
  int lower = 0;
   
  slen = strlen(str);
  for (i=0;i<slen;i++) {
    if (filterN) {
      if (str[i] == 'N') {
        lower++;
      }
    } else {
      if (islower(str[i])) {
        lower++;
        str[i] = toupper(str[i]);
      }
    }
  }
  return lower;
}

int trimTrailingPolyA(char *str) {
  int n;
  int trimmed = 0;
  n = strlen(str) - 1;
  while (str[n] == 'a') {
    str[n] = '\0';
    n--;
    trimmed++;
  }
  return trimmed;
}

int main(int argc,char **argv) {
  fastareader fdin;
  FILE *fdout,*dustyFD = NULL;
  sequence seq;
  char *data;
  int count,filter,ind,toolong,slen;
  int lowCount;
  double lowRatio;

  ind = readOptions(argc,argv);
  data = (char *) malloc(sizeof(char) * lineLength);
  seq = seq_alloc(lineLength);
  fdin = fr_open(argv[ind]);
  fdout = fopen(argv[ind+1],"w");
  if (dumpDustyOnes) {
    dustyFD = fopen(dumpDustyOnes,"w");
  }
  
  count = 0;
  filter = 0;
  toolong = 0;
  while (fr_nextseq(fdin,seq)) {
    stripWhiteSpace(seq->name);
    seq_nstring(data,seq,lineLength);
    stripWhiteSpace(data);
    slen = strlen(data);
    lowCount = trimTrailingPolyA(data);
    lowCount += countLowerCase(data);
    lowRatio = lowCount * 1.0 / slen;
    if (lowRatio > filterThreshold) {
      if (dustyFD) {
        seq_print(dustyFD,seq);
      }
      filter += 1;
    } else {
      if (strlen(data) > filterLength) {
        toolong++;
      } else {
        seq_toupper(seq);
        seq_print(fdout,seq);
      }
    }
    count++;
    if (count % 100000 == 0) {
      fprintf(stderr,"%9d (low=%9d long=%9d)\r",count,filter,toolong);
    }
  }
  fprintf(stderr,"read %d sequences, %d low quality, %d long\n",count,filter,toolong);
  return 0;
}
