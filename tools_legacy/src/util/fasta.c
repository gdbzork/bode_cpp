#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fasta.h"
#include "util.h"

fastap fa_alloc(int maxlen) {
  fastap fa;
  fa = (fastap) malloc(sizeof(struct fasta));
  fa->id = (char *) malloc(maxlen+1);
  fa->data = (char *) malloc(maxlen+1);
  fa->maxlen = maxlen;
  return fa;
}

int fa_next(fastap seq,FILE *fd) {
  fgets(seq->id,seq->maxlen+1,fd);
  if (feof(fd)) {
    return 0;
  }
  seq->id[0] = ' '; /* white out the ">" */
  stripWhiteSpace(seq->id); /* remove blank, newline */
  fgets(seq->data,seq->maxlen+1,fd);
  stripWhiteSpace(seq->data); /* remove newline */
  return 1;
}

void fa_fasta(fastap seq,FILE *fd) {
  fprintf(fd,">%s\n%s\n",seq->id,seq->data);
}

void fa_fasta_trunc(fastap seq,FILE *fd,int len) {
  seq->data[len] = 0;
  fprintf(fd,">%s\n%s\n",seq->id,seq->data);
}

void fa_free(fastap seq) {
  if (seq != NULL) {
    free(seq->id);
    free(seq->data);
    free(seq);
  }
}

void fa_mask(fastap seq,unsigned start,unsigned length,char mask) {
  unsigned i;
  unsigned seqlen;
  seqlen = strlen(seq->data);
  if (start >= seqlen) {
    return; /* can't start past the end of the sequence */
  }
  if (length == 0 || /* default: from 'start' to end of sequence */
      start + length > seqlen) { /* writing past end of sequence */
    length = seqlen - start;
  }
  for (i=start;i<start+length;i++) {
    seq->data[i] = mask;
  }
}
