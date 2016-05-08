#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>

#include "bode/util.h"

char *bode::stripWhiteSpace(char *str) {
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

void bode::reverseComplementInPlace(char *str) {
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

void bode::phredQStr2Int(int *dest,char *src) {
  int i,n;
  n = strlen(src);
  for (i=0;i<n;i++) {
    dest[i] = (int) src[i] - 33;
  }
}

void bode::phredScore2Prob(float *dest,int *src,int count) {
  int i;
  for (i=0;i<count;i++) {
    dest[i] = powf(10.0,-(float)src[i]/10.0);
  }
}

#define MAXLINELEN 1024
char scrap[MAXLINELEN];
int bode::isFastaFile(char *fn) {
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

int bode::isFastqFile(char *fn) {
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

int bode::isRegularFile(char *fn) {
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

int bode::fileExists(char *fn) {
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

int bode::split(char *str,char **dest,int max) {
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

int bode::splitchar(char *str,char **dest,char c,int max) {
  int index = 0;
  char *start = str;
  if (str == NULL) {
    return 0;
  }
  while (*str != '\0' && index < max) {
    if (*str == c) {
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

int bode::splits(char *str,char **dest,int max) {
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

std::vector<std::string> *bode::splitV(std::string const &str,char c) {
  std::vector<std::string> *v;
  int index = 0;
  int start = 0;
  int max = str.length();
  v = new std::vector<std::string>();
  
  while (index < max) {
    if (str[index] == c) {
      v->push_back(str.substr(start,index-start));
      start = index+1;
    }
    index++;
  }
  if (index == max) {
    v->push_back(str.substr(start,index-start));
  }
  return v;
}

char md5exec[] = "/usr/bin/md5sum";
std::string bode::md5(std::string const &filename) {
  std::string md5;
  FILE *out;
  char cmd[1024];
  char result[1024];
  char resfn[1024];

  sprintf(cmd,"%s %s",md5exec,filename.c_str());
  out = popen(cmd,"r");
  fscanf(out,"%s %s",result,resfn);
  pclose(out);
  md5 = std::string(result);
  return md5;
}

void bode::toLower(std::string &str) {
  int slen = str.length();
  for (int i=0;i<slen;i++) {
    str[i] = tolower(str[i]);
  }
}

bode::ByteOrder bode::getByteOrder(void) {
  union {
    long l;
    char c[4];
  } test;
  test.l = 1;
  if( test.c[3] && !test.c[2] && !test.c[1] && !test.c[0] )
    return bode::BIGENDIAN;
  if( !test.c[3] && !test.c[2] && !test.c[1] && test.c[0] )
    return bode::LITTLEENDIAN;
  return bode::UNKNOWN_ENDIAN;
}

