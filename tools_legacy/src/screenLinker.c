/*
 * Fast linker screening, version 1.0 by Gord Brown.
 *
 * This code is in the public domain.  Do whatever you like with it, but
 * please do not remove my name.  Unless a bug in the code causes a terrible
 * tragedy in which thousands perish.  Then please do remove my name.
 *
 * No guarantees, expressed or implied.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#include "calign.h"
#include "fastq.h"
#include "fasta.h"
#include "util.h"

#define LINELENGTH 1024 /* maximum line length          */
#define TRUNCATE 0
#define MASK_N 1
#define MASK_X 2

int filterQual = 0;     /* whether to filter on quality */
int requireLinker = 0;  /* throw sequence out unless linker is found */
int solexaQuality = 0;  /* quality values are Solexa-style */
int maskMethod = TRUNCATE; /* what to do with masked sequence */
int wholeRead = 0; /* if true, only filter reads which are all linker; don't
                      trim other reads (implies not requireLinker) */

int minLengthToKeep = 16;
int minLengthToCallLinker = 5;
int matchLen = 15;
float identity = 0.80;
int filterAvg = 13;
int filterPrefix = 25;
char *badOutputFile = NULL;
char *missingLinkerOutputFile = NULL;
int verbose = 0;
int overwrite = 0;
int justCount = 0;

static char *USAGE = "usage: screenLinker [-c] [-k<nn>] [-i<ff>] [-l<nn>] [-f [-a<nn>] [-p<nn>]] [-s<badOutputFile>] [-q] [-r] [-n|-x|-t] [-v] [-o] [-w] <linkerFile> <inputFile> <outputFile>\n";
static char opts[] = "coqfi:l:a:p:s:k:rnxtm:wv";
int readOptions(int argc,char **argv) {
  char c;
  while ((c = getopt(argc,argv,opts)) != -1) {
    switch (c) {
      case 'c': justCount = 1;
                break;
      case 'i': identity = atof(optarg);
                break;
      case 'l': matchLen = atoi(optarg);
                break;
      case 'f': filterQual = 1; /* yes, we want to filter */
                break;
      case 'a': filterAvg = atoi(optarg);
                break;
      case 'p': filterPrefix = atoi(optarg);
                break;
      case 's': badOutputFile = strdup(optarg);
                break;
      case 'm': missingLinkerOutputFile = strdup(optarg);
                break;
      case 'k': minLengthToKeep = atoi(optarg);
                break;
      case 'q': solexaQuality = 1;
                break;
      case 'r': requireLinker = 1;
                break;
      case 't': maskMethod = TRUNCATE;
                break;
      case 'n': maskMethod = MASK_N;
                break;
      case 'v': verbose++;
                break;
      case 'w': wholeRead = 1;
                requireLinker = 0;
                identity = 0.9;
                matchLen = 21;
                break;
      case 'x': maskMethod = MASK_X;
                break;
      case 'o': overwrite = 1;
                break;
      case '?': fprintf(stderr,USAGE);
                exit(-1);
    }
  }
  return optind;
}

void getLinker(char *fn,fastap linker1,fastap linker2) {
  FILE *lfd;
  lfd = fopen(fn,"r");
  fa_next(linker1,lfd);
  fa_next(linker2,lfd);
  if (verbose) {
    fa_fasta(linker1,stderr);
    fa_fasta(linker2,stderr);
  }
  fclose(lfd);
  return;
}

int meanQual(char *qstr,int qlen) {
  int i;
  int sum = 0;
  int offset = 33;
  if (solexaQuality) {
    offset = 64;
  }
  for (i=0;i<qlen;i++) {
    sum += qstr[i] - offset;
  }
  return (int) (sum * 1.0 / qlen) + 0.5;
}

int isLowQual(fastqp seq) {
  char *npos;
  int qmean;
  npos = strchr(seq->data,'N');
  if (npos && npos - seq->data < filterPrefix) {
    return 1;
  }
  qmean = meanQual(seq->qual,filterPrefix);
  return qmean < filterAvg;
}

char tb1[LINELENGTH*2];
char tb2[LINELENGTH*2];
int alignLinker(char *seq,char *linker,int *len, int *id,int *start,int *end) {
  int score;
  score = calignEndsFree(linker,seq);
  calignTraceback(tb1,tb2);
  *len = strlen(tb1);
  *id = calignIdentity();
  *start = calignPosition2start();
  *end = calignPosition2end();
  return score;
}

int foundLinker(char *seq,char *link1,char *link2,int *st,int *en,float *pId,
                int allowTail) {
  int score,alnlen,id,start,end,coverage;
  int score1,alnlen1,id1,start1,end1,coverage1;
  int score2,alnlen2,id2,start2,end2,coverage2;
  int found,found1,found2;
  float percentId,percentId1,percentId2;
  char *link;

  found1 = found2 = found = 0;

  score1 = alignLinker(seq,link1,&alnlen1,&id1,&start1,&end1);
  percentId1 = ((float) id1) / ((float) alnlen1);
  coverage1 = end1-start1+1;
  if (percentId1 >= identity &&
      (coverage1 >= matchLen || (allowTail && strlen(seq)-end1 <= 2 &&
                                coverage1 >= minLengthToCallLinker))) {
    found1 = 1;
  }

  score2 = alignLinker(seq,link2,&alnlen2,&id2,&start2,&end2);
  percentId2 = ((float) id2) / ((float) alnlen2);
  coverage2 = end2-start2+1;

  if (percentId2 >= identity &&
      (coverage2 >= matchLen || (allowTail && strlen(seq)-end2 <= 2 &&
                                coverage2 >= minLengthToCallLinker))) {
    found2 = 1;
  }

/*
  score2 = 0;
  percentId2 = 0.0;
  coverage2 = 0;
  found2 = 0;
*/

  if (verbose > 1) {
    if (found1) {
      fprintf(stderr,"found1: %s score=%d id=%d alnlen=%d\n",seq,score1,id1,alnlen1);
      fprintf(stderr,"        %*s%s\n",start1,"",link1);
    } 
    if (found2) {
      fprintf(stderr,"found2: %s score=%d id=%d alnlen=%d\n",seq,score2,id2,alnlen2);
      fprintf(stderr,"        %*s%s\n",start2,"",link2);
    } 
  }
  if (found1 && (!found2 || score1 > score2)) {
    found = 1;
    score = score1;
    alnlen = alnlen1;
    id = id1;
    start = start1;
    end = end1;
    coverage = coverage1;
    percentId = percentId1;
    link = link1;
  } else if ((!found1 || score2 >= score1) && found2) {
    found = 1;
    score = score2;
    alnlen = alnlen2;
    id = id2;
    start = start2;
    end = end2;
    coverage = coverage2;
    percentId = percentId2;
    link = link2;
  } else if (!found1 && !found2) {
    found = 0;
  } else {
    fprintf(stderr,"ERROR: ambiguous case in foundLinker\n");
    fprintf(stderr,"  seq:  %s\n",seq);
    fprintf(stderr,"  lnk1: %s score1=%d id1=%d start1=%d end1=%d alnlen1=%d\n",
            link1,score1,id1,start1,end1,alnlen1);
    fprintf(stderr,"  lnk2: %s score2=%d id2=%d start2=%d end2=%d alnlen2=%d\n",
            link2,score2,id2,start2,end2,alnlen2);
    found = 0;
  }
  if (found) {
    *pId = percentId;
    *st = start;
    *en = end;
    if (verbose) {
      fprintf(stderr,"final: %s score=%d id=%d alnlen=%d\n",seq,score,id,alnlen);
      fprintf(stderr,"       %*s%s\n",start,"",link);
    }
  } else {
    if (verbose) fprintf(stderr,"no match %s\n",seq);
  }
  return found;
}

char reason[1024];
char scrap[1024];
int isLinker(char *seq,fastap link1,fastap link2,int *position) {
  int start,end;
  float pId;
  int fstart,fend;

  if (foundLinker(seq,link1->data,link2->data,&start,&end,&pId,1)) {
    if (start < minLengthToKeep) {
      sprintf(reason," start=%d  length=%d  id=%f",start,end-start+1,pId);
      return 1; /* it's all linker */
    } else {
      if (wholeRead) {
        *position = -1;
        return 0;
      }
      strncpy(scrap,seq,start);
      scrap[start] = 0;
      if (verbose > 2) {
        fprintf(stderr,"second alignment (start=%d):\n       %s\n       %s\n",
                start,seq,scrap);
      }
      if (foundLinker(scrap,link1->data,link2->data,&fstart,&fend,&pId,0)) {
        if (fstart < minLengthToKeep) {
          sprintf(reason," start=%d  length=%d  id=%f (double linker)",fstart,fend-fstart+1,pId);
          return 1; /* it's all linker */
        } else {
          *position = fstart;
          return 0;
        }
      } else {
        *position = start;
        return 0;
      }
    }
  } else {
    *position = -1;
    return 0;
  }
}

/* read a Fasta file, screen for linker (no quality scoring or filtering,
 * obviously). */
void processFasta(fastap linker1,fastap linker2,
                  FILE *input, FILE *good, FILE *bad,FILE *missing,
                  int *cTrimmed, int *cUnTrimmed, int *cLinker,
                  int progress) {
  fastap seq;
  int count,position;

  seq = fa_alloc(LINELENGTH);
  count = 0;
  while (fa_next(seq,input)) {
    if (isLinker(seq->data,linker1,linker2,&position)) {
      if (bad != NULL) {
        strcat(seq->id,reason);
        fa_fasta(seq,bad);
      }
      (*cLinker)++;
    } else {
      if (wholeRead) {
        if (good != NULL) fa_fasta(seq,good);
        (*cUnTrimmed)++;
      } else if (position != -1) {
        if (good != NULL) {
          switch (maskMethod) {
            case TRUNCATE: fa_fasta_trunc(seq,good,position);
                           break;
            case MASK_N:   fa_mask(seq,position,0,'N');
                           fa_fasta(seq,good);
                           break;
            case MASK_X:   fa_mask(seq,position,0,'X');
                           fa_fasta(seq,good);
                           break;
          }
        }
        (*cTrimmed)++;
      } else {
        if (good != NULL) {
          if (requireLinker) {
            if (missing != NULL) {
  /*            strcat(seq->id," (no linker found)"); */
              fa_fasta(seq,missing);
            }
          } else {
            fa_fasta(seq,good);
          }
        }
        (*cUnTrimmed)++;
      }
    }
    count++;
    if (progress && count % 100000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
  }
  return;
}

/* read a fastq file, screen for linker (may do quality scoring, filtering
 * as well). */
void processFastq(fastap linker1,fastap linker2,
                  FILE *input, FILE *good, FILE *bad, FILE *missing,
                  int *cTrimmed, int *cUnTrimmed, int *cQual, int *cLinker,
                  int progress) {
  fastqp seq;
  int count,position;

  seq = fq_alloc(LINELENGTH);
  count = 0;
  while (fq_next(seq,input)) {
    if (filterQual && isLowQual(seq)) {
      if (bad != NULL) {
        strcat(seq->id," (low quality)");
        fq_fastq(seq,bad);
      }
      (*cQual)++;
    } else {
      if (isLinker(seq->data,linker1,linker2,&position)) {
        if (bad != NULL) {
          strcat(seq->id,reason);
          fq_fastq(seq,bad);
        }
        (*cLinker)++;
      } else {
        if (wholeRead) {
          if (good != NULL) fq_fastq(seq,good);
          (*cUnTrimmed)++;
        } else if (position != -1) {
          if (good != NULL) {
            switch (maskMethod) {
              case TRUNCATE: fq_fastq_trunc(seq,good,position);
                             break;
              case MASK_N:   fq_mask(seq,position,0,'N');
                             fq_setQualityToZero(seq,position,0,solexaQuality);
                             fq_fastq(seq,good);
                             break;
              case MASK_X:   fq_mask(seq,position,0,'X');
                             fq_fastq(seq,good);
                             break;
            }
          }
          (*cTrimmed)++;
        } else {
          if (good != NULL) {
            if (requireLinker) {
              if (missing != NULL) {
                strcat(seq->id," (no linker)");
                fq_fastq(seq,missing);
              }
            } else {
              fq_fastq(seq,good);
            }
          }
          (*cUnTrimmed)++;
        }
      }
    }
    count++;
    if (progress && count % 100000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
  }
  return;
}

int main(int argc,char **argv) {
  FILE *input,*good,*bad = NULL,*missing=NULL;
  fastap linker1,linker2;
  int ind,isFasta;
  int wantProgressMeter = 0;
  int cTrimmed=0,cUnTrimmed=0,cLinker=0,cQual=0;

  /* process the command-line options */
  ind = readOptions(argc,argv);
  if (isatty(fileno(stderr))) {
    wantProgressMeter = 1;
  }

  /* check the linker file, read linkers */
  if (!isFastaFile(argv[ind])) {
    fprintf(stderr,"linker file '%s' does not exist or does not appear to be a Fasta file.\n",argv[ind]);
    exit(-1);
  }
  linker1 = fa_alloc(LINELENGTH);
  linker2 = fa_alloc(LINELENGTH);
  getLinker(argv[ind],linker1,linker2);
  
  /* check that the input file looks good, get its type */
  if (ind + 1 >= argc) {
    fprintf(stderr,USAGE);
    exit(-1);
  }
  if (!fileExists(argv[ind+1])) {
    fprintf(stderr,"input file '%s' does not exist (or some other error occurred).\n",argv[ind+1]);
    exit(-1);
  }
  if (isFastaFile(argv[ind+1])) {
    isFasta = 1;
  } else if (isFastqFile(argv[ind+1])) {
    isFasta = 0;
  } else {
    fprintf(stderr,"input file '%s' does not appear to be Fasta or Fastq.\n",argv[ind+1]);
    exit(-1);
  }
  input = fopen(argv[ind+1],"r");

  /* check that output file doesn't exist, open it */
  if (!justCount) {
    if (ind + 2 >= argc) {
      fprintf(stderr,USAGE);
      exit(-1);
    }
    if (fileExists(argv[ind+2]) && !overwrite) {
      fprintf(stderr,"output file '%s' exists, will not overwrite.\n",argv[ind+2]);
      exit(-1);
    }
    good = fopen(argv[ind+2],"w");
  } else {
    good = NULL;
  }

  /* if we're supposed to save the bad stuff, get it ready... */
  if (badOutputFile != NULL) {
    if (fileExists(badOutputFile)) {
      fprintf(stderr,"bad output file '%s' exists, will not overwrite.\n",badOutputFile);
      exit(-1);
    }
    bad = fopen(badOutputFile,"w");
  }

  /* missing linker output file */
  if (missingLinkerOutputFile != NULL) {
    if (fileExists(missingLinkerOutputFile)) {
      fprintf(stderr,"missing linker output file '%s' exists, will not overwrite.\n",missingLinkerOutputFile);
      exit(-1);
    }
    missing = fopen(missingLinkerOutputFile,"w");
  }
    
  /* set up the alignment stuff */
  calignInit(LINELENGTH,2,-2,-6,-1);

  if (isFasta) {
    processFasta(linker1,linker2,input,good,bad,missing,&cTrimmed,&cUnTrimmed,&cLinker,wantProgressMeter);
  } else {
    processFastq(linker1,linker2,input,good,bad,missing,&cTrimmed,&cUnTrimmed,&cQual,&cLinker,wantProgressMeter);
  }
  fprintf(stdout,"trimmed=%d  untrimmed=%d  linker=%d  lowqual=%d\n",
                 cTrimmed,cUnTrimmed,cLinker,cQual);
  return 0;
}
