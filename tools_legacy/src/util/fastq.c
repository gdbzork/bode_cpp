#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fastq.h"
#include "util.h"

fastqp fq_alloc(int maxlen) {
  fastqp fq;
  fq = (fastqp) malloc(sizeof(struct fastq));
  fq->id = (char *) malloc(maxlen+1);
  fq->data = (char *) malloc(maxlen+1);
  fq->qual = (char *) malloc(maxlen+1);
  fq->maxlen = maxlen;
  return fq;
}

int fq_next(fastqp seq,FILE *fd) {
  fgets(seq->id,seq->maxlen+1,fd);
  if (feof(fd)) {
    return 0;
  }
  seq->id[0] = ' ';
  stripWhiteSpace(seq->id);
  fgets(seq->data,seq->maxlen+1,fd);
  stripWhiteSpace(seq->data);
  fgets(seq->qual,seq->maxlen+1,fd); /* use qual buffer as temporary scratch
                                      * space to read 2nd header line */
  fgets(seq->qual,seq->maxlen+1,fd);
  stripWhiteSpace(seq->qual);
  return 1;
}

void fq_fastq(fastqp seq,FILE *fd) {
  fprintf(fd,"@%s\n%s\n+\n%s\n",seq->id,seq->data,seq->qual);
}

void fq_fastq_trunc(fastqp seq,FILE *fd,int len) {
  seq->data[len] = 0;
  seq->qual[len] = 0;
  fprintf(fd,"@%s\n%s\n+\n%s\n",seq->id,seq->data,seq->qual);
}

void fq_fasta(fastqp seq,FILE *fd) {
  fprintf(fd,">lcl|%s\n%s\n",seq->id,seq->data);
}

void fq_fasta_trunc(fastqp seq,FILE *fd,int len) {
  seq->data[len] = 0;
  fprintf(fd,">lcl|%s\n%s\n",seq->id,seq->data);
}

void fq_setQualityToZero(fastqp seq,int start,int length,int isSolexa) {
  int i;
  int quallen;
  int zero = 33;
  if (isSolexa) {
    zero = 64;
  }
  quallen = strlen(seq->qual);
  if (start >= quallen || start < 0) {
    return; /* can't start past the end of the sequence, or before the start */
  }
  if (length == 0 || /* default: from 'start' to end of sequence */
      start + length > quallen) { /* writing past end of sequence */
    length = quallen - start;
  } 
  for (i=start;i<start+length;i++) {
    seq->qual[i] = (char) zero;
  }
}

void fq_mask(fastqp seq,unsigned start,unsigned length,char mask) {
  unsigned i;
  unsigned seqlen;
  seqlen = strlen(seq->qual);
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

