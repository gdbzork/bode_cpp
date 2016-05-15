#ifndef FASTAREADER_H
#define FASTAREADER_H

#include <stdio.h>

#include "sequence.h"

typedef struct fastareader {
  FILE *file;
  char *filename;
  char *linebuffer;
  int count;
  int lineno;
} *fastareader;

fastareader fr_open(char *filename);
void fr_close(fastareader fr);
sequence fr_next(fastareader fr);
sequence fr_nextseq(fastareader fr,sequence seq);
int fr_eof(fastareader fr);

#endif
