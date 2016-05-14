/*
 * Fast linker screening, version 2.0 by Gord Brown.
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

#if 0

#include "calign.h"
#include "fastq.h"
#include "fasta.h"
#include "util.h"

#define LINELENGTH 1024 /* maximum line length          */
#define TRUNCATE 0
#define MASK_N 1
#define MASK_X 2

int requireLinker = 0;  /* throw sequence out unless linker is found */
int solexaQuality = 0;  /* quality values are Solexa-style */
int maskMethod = TRUNCATE; /* what to do with masked sequence */
int wholeRead = 0; /* if true, only filter reads which are all linker; don't
                      trim other reads (implies not requireLinker) */

int minLengthToKeep = 16;
int minLengthToCallLinker = 5;
int matchLen = 15;
float identity = 0.80;
char badFN1[255];
char badFN2[255];
char missingLinkerFN1[255];
char missingLinkerFN2[255];
int verbose = 0;
int overwrite = 0;
int isFasta = 0;
int isPaired = 0;

static char *USAGE = "usage: screenLinker [-k<nn>] [-i<ff>] [-l<nn>] [-s<badOutputPrefix>] [-q] [-r] [-n|-x|-t] [-v] [-o] [-w] <linkerFile> <inputFile> <outputFile>\n";
static char opts[] = "oqi:l:s:k:rnxtm:wv";
int readOptions(int argc,char **argv) {
  char c;
  missingLinkerFN1[0] = '\0';
  missingLinkerFN2[0] = '\0';
  badFN1[0] = '\0';
  badFN2[0] = '\0';
  while ((c = getopt(argc,argv,opts)) != -1) {
    switch (c) {
      case 'i': identity = atof(optarg);
                break;
      case 'l': matchLen = atoi(optarg);
                break;
      case 's': strcpy(badFN1,optarg);
                strcpy(badFN2,optarg);
                strcat(badFN1,"p1.fq");
                strcat(badFN2,"p2.fq");
                break;
      case 'm': strcpy(missingLinkerFN1,optarg);
                strcpy(missingLinkerFN2,optarg);
                strcat(missingLinkerFN1,"p1.fq");
                strcat(missingLinkerFN2,"p2.fq");
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

int checkArgs(int argc,char **argv,int ind) {
  /* basic rule: after option processing, we should have exactly 3 filename
   * arguments (for single end), or exactly 5 filename arguments (for paired-
   * end): the linker file, 1 or 2 input Fastq (or Fasta) files, and 1 or 2
   * output files.  The input files must exist.  The output files should not
   * exist, unless "-o" (overwrite) is set. */
  int count = argc - ind;
  if (count == 3) {
    /* single-end */
    isPaired = 0;
  } else if (count == 5) {
    /* paired-end */
    isPaired = 1;
  } else {
    fprintf(stderr,USAGE);
    exit(-2);
  }
  if (!isFastaFile(argv[ind])) {
    fprintf(stderr,
            "linker file '%s' not found or not a Fasta file.\n",
            argv[ind]);
    exit(-3);
  }

  /* check whether input is FA or FQ (or something else) */
  if (!fileExists(argv[ind+1])) {
    fprintf(stderr,"cannot stat input file '%s'\n",argv[ind+1]);
    exit(-4);
  }
  if (isPaired && !fileExists(argv[ind+2])) {
    fprintf(stderr,"cannot stat input file '%s'\n",argv[ind+2]);
    exit(-4);
  }
  if (isFastaFile(argv[ind+1])) {
    isFasta = 1;
  } else if (isFastqFile(argv[ind+1])) {
    isFasta = 0;
  } else {
    fprintf(stderr,"input file '%s' does not appear to be Fasta or Fastq.\n",argv[ind+1]);
    exit(-5);
  }
  /* we're going to trust that the two inputs are the same type... */

  /* if we got this far, it's all okay. */
  return 1;
}



/* open a file for writing, if we want it, and if either it doesn't exist,
 * or 'overwrite' is true.  Return the FILE pointer or NULL. */
FILE *openIfAppropriate(char *fn,int wantFile,int overwrite) {
  FILE *fd = NULL;
  if (wantFile) {
    if (fileExists(fn) && !overwrite) {
      fprintf(stderr,"output file '%s' exists, will not overwrite.\n",fn);
      exit(-1);
    }
    fd = fopen(fn,"w");
  }
  return fd;
}

void getLinker(char *fn,fastap *linker1,fastap *linker2) {
  FILE *lfd;
  lfd = fopen(fn,"r");
  *linker1 = fa_alloc(LINELENGTH);
  *linker2 = fa_alloc(LINELENGTH);
  fa_next(*linker1,lfd);
  fa_next(*linker2,lfd);
  if (verbose) {
    fa_fasta(*linker1,stderr);
    fa_fasta(*linker2,stderr);
  }
  fclose(lfd);
  return;
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
  score2 = alignLinker(seq,link2,&alnlen2,&id2,&start2,&end2);
  percentId1 = ((float) id1) / ((float) alnlen1);
  percentId2 = ((float) id2) / ((float) alnlen2);
  coverage1 = end1-start1+1;
  coverage2 = end2-start2+1;

  if (percentId1 >= identity &&
      (coverage1 >= matchLen || (allowTail && strlen(seq)-end1 <= 2 &&
                                coverage1 >= minLengthToCallLinker))) {
    found1 = 1;
  }
  if (percentId2 >= identity &&
      (coverage2 >= matchLen || (allowTail && strlen(seq)-end2 <= 2 &&
                                coverage2 >= minLengthToCallLinker))) {
    found2 = 1;
  }

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
void processFasta(fastap linker1,fastap linker2, FILE *input1, FILE *input2,
                  FILE *good1, FILE *good2, FILE *bad1, FILE *bad2,
                  FILE *missing1, FILE *missing2,
                  int *cTrimmed, int *cUnTrimmed, int *cLinker,
                  int progress) {
  fastap seq1,seq2;
  int count,position;

  seq1 = fa_alloc(LINELENGTH);
  seq2 = fa_alloc(LINELENGTH);
  count = 0;
  while (fa_next(seq1,input1)) {
    if (isPaired) {
      fa_next(seq2,input2);
    }
    if (isLinker(seq1->data,linker1,linker2,&position)) {
      if (bad1 != NULL) {
        strcat(seq1->id,reason);
        fa_fasta(seq1,bad1);
      }
      (*cLinker)++;
    } else {
      if (wholeRead) {
        fa_fasta(seq1,good1);
        (*cUnTrimmed)++;
      } else if (position != -1) {
        switch (maskMethod) {
          case TRUNCATE: fa_fasta_trunc(seq1,good1,position);
                         break;
          case MASK_N:   fa_mask(seq1,position,0,'N');
                         fa_fasta(seq1,good1);
                         break;
          case MASK_X:   fa_mask(seq1,position,0,'X');
                         fa_fasta(seq1,good1);
                         break;
        }
        (*cTrimmed)++;
      } else {
        if (requireLinker) {
          if (missing1 != NULL) {
/*            strcat(seq->id," (no linker found)"); */
            fa_fasta(seq1,missing1);
          }
        } else {
          fa_fasta(seq1,good1);
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

/* read a fastq file, screen for linker. */
void processFastq(fastap linker1,fastap linker2, FILE *input1, FILE *input2,
                  FILE *good1, FILE *good2, FILE *bad1, FILE *bad2,
                  FILE *missing1, FILE *missing2,
                  int *cTrimmed, int *cUnTrimmed, int *cLinker,
                  int progress) {
  fastqp seq1,seq2;
  int count,position;

  seq1 = fq_alloc(LINELENGTH);
  seq2 = fq_alloc(LINELENGTH);
  count = 0;
  while (fq_next(seq1,input1)) {
    if (isPaired) {
      fq_next(seq2,input2);
    }
    if (isLinker(seq1->data,linker1,linker2,&position)) {
      if (bad1 != NULL) {
        strcat(seq1->id,reason);
        fq_fastq(seq1,bad1);
        if (isPaired) {
          strcat(seq2->id,reason);
          fq_fastq(seq2,bad2);
        }
      }
      (*cLinker)++;
    } else {
      if (wholeRead) {
        fq_fastq(seq1,good1);
        if (isPaired) {
          fq_fastq(seq2,good2);
        }
        (*cUnTrimmed)++;
      } else if (position != -1) {
        switch (maskMethod) {
          case TRUNCATE: fq_fastq_trunc(seq1,good1,position);
                         if (isPaired) {
                           fq_fastq(seq2,good2);
                         }
                         break;
          case MASK_N:   fq_mask(seq1,position,0,'N');
                         fq_setQualityToZero(seq1,position,0,solexaQuality);
                         fq_fastq(seq1,good1);
                         if (isPaired) {
                           fq_fastq(seq2,good2);
                         }
                         break;
          case MASK_X:   fq_mask(seq1,position,0,'X');
                         fq_fastq(seq1,good1);
                         if (isPaired) {
                           fq_fastq(seq2,good2);
                         }
                         break;
        }
        (*cTrimmed)++;
      } else {
        if (requireLinker) {
          if (missing1 != NULL) {
            strcat(seq1->id," (no linker)");
            fq_fastq(seq1,missing1);
            if (isPaired) {
              fq_fastq(seq2,missing2);
            }
          }
        } else {
          fq_fastq(seq1,good1);
          if (isPaired) {
            fq_fastq(seq2,good2);
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

int main(int argc,char **argv) {
  FILE *input1,*input2=NULL,*good1,*good2=NULL;
  FILE *bad1=NULL,*bad2=NULL,*missing1=NULL,*missing2=NULL;
  fastap linker1,linker2;
  int ind;
  int showProgress = 0;
  int cTrimmed=0,cUnTrimmed=0,cLinker=0;

  /* process the command-line options */
  ind = readOptions(argc,argv);
  if (isatty(fileno(stderr))) {
    showProgress = 1;
  }

  /* after checkArgs, know either 3 (single-end) or 5 (paired-end) arguments
   * are present (checkArgs won't return if args are not good). */
  checkArgs(argc,argv,ind);

  /* check the linker file, read linkers */
  getLinker(argv[ind],&linker1,&linker2);
  
  input1 = fopen(argv[ind+1],"r");
  if (isPaired) {
    input2 = fopen(argv[ind+2],"r");
  }

  /* open output file(s) */
  if (isPaired) {
    good1 = openIfAppropriate(argv[ind+3],1,overwrite);
    good2 = openIfAppropriate(argv[ind+4],1,overwrite);
  } else {
    good1 = openIfAppropriate(argv[ind+2],1,overwrite);
  }

  /* if we're supposed to save the bad stuff, get it ready... */
  bad1 = openIfAppropriate(badFN1,strlen(badFN1)!=0,overwrite);
  if (bad1 && isPaired) {
    bad2 = openIfAppropriate(badFN2,1,overwrite);
  }

  /* missing linker output file */
  missing1 = openIfAppropriate(missingLinkerFN1,missingLinkerFN1!=NULL,overwrite);
  if (missing1 && isPaired) {
    missing2 = openIfAppropriate(missingLinkerFN2,1,overwrite);
  }
    
  /* set up the alignment stuff */
  calignInit(LINELENGTH,2,-2,-6,-1);

  if (isFasta) {
    processFasta(linker1,linker2,input1,input2,good1,good2,bad1,bad2,missing1,missing2,&cTrimmed,&cUnTrimmed,&cLinker,showProgress);
  } else {
    processFastq(linker1,linker2,input1,input2,good1,good2,bad1,bad2,missing1,missing2,&cTrimmed,&cUnTrimmed,&cLinker,showProgress);
  }
  fprintf(stdout,"trimmed=%d  untrimmed=%d  linker=%d\n",
                 cTrimmed,cUnTrimmed,cLinker);
  return 0;
}

#endif
