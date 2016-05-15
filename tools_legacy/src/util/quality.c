#include <stdio.h>
#include <string.h>
#include <math.h>

#include "quality.h"

#define PHRED_OFFSET 33
#define ILLUMINA_OFFSET 64

#define ILLUMINA_SCORE_TO_PROB(s) (1.0-(1.0/(1.0+pow(10.0,((double)(s)/10.0)))))
#define PHRED_SCORE_TO_PROB(s) (1.0-pow(10.0,(-((double)(s))/10.0)))
#define PROB_TO_PHRED_SCORE(p) ((int) -(10.0 * log10(1.0-(p))))

int isPhredOffset(char *src,int n) {
  int i,okay,s;
  okay = 0;
  for (i=0;i<n;i++) {
    s = src[i] - PHRED_OFFSET;
    if (s < 40 && s > 0) {
      okay++;
    }
  }
  return okay == n;
}

int isPhredScaled(char *src,int n,int isPhredOffset) {
  int i,okay,offset;
  double p;
  offset = (isPhredOffset ? PHRED_OFFSET : ILLUMINA_OFFSET);
  okay = 0;
  for (i=0;i<n;i++) {
    p = PHRED_SCORE_TO_PROB(src[i]-offset);
/*    fprintf(stderr,"%c\t%d\t%f\t%f\n",src[i],src[i]-offset,p,ILLUMINA_SCORE_TO_PROB(src[i]-offset)); */
    if (p >= 0.0 && p <= 1.0) {
      okay++;
    }
  }
  return okay == n;
}

void fixOffset(char *src,int n) {
  int i,off;
  off = ILLUMINA_OFFSET - PHRED_OFFSET;
  for (i=0;i<n;i++) {
    src[i] = src[i] - off;
  }
  return;
}

void illumina2phred(char *src,int n) {
  int i,s,q;
  double p;
  for (i=0;i<n;i++) {
    s = src[i] - ILLUMINA_OFFSET;
    p = ILLUMINA_SCORE_TO_PROB(s);
    q = PROB_TO_PHRED_SCORE(p);
    src[i] = q + PHRED_OFFSET;
  }
}
