#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <ctype.h>
#include <libgen.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "dictionary.h"
#include "pairlist.h"
#include "util.h"

/* #define DEBUG 1 */

#define UNUSED(x) (void)(x)

#define UNKFMT 0
#define BEDFMT 1
#define GFFFMT 2
#define MAQFMT 3
#define BOWFMT 4
#define SUGFMT 5

#define COMPUNKNOWN -1
#define COMPNO 0
#define COMPYES 1

#define WIGGLE 0
#define BEDGRAPH 1

#define LINELEN 1024
#define THRESHOLD 1
#define INTERVAL 10
#define ROWS_PER_FILE 2000000
#define MAPPING_QUAL_THRESHOLD 0
#define FLOATINGPOINT 0

static char usage[] = "usage: makeWiggle [-q nn] [-b|-g|-m|-o|-s] [-r nn] [-1] [-c] [-t nn] [-i nn] [-a] [-B] [-N nn] [-f] [-3|-5] [-k karyotypeFile] <infile> <outfile>\n";
static char opts[] = "abBgm1scot:i:d:n:r:q:N:f35k:";

static char SUFFIX[][5] = {".wig",".bgr"};
static char TRACK[][9] = {"wiggle_0","bedGraph"};

/***************************************************************/

int readOptions(int argc,char **argv,int *threshold,
                                     int *interval,
                                     int *ftype,
                                     int *compressed,
                                     char *name,
                                     char *description,
                                     int *oneFile,
                                     int *affyFmt,
                                     int *rowsPerFile,
                                     int *outFmt,
                                     int *mappingQualThreshold,
                                     int *normalize,
                                     int *floatingpoint,
                                     int *strand,
                                     char *karyoFile) {
  char c;
  *threshold = THRESHOLD;
  *interval = INTERVAL;
  *compressed = COMPUNKNOWN;
  *oneFile = 0;
  *ftype = UNKFMT;
  *rowsPerFile = ROWS_PER_FILE;
  *mappingQualThreshold = MAPPING_QUAL_THRESHOLD;
  name[0] = '\0';
  description[0] = '\0';
  *outFmt = WIGGLE;
  *floatingpoint = FLOATINGPOINT;
  *strand = 0;
  karyoFile[0] = '\0';
  while ((c = getopt(argc,argv,opts)) != -1) {
    switch (c) {
      case 'b': *ftype = BEDFMT; break;
      case 'g': *ftype = GFFFMT; break;
      case 'm': *ftype = MAQFMT; break;
      case 'o': *ftype = BOWFMT; break;
      case 's': *ftype = SUGFMT; break;
      case 'c': *compressed = 1; break;
      case '1': *oneFile = 1; break;
      case 't': *threshold = atoi(optarg); break;
      case 'i': *interval = atoi(optarg); break;
      case 'r': *rowsPerFile = atoi(optarg); break;
      case 'n': strcpy(name,optarg); break;
      case 'd': strcpy(description,optarg); break;
      case 'a': *affyFmt = 1; break;
      case 'B': *outFmt = BEDGRAPH; break;
      case 'q': *mappingQualThreshold = atoi(optarg); break;
      case 'N': *normalize = atoi(optarg); break;
      case 'f': *floatingpoint = 1; break;
      case '3': *strand = -1; break;
      case '5': *strand = 1; break;
      case 'k': strcpy(karyoFile,optarg); break;
      case '?': fprintf(stderr,usage); exit(0);
      default:  fprintf(stderr,usage); exit(0);
    }
  }
  return optind;
}

int isCompressed(char *fn) {
  int fnlen;
  fnlen = strlen(fn);
  return strcmp(fn+fnlen-3,".gz") == 0;
}

int guessFileType(char *fn,int compressed) {
  int fnlen;
  char *suffix;
  int ftype;

  fnlen = strlen(fn);
  if (compressed) {
    fn[fnlen-3] = '\0';
  }
  suffix = strrchr(fn,'.');
  if (suffix != NULL) {
    if (!strncmp(suffix,".bed",4)) {
      ftype = BEDFMT;
    } else if (!strncmp(suffix,".gff",4)) {
      ftype = GFFFMT;
    } else if (!strncmp(suffix,".map",4)) {
      ftype = MAQFMT;
    } else if (!strncmp(suffix,".bow",4)) {
      ftype = BOWFMT;
    } else {
      ftype = 0;
    }
  } else {
    ftype = 0;
  }
  if (compressed) {
    fn[fnlen-3] = '.';
  }
  return ftype;
}

/*
static char outFN[1024];
char *makeOutputFilename(char *inFN) {
  int fnlen;
  char *dotpos;
  fnlen = strlen(inFN);
  strcpy(outFN,inFN);
  if (!strcmp(outFN+fnlen-3,".gz")) {
    outFN[fnlen-3] = '\0';
    fnlen = fnlen - 3;
  }
  dotpos = strrchr(outFN,'.');
  if (dotpos != NULL) {
    *dotpos = '\0';
  }
  strcat(outFN,".wig");
  fprintf(stderr,"%s  -->  %s",inFN,outFN);
  return outFN;
}
*/

int chromNameComp(const void *a,const void *b) {
  int an = 0,bn = 0;
  char *ac,*bc;
  ac = (char *) *(char **)a;
  bc = (char *) *(char **)b;
  an = atoi(ac+3);
  bn = atoi(bc+3);
  if (an > 0 && bn > 0) {
    return (an < bn ? -1 : (an > bn ? 1 : 0));
  } else {
    return strcmp(ac,bc);
  }
}

int allDigits(char *str) {
  while (*str != '\0' && isdigit(*str)) {
    str++;
  }
  return (*str == '\0');
}

int countOccurrences(char *name) {
  /* if name ends in "_xNNN" for digits NNN, return the number, else
     return 1. */
  char *underscore;
  int number;

  if (allDigits(name)) {
    number = strtol(name,NULL,10);
  } else {
    underscore = strrchr(name,'_');
    if (underscore == NULL || underscore[1] != 'x') {
      number = 1;
    } else if (allDigits(underscore+2)) {
      number = strtol(underscore+2,NULL,10);
    } else {
      number = 1;
    }
  }
  return number;
}

void processBedLine(char *line,dictionary chromosomes,int mapQualThr,int strand) {
  char *chrom;
  char *name;
  char *fields[6];
  pairlist pl;
  int left,right,tmp,occurrences,mapqual,count;
  if (!strncmp(line,"track",5)) {
    return;
  }
  count = split(line,fields,6);
  chrom = fields[0];
  left = atoi(fields[1]);
  right = atoi(fields[2]);
  if (count >= 4) {
    name = fields[3];
    occurrences = countOccurrences(name);
  } else {
    occurrences = 1;
  }
  if (count >= 5 ) {
    mapqual = atoi(fields[4]);
    if (mapqual < mapQualThr) {
      return;
    }
  }
  if (count >= 6) {
    if (strand != 0 && ((strand < 0 && fields[5][0] == '+') || (strand > 0 && fields[5][0] == '-'))) {
      return;
    }
  }
  if (!dict_contains(chromosomes,chrom)) {
    pl = pl_create();
    dict_add(chromosomes,chrom,pl);
  } else {
    pl = dict_get(chromosomes,chrom);
  }
  if (left > right) {
    tmp = left;
    left = right;
    right = tmp;
  }
  pl_add(pl,left,right,occurrences);
}

int iscomment(char *line) {
  int i=0;
  int llen = strlen(line);
  while (isspace(line[i]) && i < llen) {
    i++;
  }
  return (i == llen || line[i] == '#');
}

void processGffLine(char *line,dictionary chromosomes,int mapQualThr,int strand) {
  pairlist pl;
  char *chrom;
  char *fields[6];
  int left,right,count,score;
  if (iscomment(line)) {
    return;
  }
  count = split(line,fields,6);
  score = atoi(fields[5]);
  if (score < mapQualThr) {
    return;
  }
  chrom = fields[0];
  if (!dict_contains(chromosomes,chrom)) {
    pl = pl_create();
    dict_add(chromosomes,chrom,pl);
  } else {
    pl = dict_get(chromosomes,chrom);
  }
  left = atoi(fields[3]);
  right = atoi(fields[4]);
  pl_add(pl,left,right,1);
}

void processMaqLine(char *line,dictionary chromosomes,int mapQualThr,int strand) {
  pairlist pl;
  char *chrom;
  int left,right,seqlen,qual,count;
  char *fields[16];
  count = split(line,fields,16);
  qual = atoi(fields[6]);
  if (qual < mapQualThr) {
    return;
  }
  chrom = fields[1];
  if (!dict_contains(chromosomes,chrom)) {
    pl = pl_create();
    dict_add(chromosomes,chrom,pl);
  } else {
    pl = dict_get(chromosomes,chrom);
  }
  left = atoi(fields[2]);
  seqlen = atoi(fields[13]);
  right = left + seqlen - 1;
  pl_add(pl,left,right,1);
}

void processBowLine(char *line,dictionary chromosomes,int mapQualThr,int strand) {
  char read[100];
  char chrom[100];
  char cstrand;
  char seq[512];
  pairlist pl;
  int left,right;
  int count = sscanf(line,"%s %c %s %d %s",read,&cstrand,chrom,&left,seq);
  assert(count == 5);
  right = left + strlen(seq) - 1;
  if (!dict_contains(chromosomes,chrom)) {
    pl = pl_create();
    dict_add(chromosomes,chrom,pl);
  } else {
    pl = dict_get(chromosomes,chrom);
  }
  pl_add(pl,left,right,1);
  UNUSED(mapQualThr); /* suppress compiler warning */
}

void processSugarLine(char *line,dictionary chromosomes,int mapQualThr,int strand) {
  char read[100];
  char chrom[100];
  char *underscore;
  char cstrand;
  pairlist pl;
  int left,right,score,tmp,copies;
  int count = sscanf(line,"%*s %s %*d %*d %*c %s %d %d %c %d",read,chrom,&left,&right,&cstrand,&score);
  if (count != 6) {
    fprintf(stderr,line);
    fflush(stderr);
  }
  assert(count == 6);
  if (left > right) {
    tmp = left;
    left = right;
    right = tmp;
  }
  if (!dict_contains(chromosomes,chrom)) {
    pl = pl_create();
    dict_add(chromosomes,chrom,pl);
  } else {
    pl = dict_get(chromosomes,chrom);
  }
  /* check for _xNNN replicates... */
  underscore = strrchr(read,'_');
  if (underscore == NULL) {
    pl_add(pl,left,right,1);
  } else {
    fflush(stderr);
    count = sscanf(underscore,"_x%d",&copies);
    if (count == 1) {
/*      fprintf(stderr,"got %d (copies %s)\n",copies,read); */
/*      for (i=0;i<copies;i++) { */
      pl_add(pl,left,right,copies);
/*      } */
    } else {
/*      fprintf(stderr,"got no count (%s)\n",read); */
      pl_add(pl,left,right,1);
    }
  }
  UNUSED(mapQualThr); /* suppress compiler warning */
}

void generateWiggle(FILE *fd,pairlist pl,int threshold,int interval,int normalize,double normScale,int floatingpoint,int strand,char *chrom,dictionary chromSizes) {
  int end,i,low,high,height,tmpheight;
  double tmpheightd;
  int bufferpos[65536];
  int bufferval[65536];
  int bufferindex,z;
  int buffermax,keeping;
  int chromSize;
  end = pl->pairs[pl->index-1].right; /* highest possible value */
  i = pl->pairs[0].left; /* lowest possible value */
  low = 0;
  high = 0;
  height = 0;
  bufferindex = 0;
  buffermax = 0;
  keeping = 0;
  if (dict_contains(chromSizes,chrom)) {
    chromSize = *(int *)dict_get(chromSizes,chrom);
    if (end > chromSize) {
      fprintf(stderr,"WARNING: chrom %s: truncating data to %dbp (chrom length) from %d\n",chrom,chromSize,end);
      end = chromSize;
    }
  }
  while (i < end) {
    while (pl->pairs[high].left <= i && high < pl->index) {
      high++;
      height += pl->pairs[high].height;
    }
    while (pl->pairs[low].right < i && low < pl->index) {
      low++;
      height -= pl->pairs[low].height;
    }
    if ((height > threshold || (keeping && height > 0)) && i > 0) {
      /* fprintf(fd,"%d %d\n",i,high-low); */
      if (bufferindex > 0) {
        if (bufferindex > buffermax) {
          buffermax = bufferindex;
        }
        for (z=0;z<bufferindex;z++) {
          if (floatingpoint) {
            if (normalize) {
              tmpheightd = (double)(bufferval[z]) * normScale;
            } else {
              tmpheightd = bufferval[z];
            }
            if (strand < 0) {
              tmpheightd = -tmpheightd;
            }
            fprintf(fd,"%d\t%f\n",bufferpos[z],tmpheightd);
          } else {
            if (normalize) {
              tmpheight = (int)((double)(bufferval[z]) * normScale + 0.5);
            } else {
              tmpheight = bufferval[z];
            }
            if (strand < 0) {
              tmpheight = -tmpheight;
            }
            fprintf(fd,"%d\t%d\n",bufferpos[z],tmpheight);
          }
        }
        bufferindex = 0;
      }
      keeping = 1;
      if (floatingpoint) {
        if (normalize) {
          tmpheightd = (double)height * normScale;
        } else {
          tmpheightd = height;
        }
        if (strand < 0) {
          tmpheightd = -tmpheightd;
        }
        fprintf(fd,"%d\t%f\n",i+1,tmpheightd);
      } else {
        if (normalize) {
          tmpheight = (int)((double)height * normScale + 0.5);
        } else {
          tmpheight = height;
        }
        if (strand < 0) {
          tmpheight = -tmpheight;
        }
        fprintf(fd,"%d\t%d\n",i+1,tmpheight);
      }
    } else if (height == 0) {
      bufferindex = 0;
      keeping = 0;
    } else {
      bufferpos[bufferindex] = i+1;
      bufferval[bufferindex] = height;
      bufferindex++;
      if (bufferindex >= 65536) {
        fprintf(stderr,"buffer overflow: i=%d\n",bufferindex);
        exit(-99);
      }
    }
    i += interval;
  }
}

void generateBedgraph(FILE *fd,pairlist pl,int threshold,char *chrom,int normalize,double normScale,int floatingpoint,int strand,dictionary chromSizes) {
  int end,i,j,low,high,height,pheight,left,tmpright;
  int tmpheight;
  double tmpheightd;
  int bufferleft[65536];
  int bufferright[65536];
  int bufferval[65536];
  int bufferindex,z;
  int buffermax,keeping;
  int chromSize;
  if (dict_contains(chromSizes,chrom)) {
    chromSize = *(int *)dict_get(chromSizes,chrom);
#ifdef DEBUG
    fprintf(stderr,"DEBUG: chrom %s size=%d\n",chrom,chromSize);
#endif
  } else {
    chromSize = 2000000000;
  }
  end = pl->pairs[pl->index-1].right; /* highest possible value */
  i = pl->pairs[0].left; /* lowest possible value */
  low = 0;
  high = 0;
  height = 0;
  pheight = 0;
  keeping = 0;
  bufferindex = 0;
  buffermax = 0;
  left = 0;
  while (i <= end) {
    pheight = height;
    while (pl->pairs[high].left <= i && high < pl->index) {
      height += pl->pairs[high].height;
      high++;
    }
    while (pl->pairs[low].right <= i && low < pl->index) {
      if (pl->pairs[low].right == i) {
        height -= pl->pairs[low].height;
      }
	  low++;
    }
    for (j=low;j<high;j++) {
      if (pl->pairs[j].right == i) {
        height -= pl->pairs[j].height;
      }
    }
    if (height < 0) {
      fprintf(stderr,"height implausible: %d (at %d)\n",height,i);
      exit(-1);
    }
    if (pheight != height) {
      if (pheight > threshold || (keeping && pheight > 0)) {
        if (bufferindex > 0) {
          if (bufferindex > buffermax) {
            buffermax = bufferindex;
          }
          for (z=0;z<bufferindex;z++) {
            if (floatingpoint) {
              if (normalize) {
                tmpheightd = (double)(bufferval[z]) * normScale;
              } else {
                tmpheightd = bufferval[z];
              }
              if (strand < 0) {
                tmpheightd = -tmpheightd;
              }
              if (bufferleft[z] < chromSize) {
                if (bufferright[z] > chromSize) {
                  tmpright = chromSize;
                  fprintf(stderr,"WARNING: truncating %s:%d-%d to %s:%d-%d, chrom end overhang\n",chrom,bufferleft[z],bufferright[z],chrom,bufferleft[z],tmpright);
                } else {
                  tmpright = bufferright[z];
                }
                fprintf(fd,"%s\t%d\t%d\t%f\n",chrom,bufferleft[z],tmpright,tmpheightd);
              } else {
                fprintf(stderr,"WARNING: omitting %s:%d-%d, chrom end overhang\n",chrom,bufferleft[z],bufferright[z]);
              }
            } else {
              if (normalize) {
                tmpheight = (int) ((double)(bufferval[z]) * normScale + 0.5);
              } else {
                tmpheight = bufferval[z];
              }
              if (strand < 0) {
                tmpheight = -tmpheight;
              }
              if (bufferleft[z] < chromSize) {
                if (bufferright[z] > chromSize) {
                  tmpright = chromSize;
                  fprintf(stderr,"WARNING: truncating %s:%d-%d to %s:%d-%d, chrom end overhang\n",chrom,bufferleft[z],bufferright[z],chrom,bufferleft[z],tmpright);
                } else {
                  tmpright = bufferright[z];
                }
                fprintf(fd,"%s\t%d\t%d\t%d\n",chrom,bufferleft[z],tmpright,tmpheight);
              } else {
                fprintf(stderr,"WARNING: omitting %s:%d-%d, chrom end overhang\n",chrom,bufferleft[z],bufferright[z]);
              }
            }
          }
          bufferindex = 0;
        }
        keeping = 1;
        if (floatingpoint) {
          if (normalize) {
            tmpheightd = (double)pheight * normScale;
          } else {
            tmpheightd = pheight;
          }
          if (strand < 0) {
            tmpheightd = -tmpheightd;
          }
          if (left < chromSize) {
            if (i > chromSize) {
              tmpright = chromSize;
              fprintf(stderr,"WARNING: truncating %s:%d-%d to %s:%d-%d, chrom end overhang\n",chrom,left,i,chrom,left,tmpright);
            } else {
              tmpright = i;
            }
            fprintf(fd,"%s\t%d\t%d\t%f\n",chrom,left,tmpright,tmpheightd);
          } else {
            fprintf(stderr,"WARNING: omitting %s:%d-%d, chrom end overhang\n",chrom,left,i);
          }
        } else {
          if (normalize) {
            tmpheight = (int) ((double)pheight * normScale + 0.5);
          } else {
            tmpheight = pheight;
          }
          if (strand < 0) {
            tmpheight = -tmpheight;
          }
          if (left < chromSize) {
            if (i > chromSize) {
              tmpright = chromSize;
              fprintf(stderr,"WARNING: truncating %s:%d-%d to %s:%d-%d, chrom end overhang\n",chrom,left,i,chrom,left,tmpright);
            } else {
              tmpright = i;
            }
            fprintf(fd,"%s\t%d\t%d\t%d\n",chrom,left,tmpright,tmpheight);
          } else {
            fprintf(stderr,"WARNING: omitting %s:%d-%d, chrom end overhang\n",chrom,left,i);
          }
        }
      } else if (pheight == 0) {
        keeping = 0;
        bufferindex = 0;
      } else {
        bufferleft[bufferindex] = left;
        bufferright[bufferindex] = i;
        bufferval[bufferindex] = pheight;
        bufferindex++;
        if (bufferindex >= 65536) {
          fprintf(stderr,"buffer overflow: i=%d\n",bufferindex);
          exit(-99);
        }
      }
      left = i;
    }
    i++;
  }
}

void tidyString(char *s) {
  int fnlen,i;
  char *dotpos;
  char tmpname[LINELEN];
  char *basepos;
  strcpy(tmpname,s);
  basepos = basename(tmpname);
  strcpy(s,basepos);
  fnlen = strlen(s);
  if (!strcmp(s+fnlen-3,".gz")) {
    s[fnlen-3] = '\0';
    fnlen = fnlen - 3;
  }
  dotpos = strrchr(s,'.');
  if (dotpos != NULL) {
    *dotpos = '\0';
    fnlen = strlen(s);
  }
  for (i=0;i<fnlen;i++) {
    if (s[i] == '_') {
      s[i] = ' ';
    }
  }
}

int findTop(dictionary chromosomes,char **chromnames,int ccount,int start,int oneFile,int rowsPerFile) {
  int total,index;
  pairlist cset;

  if (oneFile) {
    return ccount - 1;
  }
  total = 0;
  index = start;
  while (total < rowsPerFile && index < ccount) {
    cset = (pairlist) dict_get(chromosomes,chromnames[index]);
    total += pl_count(cset);
    index += 1;
  }
  if (index == ccount) {
    index -= 1;
  }
  return index;
}

void loadChromSizes(char *fn,dictionary sizes) {
  FILE *fd;
  char line[LINELEN];
  char *flds[2];
  int lcount,count,*n;

  fd = fopen(fn,"r");
  lcount = 0;
  while (fgets(line,1023,fd)) {
    lcount++;
    count = split(line,flds,2);
    if (count != 2 && count != 0) {
      fprintf(stderr,"Expected 2 fields per line in ChromSizes file, got '%d' at line %d, quitting.\n",count,lcount);
      exit(-1);
    }
    if (count == 2) {
      n = (int *) malloc(sizeof(int));
      *n = atoi(flds[1]);
      dict_add(sizes,flds[0],(void *)n);
#ifdef DEBUG
        fprintf(stderr,"DEBUG: chrom %s: %d bp\n",flds[0],*n);
#endif
    }
  }
#ifdef DEBUG
  fprintf(stderr,"DEBUG: read %d lines from chromsizes file\n",lcount);
#endif
  fclose(fd);
}

void dumpUsage(char *msg,FILE *fd) {
  struct rusage *ru;
  ru = (struct rusage *) malloc(sizeof(struct rusage));
  getrusage(RUSAGE_SELF,ru);
  fprintf(fd,"%s: maxrss=%ld ixrss=%ld idrss=%ld isrss=%ld\n",msg,
          ru->ru_maxrss,ru->ru_ixrss,ru->ru_idrss,ru->ru_isrss);
}
  

/***************************************************************/

int main(int argc,char **argv) {
  dictionary chromosomes;
  char **chromnames;
  char *inFN,outFN[1024],outFNPart[1024];
  FILE *inFD,*outFD;
  char inLine[LINELEN];
  char name[LINELEN];
  char description[LINELEN];
  char cmd[LINELEN];
  char chromSizeFile[LINELEN];
  int i,ccount;
  pairlist pl;
  int optpos;
  int threshold;
  int interval;
  int compressed;
  int filetype;
  int affyFmt;
  int oneFile;
  int rowsPerFile;
  int mappingQualThreshold;
  int total;
  int cfirst,clast;
  void (*lineFunc)(char *line,dictionary chromosomes,int mapQualThr,int strand);
  int outputFmt;
  int readcount = 0;
  int normalize = 0;
  int floatingpoint = 0;
  double normScale = 1.0;
  int strand = 0;
  char *strand_tag;
  char *colour_tag;
  dictionary chromSizes;

  optpos = readOptions(argc,argv,&threshold,&interval,&filetype,&compressed,
                       name,description,&oneFile,&affyFmt,&rowsPerFile,
                       &outputFmt,&mappingQualThreshold,&normalize,
                       &floatingpoint,&strand,chromSizeFile);
  inFN = argv[optpos];
  strcpy(outFN,argv[optpos+1]);
  if (strlen(name) == 0) {
    strcpy(name,inFN);
    tidyString(name);
  }
  if (strlen(description) == 0) {
    strcpy(description,inFN);
    tidyString(description);
  }

  switch (strand) {
    case -1: strand_tag = "_3p"; colour_tag = "0,0,255"; break;
    case  1: strand_tag = "_5p"; colour_tag = "255,0,0"; break;
    default: strand_tag = ""; colour_tag = "255,0,0";
  }
    
  chromosomes = dict_alloc("_.-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
  chromSizes = dict_alloc("_.-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

  if (chromSizeFile[0] != '\0') {
    loadChromSizes(chromSizeFile,chromSizes);
  }

  if (compressed == COMPUNKNOWN) { /* not specified by user */
    compressed = isCompressed(inFN);
  }
  if (filetype == UNKFMT) {
    filetype = guessFileType(inFN,compressed);
    if (filetype == UNKFMT) {
      fprintf(stderr,"Unable to determine file type of '%s'.\n",inFN);
      exit(1);
    }
  }

  if (compressed) {
    sprintf(cmd,"gunzip -c %s",inFN);
    inFD = popen(cmd,"r");
  } else if (filetype == MAQFMT) {
    sprintf(cmd,"maq mapview %s",inFN);
    inFD = popen(cmd,"r");
  } else {
    inFD = fopen(inFN,"r");
  }

  lineFunc = &processBedLine; /* defaults to bed fmt... */
  switch (filetype) {
    case BEDFMT: lineFunc = &processBedLine; break;
    case GFFFMT: lineFunc = &processGffLine; break;
    case MAQFMT: lineFunc = &processMaqLine; break;
    case BOWFMT: lineFunc = &processBowLine; break;
    case SUGFMT: lineFunc = &processSugarLine; break;
  }
  
  fgets(inLine,LINELEN,inFD);
  while (!feof(inFD)) {
    readcount++;
    (*lineFunc)(inLine,chromosomes,mappingQualThreshold,strand);
    fgets(inLine,LINELEN,inFD);
  }
  if (compressed || filetype == MAQFMT) {
    pclose(inFD);
  } else {
    fclose(inFD);
  }
/*  dumpUsage("loaded",stderr); */
  ccount = dict_count(chromosomes);
  chromnames = dict_keys(chromosomes);
  qsort(chromnames,ccount,sizeof(char *),&chromNameComp);
/*  dumpUsage("got keys",stderr); */
  total = 0;
  cfirst = 0;
  if (normalize) {
    normScale = ((double)normalize) / ((double)readcount);
  }
  clast = findTop(chromosomes,chromnames,ccount,cfirst,oneFile,rowsPerFile);
  while (cfirst < ccount) {
    /* fprintf(stderr,"first: %s  last %s\n",chromnames[cfirst],chromnames[clast]); */

    if (cfirst == clast) {
      sprintf(outFNPart,"%s_%s%s",outFN,chromnames[cfirst],SUFFIX[outputFmt]);
    } else if (cfirst != 0 || clast != ccount-1) {
      sprintf(outFNPart,"%s_%s_%s%s",outFN,
               chromnames[cfirst],chromnames[clast],SUFFIX[outputFmt]);
    } else {
      sprintf(outFNPart,"%s%s",outFN,SUFFIX[outputFmt]);
    }
    fprintf(stdout,"%s\n",outFNPart);
    outFD = fopen(outFNPart,"w");
    if (!affyFmt) {
      if (cfirst == clast) {
        fprintf(outFD,"track type=%s name=\"%s%s\" description=\"%s\" visibility=2 color=%s windowingFunction=maximum alwaysZero=ON\n",TRACK[outputFmt],name,strand_tag,description,colour_tag);
      } else {
        fprintf(outFD,"track type=%s name=\"%s_%s_%s%s\" description=\"%s_%s_%s\" visibility=2 color=%s windowingFunction=maximum alwaysZero=ON\n",TRACK[outputFmt],name,chromnames[cfirst],chromnames[clast],strand_tag,description,chromnames[cfirst],chromnames[clast],colour_tag);
      }
    }
    for (i=cfirst;i<=clast;i++) {
      /*
      if (strcmp(chromnames[i],"chrM") == 0) {
        continue;
      }
      */
      pl = (pairlist) dict_get(chromosomes,chromnames[i]);
      total += pl_count(pl);
      pl_sort(pl);
      if (affyFmt) {
        if (cfirst == clast) {
          fprintf(outFD,"track type=%s name=\"%s%s\" description=\"%s\" visibility=2 color=%s windowingFunction=maximum alwaysZero=ON\n",TRACK[outputFmt],name,strand_tag,description,colour_tag);
        } else {
          fprintf(outFD,"track type=%s name=\"%s_%s_%s%s\" description=\"%s_%s_%s\" visibility=2 color=%s windowingFunction=maximum alwaysZero=ON\n",TRACK[outputFmt],name,chromnames[cfirst],chromnames[clast],strand_tag,description,chromnames[cfirst],chromnames[clast],colour_tag);
        }
      }
      if (outputFmt == WIGGLE) {
        fprintf(outFD,"variableStep chrom=%s\n",chromnames[i]);
        generateWiggle(outFD,pl,threshold,interval,normalize,normScale,
                       floatingpoint,strand,chromnames[i],chromSizes);
      } else if (outputFmt == BEDGRAPH) {
        generateBedgraph(outFD,pl,threshold,chromnames[i],normalize,normScale,
                         floatingpoint,strand,chromSizes);
      }
    }
    fclose(outFD);
    cfirst = clast + 1;
    clast = findTop(chromosomes,chromnames,ccount,cfirst,oneFile,rowsPerFile);
  }
  /* fprintf(stderr,"total\t%d\t%s\n",total,inFN); */
/*  dumpUsage("end",stderr); */
  return 0;

}
