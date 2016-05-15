#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <errno.h>

#include "util.h"

#define USAGE "usage: summarizeFile [-e -q -f] inputfile\n"
#define LINELENGTH 2048
#define FLDLEN 128
#define OPTS "efq"
#define SAMPLEFREQ 10000

#define UNKNOWN_FORMAT 0
#define QSEQ_FORMAT 1
#define FASTQ_FORMAT 2
#define EXPORT_FORMAT 3

int file_format = UNKNOWN_FORMAT;

int readOptions(int argc, char **argv) {
  char c;
  while ((c = getopt(argc,argv,OPTS)) != -1) {
    switch (c) {
      case 'f': file_format = FASTQ_FORMAT;
                break;
      case 'q': file_format = QSEQ_FORMAT;
                break;
      case 'e': file_format = EXPORT_FORMAT;
                break;
      case '?': fprintf(stderr,USAGE);
                exit(-1);
    }
  }
  return optind;
}

int guessFileType(char *fn) {
  int fnlen;
  char *suffix;
  int ftype;

  fnlen = strlen(fn);
  suffix = strrchr(fn,'.');
  ftype = UNKNOWN_FORMAT;
  if (suffix != NULL) {
    if (!strcmp(suffix,".fq")) {
      ftype = FASTQ_FORMAT;
    } else if (!strcmp(suffix,".qseq")) {
      ftype = QSEQ_FORMAT;
    } else if (!strcmp(suffix,".export")) {
      ftype = EXPORT_FORMAT;
    }
  }
  return ftype;
}

void qual2num(char *qual,double *nums,int qlen,int offset) {
  int i;
  for (i=0;i<qlen;i++) {
    nums[i] = ((int) qual[i]) - offset;
  }
}

void parseExport(FILE *fd,char *line,char *machine,char *run,int *lane,
                 int *tile,int *xoff,int *yoff,char *index,
                 char *seq,char *qual,int *passedpf) {
  /* The following ugliness is made necessary because sscanf and related
   * functions do not correctly handle empty fields. */
  char *fld;
  fld = strsep(&line,"\t");
  strcpy(machine,fld);
  fld = strsep(&line,"\t");
  strcpy(run,fld);
  fld = strsep(&line,"\t");
/*  strcpy(lane,fld); */
  *lane = atoi(fld);
  fld = strsep(&line,"\t");
/*  strcpy(tile,fld); */
  *tile = atoi(fld);
  fld = strsep(&line,"\t");
/*  strcpy(xoff,fld); */
  *xoff = atoi(fld);
  fld = strsep(&line,"\t");
  /* strcpy(yoff,fld); */
  *yoff = atoi(fld);
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  strcpy(seq,fld);
  fld = strsep(&line,"\t");
  strcpy(qual,fld);
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t");
  fld = strsep(&line,"\t\n");
  *passedpf = !strcmp(fld,"Y");
  strcpy(index,"zork");
/*  strcpy(passedpf,fld); */
}

void parseQseq(FILE *fd,char *line,char *machine,char *run,int *lane,
              int *tile,int *xoff,int *yoff,char *index,
              char *seq,char *qual,int *passedpf) {
  int i,l;
  int fldcount = 0;
  char *fields[11];

/*  fldcount = sscanf(line,"%s %s %d %d %d %d %s %*d %s %s %d",machine,
                    run,lane,tile,xoff,yoff,index,seq,qual,passedpf);
*/
  fldcount = splits(line,fields,11);
  strcpy(machine,fields[0]);
  strcpy(run,fields[1]);
  *lane = atoi(fields[2]);
  *tile = atoi(fields[3]);
  *xoff = atoi(fields[4]);
  *yoff = atoi(fields[5]);
  strcpy(index,fields[6]);
  strcpy(seq,fields[8]);
  strcpy(qual,fields[9]);
  *passedpf = atoi(fields[10]);
  l = strlen(seq);
  for (i=0;i<l;i++) {
    if (seq[i] == '.') {
      seq[i] = 'N';
    }
  }
  assert (fldcount == 11);
}

void parseFastq(FILE *fd,char *line,char *machine,char *run,int *lane,
              int *tile,int *xoff,int *yoff,char *index,
              char *seq,char *qual,int *passedpf) {
  int fldcount = 0;
/*  char spare[100]; */
  fldcount = sscanf(line,"@%[^_]_%[^:]:%d:%d:%d:%d",
                         machine,run,lane,tile,xoff,yoff);

  /* assert (fldcount == 7 || fldcount == 6); */
  if (fldcount != 7 && fldcount != 6) {
    fldcount = sscanf(line,"@%[^:]:%[^:]:%d:%d:%d:%d",
                           machine,run,lane,tile,xoff,yoff);
  }
  fgets(line,LINELENGTH,fd); /* get sequence */
  fldcount = sscanf(line,"%s",seq);
  fgets(line,LINELENGTH,fd); /* get empty line */
  fgets(line,LINELENGTH,fd); /* get quality line */
  fldcount = sscanf(line,"%s",qual);
  strcpy(index,"zork");
  *passedpf = 1;
}

void addQual(double *qual,double *qmean,double *qm2,int total,int n) {
  int i;
  double d;
  for (i=0;i<n;i++) {
    d = qual[i] - qmean[i];
    qmean[i] = qmean[i] + (d/total);
    qm2[i] = qm2[i] + d*(qual[i]-qmean[i]);
  }
}

void finalizeQual(double *qm,int total,int n) {
  int i;
  for (i=0;i<n;i++) {
    qm[i] = sqrt(qm[i]/total);
  }
}

double qtmp[1000];
void processLine(int total,int ppfcount,int passedpf,char *qual,char *seq,
                 int qlen,double *qmean,double *qm2,double *qmeanpf,
                 double *qm2pf,int qualOffset) {
  
  qual2num(qual,qtmp,qlen,qualOffset);
  addQual(qtmp,qmean,qm2,total,qlen);
  if (passedpf) {
    addQual(qtmp,qmeanpf,qm2pf,ppfcount,qlen);
    if (ppfcount % SAMPLEFREQ == 0) {
      printf("good %s\n",seq);
    }
  }
  if (!passedpf && (total - ppfcount) % SAMPLEFREQ == 0) {
    printf("bad %s\n",seq);
  }
}

void statdump(FILE *fd,char *flag,double *data,int n) {
  int i;

  fprintf(fd,flag);
  for (i=0;i<n;i++) {
    fprintf(fd," %f",data[i]);
  }
  fprintf(fd,"\n");
}

int main(int argc,char **argv) {
  FILE *inFD;
  char *rc;
  int ind,reads,ppfcount,qualOffset,seqlen;
  char inLine[LINELENGTH];
  char machine[FLDLEN],seq[LINELENGTH],qual[LINELENGTH],run[FLDLEN];
  int lane,tile,xoff,yoff,passedpf;
  double *qmean,*qm2,*qmeanpf,*qm2pf;
  char index[100];
  void (*lineFunc)(FILE *fd,char *line,char *machine,char *run,
                   int *lane,int *tile,int *xoff,int *yoff,char *index,
                   char *seq,char *qual,int *passedpf);

  ind = readOptions(argc,argv);
  if (ind < argc) {
    inFD = fopen(argv[ind],"r");
    if (inFD == NULL) {
      printf("error %d %s\n",errno,strerror(errno));
      return 0;
    }
  } else {
    fprintf(stderr,USAGE);
    exit(-1);
  }

  reads = 0;
  ppfcount = 0;
  if (file_format == UNKNOWN_FORMAT) {
    file_format = guessFileType(argv[ind]);
  }
  lineFunc = NULL;
  qualOffset = 0;
  switch (file_format) {
    case UNKNOWN_FORMAT:
      fprintf(stderr,"Cannot determine file type of '%s'.\n",argv[ind]);
      exit(-2);
    case FASTQ_FORMAT:
      lineFunc = parseFastq;
      qualOffset = 33;
      break;
    case QSEQ_FORMAT:
      lineFunc = parseQseq;
      qualOffset = 33;
      break;
    case EXPORT_FORMAT:
      lineFunc = parseExport;
      qualOffset = 64;
      break;
    default:
      fprintf(stderr,"Unknown file format '%d'.\n",file_format);
      exit(-3);
  }

  rc = fgets(inLine,LINELENGTH,inFD);
  if (rc == NULL) {
    printf("empty\n");
    return 0 ;
  }
  lineFunc(inFD,inLine,machine,run,&lane,&tile,&xoff,
           &yoff,index,seq,qual,&passedpf);
  
  printf("machine %s\nrunnumber %s\nflowlane %d\n",machine,run,lane);
  seqlen = strlen(seq);
  qmean = (double *) calloc(seqlen,sizeof(double));
  qm2 = (double *) calloc(seqlen,sizeof(double));
  qmeanpf = (double *) calloc(seqlen,sizeof(double));
  qm2pf = (double *) calloc(seqlen,sizeof(double));
  reads = 1;
  ppfcount = passedpf;
  processLine(reads,ppfcount,passedpf,qual,seq,seqlen,
              qmean,qm2,qmeanpf,qm2pf,qualOffset);
  while (fgets(inLine,LINELENGTH,inFD) != NULL) {
    reads++;
    lineFunc(inFD,inLine,machine,run,&lane,&tile,&xoff,&yoff,index,seq,qual,&passedpf);
    ppfcount += passedpf;
    processLine(reads,ppfcount,passedpf,qual,seq,seqlen,
                qmean,qm2,qmeanpf,qm2pf,qualOffset);
  }
  finalizeQual(qm2,reads,seqlen);
  finalizeQual(qm2pf,ppfcount,seqlen);
  fprintf(stdout,"reads %d\npassedpf %d\n",reads,ppfcount);
  if (ppfcount > 0) {
    statdump(stdout,"qualmean",qmean,seqlen);
    statdump(stdout,"qualstdev",qm2,seqlen);
    statdump(stdout,"qualmeanpf",qmeanpf,seqlen);
    statdump(stdout,"qualstdevpf",qm2pf,seqlen);
  }
  return(0);
}
