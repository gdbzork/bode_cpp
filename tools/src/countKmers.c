#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 1024

int K;
char cmap[] = {'A','C','G','T'};

int ipow(int base,int exp) {
  int i,res;
  res = 1;
  if (exp==0) {
    return 1;
  }
  for (i=0;i<exp;i++) {
    res *= base;
  }
  return res ;
}

char *i2t(int i,char *buffer) {
  int j;
  for (j=K-1;j>=0;j--) {
    int r = i % 4;
    buffer[j] = cmap[r];
    i = i / 4;
  }
  buffer[K] = '\0';
  return buffer;
}

int t2i(int *digits) {
  int i,n;
  n = digits[0];
  for (i=1;i<K;i++) {
    n = n * 4 + digits[i];
  }
  return n;
}

int read2ints(char *buffer,int blen,int *ints) {
  int i,okay;
  okay = 1;
  for (i=0;i<blen;i++) {
    switch (buffer[i]) {
      case 'A': ints[i] = 0; break;
      case 'C': ints[i] = 1; break;
      case 'G': ints[i] = 2; break;
      case 'T': ints[i] = 3; break;
      default:  okay = 0; /* discard any read with non-ACGT */
    }
  }
  return okay;
}

int countKmers(char *buffer,long *counts,int *conv) {
  int i,slen,top,used;
  slen = strlen(buffer);
  buffer[slen-1] = '\0'; /* remove trailing newline */
  slen--;
  top = slen - K;
  if (read2ints(buffer,slen,conv)) {
    for (i=0;i<top;i++) {
      int n = t2i(conv+i);
      counts[n]++;
    }
    used = 1;
  } else {
    used = 0;
  }
  return used;
}

int main(int argc,char **argv) {
  char *inFN,*outFN;
  FILE *inFD,*outFD;
  long *counts;
  int wtop,readcount,used,i;
  char buffer[MAXLINE];
  int conv[MAXLINE];

  K = atoi(argv[1]);
  inFN = argv[2];
  outFN = argv[3];
  inFD = fopen(inFN,"r");
  wtop = ipow(4,K);
  counts = (long *) calloc(wtop,sizeof(long));

  readcount = 0;
  used = 0;
  while (!feof(inFD)) {
    fgets(buffer,MAXLINE,inFD);
    if (feof(inFD)) {
      continue;
    }
    fgets(buffer,MAXLINE,inFD);
    used += countKmers(buffer,counts,conv);
    fgets(buffer,MAXLINE,inFD);
    fgets(buffer,MAXLINE,inFD);
    readcount++;
  }
  fclose(inFD);
  fprintf(stderr,"%d of %d reads\n",used,readcount);

  outFD = fopen(outFN,"w");
  for (i=0;i<wtop;i++) {
    fprintf(outFD,"%ld\t%s\n",counts[i],i2t(i,buffer));
  }
  fclose(outFD);
  return 0;
}
