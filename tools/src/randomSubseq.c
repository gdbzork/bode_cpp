#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define MAXSEQ 4096
#define FASTA_WIDTH 50

/******************************************************************************/

void stripTrailingSpace(char *str) {
  int end;

  end = strlen(str)-1;
  while (end >= 0 && isspace(str[end])) {
    end--;
  }
  str[end+1] = '\0';
}

char buffer[MAXSEQ];
int loadSeq(FILE *inFD,char *seqname,char *sequence) {
  if (buffer[0] != '\0') {
    strcpy(seqname,buffer);
  } else {
    fgets(seqname,MAXSEQ-1,inFD);
    stripTrailingSpace(seqname);
  }
  if (feof(inFD)) {
    return 0;
  }
  fgets(buffer,MAXSEQ-1,inFD);
  stripTrailingSpace(buffer);
  sequence[0] = '\0';
  while (buffer[0] != '>' && !feof(inFD)) {
    strcat(sequence,buffer);
    fgets(buffer,MAXSEQ-1,inFD);
    stripTrailingSpace(buffer);
  }
  return strlen(sequence);
}

int randomStart(maxStart) {
  return lrand48() % maxStart;
}

void dumpSeq(FILE *outFD,char *seqname,char *sequence,int offset,int len) {
  fprintf(outFD,"%s\n",seqname);
  sequence = sequence + offset;
  while (len > FASTA_WIDTH) {
    fwrite(sequence,sizeof(char),FASTA_WIDTH,outFD);
    fprintf(outFD,"\n");
    sequence += FASTA_WIDTH;
    len -= FASTA_WIDTH;
  }
  if (len > 0) {
    fwrite(sequence,sizeof(char),len,outFD);
    fprintf(outFD,"\n");
  }
}

/******************************************************************************/

int main(int argc,char **argv) {
  int subseqLen,seqLen,total,offset;
  char *inFN,*outFN;
  FILE *inFD,*outFD;
  char seqname[MAXSEQ],sequence[MAXSEQ];

  subseqLen = atoi(argv[1]);
  inFN = argv[2];
  outFN = argv[3];

  inFD = fopen(inFN,"r");
  outFD = fopen(outFN,"w");
  total = 0;
  srand48(clock());
  buffer[0] = '\0';

  while ((seqLen=loadSeq(inFD,seqname,sequence))) {
    offset = randomStart(seqLen-subseqLen+1);
    fprintf(stderr,"%d\n",offset);
    dumpSeq(outFD,seqname,sequence,offset,subseqLen);
    total++;
  }

  fclose(inFD);
  fclose(outFD);
  fprintf(stderr,"processed %d sequences.\n",total);
  return 0;
}
