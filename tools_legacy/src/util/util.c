#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "util.h"
#include "fastq.h"

/* Odds and ends that are useful:
 *
 * char *stripWhiteSpace(char *str) -- strip white space from the beginning
 *                                     and end of a string, returning the
 *                                     passed-in pointer.  Assumes the
 *                                     string is null-terminated.
 *
 * void reverseComplementInPlace(char *str) -- reverse complements a
 *                                             null-terminated nucleotide
 *                                             string (paying attention to
 *                                             IUPAC ambiguity codes).
 *
 * void phredQStr2Int(int *dest,char *src) -- convert a null-terminated
 *                                            Phred-style quality string
 *                                            to integer scores.
 *
 * void phredScore2Prob(float *dest,int *src,int count) -- convert integer
 *                                                         Phred scores to
 *                                                         probabilities.
 * 
 * int split(char *src,char **dest,int max) -- split a string on white space,
 *                                             put results in "dest", split
 *                                             into at most "max" substrings.
 *                                             CAVEAT: destructively modifies
 *                                             "str".  Multiple spaces are
 *                                             considered one delimiter.
 *                                             Returns number of result strings.
 *
 * int splits(char *src,char **dest,int max) -- like split, but multiple 
 *                                              contiguous spaces are considered
 *                                              multiple delimiters.
 */

char *stripWhiteSpace(char *str) {
  int slen,start,end;

  slen = strlen(str);
  start = 0;
  while (start < slen && isspace(str[start])) {
    start++;
  }
  end = slen-1;
  while (end > start && isspace(str[end])) {
    end--;
  }
  str[end+1] = '\0';
  if (start > 0) {
    memmove(str,str+start,end-start+2);
  }
  return str;
}

void reverseComplementInPlace(char *str) {
  int slen,i;
  char tmp;

  if (str == NULL) {
    return;
  }
  slen = strlen(str);
  /* first complement, taking IUPAC ambiguity codes into account: */
  /* http://www.bioinformatics.org/SMS/iupac.html                 */
  for (i=0;i<slen;i++) {
    switch (str[i]) {
      case 'A': str[i] = 'T'; break;
      case 'a': str[i] = 't'; break;
      case 'C': str[i] = 'G'; break;
      case 'c': str[i] = 'g'; break;
      case 'G': str[i] = 'C'; break;
      case 'g': str[i] = 'c'; break;
      case 'T': str[i] = 'A'; break;
      case 't': str[i] = 'a'; break;
      case 'U': str[i] = 'A'; break;
      case 'u': str[i] = 'a'; break;
      case 'R': str[i] = 'Y'; break;
      case 'r': str[i] = 'y'; break;
      case 'Y': str[i] = 'R'; break;
      case 'y': str[i] = 'r'; break;
      case 'S': str[i] = 'W'; break;
      case 's': str[i] = 'w'; break;
      case 'W': str[i] = 'S'; break;
      case 'w': str[i] = 's'; break;
      case 'K': str[i] = 'M'; break;
      case 'k': str[i] = 'm'; break;
      case 'M': str[i] = 'K'; break;
      case 'm': str[i] = 'k'; break;
      case 'B': str[i] = 'V'; break;
      case 'b': str[i] = 'v'; break;
      case 'D': str[i] = 'H'; break;
      case 'd': str[i] = 'h'; break;
      case 'H': str[i] = 'D'; break;
      case 'h': str[i] = 'd'; break;
      case 'V': str[i] = 'B'; break;
      case 'v': str[i] = 'b'; break;
      default: break;
    }
  }

  /* now reverse */
  i = 0;
  slen--;
  while (i < slen) {
    tmp = str[i];
    str[i] = str[slen];
    str[slen] = tmp;
    i++;
    slen--;
  }
}

void phredQStr2Int(int *dest,char *src) {
  int i,n;
  n = strlen(src);
  for (i=0;i<n;i++) {
    dest[i] = (int) src[i] - 33;
  }
}

void phredScore2Prob(float *dest,int *src,int count) {
  int i;
  for (i=0;i<count;i++) {
    dest[i] = powf(10.0,-(float)src[i]/10.0);
  }
}

#define MAXLINELEN 1024
char scrap[MAXLINELEN];
int isFastaFile(char *fn) {
  FILE *fd;
  int rv = 1;
  fd = fopen(fn,"r");
  if (fd == NULL) {
    return 0;
  }
  fgets(scrap,MAXLINELEN,fd);
  stripWhiteSpace(scrap);
  if (scrap[0] != '>') {
    rv = 0; /* doesn't appear to be Fasta */
  }
  /* check a second header line, just to be sure... */
  fgets(scrap,MAXLINELEN,fd);
  fgets(scrap,MAXLINELEN,fd);
  stripWhiteSpace(scrap);
  if (scrap[0] != '>') {
    rv = 0; /* doesn't appear to be Fasta */
  }
  fclose(fd);
  return rv;
}

int isFastqFile(char *fn) {
  FILE *fd;
  int rv = 1;
  fd = fopen(fn,"r");
  if (fd == NULL) {
    return 0;
  }
  fgets(scrap,MAXLINELEN,fd);
  stripWhiteSpace(scrap);
  if (scrap[0] != '@') {
    rv = 0; /* doesn't appear to be Fastq */
  }
  /* check sub-header */
  fgets(scrap,MAXLINELEN,fd);
  fgets(scrap,MAXLINELEN,fd);
  stripWhiteSpace(scrap);
  if (scrap[0] != '+') {
    rv = 0; /* doesn't appear to be Fastq */
  }
  fgets(scrap,MAXLINELEN,fd);
  /* and check a second time to be sure... */
  fgets(scrap,MAXLINELEN,fd);
  stripWhiteSpace(scrap);
  if (scrap[0] != '@') {
    rv = 0; /* doesn't appear to be Fastq */
  }
  /* check sub-header */
  fgets(scrap,MAXLINELEN,fd);
  fgets(scrap,MAXLINELEN,fd);
  stripWhiteSpace(scrap);
  if (scrap[0] != '+') {
    rv = 0; /* doesn't appear to be Fastq */
  }
  fclose(fd);
  return rv;
}

int isRegularFile(char *fn) {
  struct stat sbuf;
  int statrc,rv;

  statrc = stat(fn,&sbuf);
  if (statrc) {
    perror("WARNING: isRegularFile");
    rv = 0;
  } else {
    rv = S_ISREG(sbuf.st_mode);
  }
  return rv;
}

int fileExists(char *fn) {
  struct stat sbuf;
  int statrc,rv = 0;

  statrc = stat(fn,&sbuf);
  if (statrc == 0) {
    rv = 1;
  } else {
    if (errno != ENOENT) {
      perror("WARNING: fileExists");
    }
  }
  return rv;
}

int split(char *str,char **dest,int max) {
  int index = 0,inspace;
  char *start = str;
  if (str == NULL) {
    return 0;
  }
  inspace = isspace(*str);
  while (*str != '\0' && index < max) {
    if (isspace(*str)) {
      if (!inspace) {
        dest[index++] = start;
        inspace = 1;
      }
      if (index < max) {
        *str = '\0';
      }
    } else {
      if (inspace) {
        start = str;
        inspace = 0;
      }
    }
    str++;
  }
  if (!inspace && index < max) {
    dest[index++] = start;
  }
  return index;
}

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
