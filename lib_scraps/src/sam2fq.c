#include <stdio.h>
#include <string.h>

#define LINELENGTH 2048
#define FLDLEN 128

int splits(char *str,char **dest,int max) {
  int index = 0;
  char *start = str;
  if (str == NULL) {
    return 0;
  }
  while (*str != '\0' && index < max) {
    if (isspace(*str)) {
      dest[index++] = start;
      start = str+1;
      if (index < max) {
        *str = '\0';
      }
    }
    str++;
  }
  if (index < max) {
    dest[index++] = start;
  }
  return index;
}

int parseSam(char *line,char *name,char *seq) {
  char *dest[FLDLEN];
  int fldcount;

  fldcount = splits(line,dest,FLDLEN);
  if (fldcount < 10) {
    fprintf(stderr,"Bad field count on string '%s': n=%d\n",line,fldcount);
    return(1);
  }
  strcpy(name,dest[0]);
  strcpy(seq,dest[9]);
  return(0);
}

void makeQual(char *seq,char *qual) {
  int i,slen;

  slen = strlen(seq);
  for (i=0;i<slen;i++) {
    if (seq[i] == 'N') {
      qual[i] = '!';
    } else {
      qual[i] = '.';
    }
  }
  qual[slen] = '\0';
}

int main(int argc,char **argv) {
  FILE *inFD, *outFD;
  int total;
  char inLine[LINELENGTH];
  char name[FLDLEN],seq[FLDLEN],qual[FLDLEN];

  inFD = fopen(argv[1],"r");
  outFD = fopen(argv[2],"w");

  total = 0;
  while (fgets(inLine,LINELENGTH,inFD) != NULL) {
    total++;
    if (inLine[0] != '@') {
      parseSam(inLine,name,seq);
      makeQual(seq,qual);
      fprintf(outFD,"@%s\n%s\n+\n%s\n",name,seq,qual);
    }
  }
  fprintf(stdout,"%d\n",total);
  return(0);
}
