#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#include "sequence.h"

#define BLOCKSIZE 1024
#define BLOCKSPACE 20
#define DEFAULTWIDTH 60

#define min(a,b) ((a)<(b)?(a):(b))

sequence seq_alloc_real(unsigned blockSpace,unsigned blockSize) {
  sequence seq;

  assert(blockSpace > 1);
  assert(blockSize > 1);

  seq = (sequence) malloc(sizeof(struct seqstruct));
  if (seq == NULL) {
    return NULL;
  }
  seq->name = (char *) malloc(blockSize+1);
  if (seq->name == NULL) {
    return NULL;
  }
  seq->namelen = blockSize;
  seq->name[0] = 0;
  seq->data = calloc(blockSpace+1,sizeof(char *));
  seq->position = calloc(blockSpace+1,sizeof(unsigned));
  seq->data[0] = (char *) malloc(blockSize+1);
  seq->position[0] = 0;
  seq->position[1] = 0;

  seq->blocksPossible = blockSpace;
  seq->blocksAllocated = 1;
  seq->blocksUsed = 1;
  seq->blocksize = blockSize;
  seq->length = 0;
  return seq;
}

sequence seq_alloc() {
  return seq_alloc_real(BLOCKSPACE,BLOCKSIZE);
}

sequence seq_allocn(unsigned size) {
  unsigned bspace;
  bspace = (unsigned) ceil(((float)size) / BLOCKSIZE);
  return seq_alloc_real(bspace,BLOCKSIZE);
}

sequence seq_allocbs(unsigned blockSize) {
  assert(blockSize > 0);
  return seq_alloc_real(BLOCKSPACE,blockSize);
}

sequence seq_allocnbs(unsigned size,unsigned blockSize) {
  unsigned bspace;
  bspace = (int) ceil(((float)size) / blockSize);
  return seq_alloc_real(bspace,BLOCKSIZE);
}

void seq_free(sequence seq) {
  unsigned i;
  if (seq == NULL) {
    return;
  }
  free(seq->name);
  free(seq->position);
  for (i=0;i<seq->blocksAllocated;i++) {
    free(seq->data[i]);
  }
  free(seq->data);
  free(seq);
  return;
}

void seq_clear(sequence seq) {
  seq->length = 0;
  seq->name[0] = 0;
  seq->blocksUsed = 1;
  seq->position[0] = 0;
  seq->position[1] = 0;
}

sequence doubleBlockSpace(sequence seq) {
  char **tmpData = NULL;
  unsigned *tmpPosition = NULL;

  assert(seq != NULL);

  tmpData = (char **) realloc(seq->data,(seq->blocksPossible*2+1)*sizeof(char *));
  tmpPosition = realloc(seq->position,(seq->blocksPossible*2+1)*sizeof(unsigned));
  if (tmpData == NULL || tmpPosition == NULL) {
    free(tmpData);
    free(tmpPosition);
#ifdef DEBUG
    fprintf(stderr,"WARNING: failed to double block space: seq->bP=%d",seq->blocksPossible);
    fflush(stderr);
#endif
    return NULL;
  }
  seq->data = tmpData;
  seq->position = tmpPosition;
  seq->blocksPossible = seq->blocksPossible*2;
  return seq;
}
 
sequence seq_ensureCapacity(sequence seq,unsigned size) {
  unsigned actualCapacity,maxCapacity;
  sequence tmp;

  assert(seq != NULL);

  maxCapacity = seq->blocksPossible * seq->blocksize;
  while (maxCapacity < size) {
    tmp = doubleBlockSpace(seq);
    if (tmp == NULL) {
      return NULL;
    }
    maxCapacity = seq->blocksPossible * seq->blocksize;
  }

  actualCapacity = seq->length + 
        (seq->blocksize - (seq->position[seq->blocksUsed] -
                           seq->position[seq->blocksUsed - 1])) +
        (seq->blocksAllocated - seq->blocksUsed) * seq->blocksize;
  while (actualCapacity < size) {
    /* allocate some more blocks, until actual >= size */
    
    seq->data[seq->blocksAllocated] = (char *) malloc(seq->blocksize+1);
    if (seq->data[seq->blocksAllocated] == NULL) {
#ifdef DEBUG
      fprintf(stderr,"WARNING: failed to allocate block: seq->bA=%d",seq->blocksAllocated);
      seq_dump(stderr,seq);
      fflush(stderr);
#endif
      return NULL;
    }
    seq->blocksAllocated += 1;
    actualCapacity += seq->blocksize;
  }
  return seq;
}

sequence seq_setName(sequence seq,char *name) {
  unsigned nlen;
  char *tmp;

  assert(name != NULL);
  assert(seq != NULL);
  assert(seq->namelen >= seq->blocksize);

  nlen = strlen(name);
  if (seq->namelen < nlen) {
    tmp = (char *) malloc(nlen+1);
    if (tmp == NULL) {
      return NULL;
    }
    free(seq->name);
    seq->name = tmp;
    seq->namelen = nlen;
  }
  strcpy(seq->name,name);
  return seq;
}

char *seq_getName(sequence seq) {
  assert(seq != NULL);
  assert(seq->name != NULL);

  return strdup(seq->name);
}

unsigned blockContainingPosition(sequence seq,unsigned position) {
  unsigned block,current;

  assert(seq != NULL);
  assert(position < seq->length);

  block = 1;
  current = seq->position[block];
  while (current <= position) {
    block++;
    current = seq->position[block];
  }
  return block-1;
}

sequence seq_append(sequence seq,char *data,unsigned datalen) {
  unsigned savedlen,block,blockPos,room,dataleft,chunklen;

  seq_ensureCapacity(seq,seq->length+datalen);

  savedlen = 0;
  block = seq->blocksUsed - 1;
  blockPos = seq->position[block+1] - seq->position[block];

  /* fill successive blocks */
  while (savedlen < datalen) {
    room = seq->blocksize - blockPos;
    if (room == 0) {
      block++;
      seq->blocksUsed++;
      blockPos = 0;
      room = seq->blocksize;
      seq->position[block+1] = seq->position[block];
    }
    dataleft = datalen - savedlen;
    chunklen = min(room,dataleft);
    strncpy(seq->data[block]+blockPos,data+savedlen,chunklen);
    blockPos += chunklen;
    seq->data[block][blockPos] = 0; /* ensure it's null-terminated */
    seq->position[block+1] += chunklen;
    seq->length += chunklen;
    savedlen += chunklen;
  }
  return seq;
}

static void str_toupper(char *str) {
  int i,slen;
  slen = strlen(str);
  for (i=0;i<slen;i++) {
    if (islower(str[i])) {
      str[i] = toupper(str[i]);
    }
  }
}

void seq_toupper(sequence seq) {
  unsigned i;
  for (i=0;i<seq->blocksUsed;i++) {
    str_toupper(seq->data[i]);
  }
}

void seq_print(FILE * fd,sequence seq) {
  seq_printlw(fd,seq,DEFAULTWIDTH);
}

void seq_printlw(FILE *fd,sequence seq,unsigned lineWidth) {
  unsigned block,blockPos,blockWidth,leftInLine,leftInBlock;

  fprintf(fd,">%s\n",seq->name);
  block = 0;
  blockPos = 0;
  blockWidth = seq->position[1];
  leftInLine = lineWidth;
  while (block < seq->blocksUsed) {
    if (leftInLine <= blockWidth - blockPos) {
      /* i.e. we're about to reach the end of the output line */
      fprintf(fd,"%.*s\n",leftInLine,seq->data[block]+blockPos);
      blockPos += leftInLine;
      leftInLine = lineWidth;
    } else {
      /* print rest of this block, then must fill in line */
      leftInBlock = blockWidth - blockPos;
      fprintf(fd,"%s",seq->data[block]+blockPos);
      block++;
      blockWidth = seq->position[block+1] - seq->position[block];
      blockPos = 0;
      leftInLine = leftInLine - leftInBlock;
    }
  }
  if (leftInLine != lineWidth) {
    fprintf(fd,"\n");
  }
}

sequence seq_substr(sequence dest,sequence src,unsigned start,unsigned length) {
  unsigned block,blockPos,copied,chunkLen;
  char *name;

  name = (char *) malloc((dest->namelen+1)*sizeof(char));
  seq_ensureCapacity(dest,length);
  seq_clear(dest);
  block = blockContainingPosition(src,start);
  blockPos = start - src->position[block];
  copied = 0;
  while (copied < length) {
    chunkLen = min(length-copied,src->blocksize-blockPos);
    seq_append(dest,src->data[block]+blockPos,chunkLen);
    copied += chunkLen;
    block += 1;
    blockPos = 0;
  }
  sprintf(name,"%s [%d,%d]",src->name,start,start+length);
  seq_setName(dest,name);
  free(name);
  return dest;
}

char *seq_string(sequence seq) {
  char *str;
  str = (char *) malloc(sizeof(char)*seq->length+1);
  if (str != NULL) {
    str = seq_nstring(str,seq,seq->length+1);
  }
  return str;
}

char *seq_nstring(char *dest,sequence src,unsigned space) {
  unsigned length,current,block,blockLen;
  length = min(src->length,space-1);
  current = 0;
  block = 0;
  while (current < length) {
    blockLen = src->position[block+1] - src->position[block];
    strncpy(dest+current,src->data[block],blockLen);
    current += blockLen;
    block++;
  }
  dest[current] = 0;
  return dest;
}

void seq_dump(FILE *fd,sequence seq) {
  unsigned i;
  fprintf(fd,"sequence '%s' (namelen=%d)\n",seq->name,seq->namelen);
  fprintf(fd,"  blocksize       = %d\n",seq->blocksize);
  fprintf(fd,"  blocksPossible  = %d\n",seq->blocksPossible);
  fprintf(fd,"  blocksAllocated = %d\n",seq->blocksAllocated);
  fprintf(fd,"  blocksUsed      = %d\n",seq->blocksUsed);
  fprintf(fd,"  length          = %d\n",seq->length);
  for (i=0;i<seq->blocksUsed;i++) {
    fprintf(fd,"%8d %s\n",seq->position[i],seq->data[i]);
  }
  fprintf(fd,"%8d\n",seq->position[seq->blocksUsed]);
}

void seq_assertSanity(sequence seq) {
  unsigned i,total,blocklen;
  assert(seq != NULL);
  assert(seq->name != NULL);
  assert(seq->blocksize > 0);
  assert(seq->namelen >= seq->blocksize);
  assert(strlen(seq->name) <= seq->namelen);
  assert(seq->blocksUsed <= seq->blocksAllocated);
  assert(seq->blocksAllocated <= seq->blocksPossible);
  assert(seq->data != NULL);
  assert(seq->position != NULL);
  assert(seq->position[0] == 0);
  total = 0;
  for (i=0;i<seq->blocksUsed;i++) {
    assert(total == seq->position[i]);
    blocklen = strlen(seq->data[i]);
    assert(blocklen <= seq->blocksize);
    total += blocklen;
  }
  assert(total == seq->position[seq->blocksUsed]);
  assert(total == seq->length);
}
