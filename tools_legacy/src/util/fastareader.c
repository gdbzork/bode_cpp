#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "sequence.h"
#include "fastareader.h"

#define LINEBUFFERSIZE 2048

fastareader fr_open(char *filename) {
  fastareader fr;

  fr = (fastareader) malloc(sizeof(struct fastareader));
  if (fr == NULL) {
    return NULL;
  }
  fr->linebuffer = (char *) malloc((LINEBUFFERSIZE) * sizeof(char));
  if (fr->linebuffer == NULL) {
    free(fr);
    return NULL;
  }
  fr->linebuffer[0] = 0;
  fr->count = 0;

  fr->file = fopen(filename,"r");
  if (fr->file == NULL) {
    free(fr->linebuffer);
    free(fr);
    return NULL;
  }
  fr->filename = strdup(filename);
  return fr;
}

void fr_close(fastareader fr) {
  fclose(fr->file);
  free(fr->filename);
  free(fr->linebuffer);
  free(fr);
}

sequence fr_next(fastareader fr) {
  sequence seq;

  seq = seq_alloc();
  if (seq == NULL) {
    return NULL;
  }
  return fr_nextseq(fr,seq);
}

sequence fr_nextseq(fastareader fr,sequence seq) {
  char *cline;
  if (feof(fr->file)) {
    return NULL;
  }
  seq_clear(seq);
  
  while (strlen(fr->linebuffer) == 0) {
    cline = fgets(fr->linebuffer,LINEBUFFERSIZE,fr->file);
    if (cline == NULL) {
      /* let's hope that's EOF, not error */
      return NULL;
    }
    stripWhiteSpace(fr->linebuffer);
    fr->lineno++;
  }
  if (fr->linebuffer[0] != '>') {
    /* syntax error, apparently... */
    fprintf(stderr,"syntax error reading '%s', line %d\n",
                   fr->filename,fr->lineno);
    return NULL;
  }
  seq_setName(seq,fr->linebuffer+1);

  fgets(fr->linebuffer,LINEBUFFERSIZE,fr->file);
  stripWhiteSpace(fr->linebuffer);
  while (!feof(fr->file) && fr->linebuffer[0] != '>') {
    seq_append(seq,fr->linebuffer,strlen(fr->linebuffer));
    fgets(fr->linebuffer,LINEBUFFERSIZE,fr->file);
    stripWhiteSpace(fr->linebuffer);
  }
    
  return seq;
}

int fr_eof(fastareader fr) {
  return feof(fr->file);
}
