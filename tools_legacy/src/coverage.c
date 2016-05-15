#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <getopt.h>

#include "calign.h"
#include "fastq.h"
#include "sequence.h"
#include "fastareader.h"
#include "util.h"
#include "dictionary.h"

/*
 * Plan: 1) read PhiX.
 *       2) For each read, that got some sort of blat hit:
 *          a) Extract region of interest.
 *          b) Align.
 *          c) Increment appropriate base per position (relative to phix).
 *          d) If indel, record position separately.
 *       3) Print out summary in HTML format (or excel, or both).
 */

#define LINELENGTH 1024
#define SLACK 50 /* extra nucleotides to align against */

#define EXONERATE 1
#define BLAT 2

#define max(a,b) ((a)>(b)?(a):(b))

typedef struct nucleotide {
  int count;
  float mean;
  float m2;
} *nucp;
  

typedef struct counter {
  nucp a;
  nucp c;
  nucp g;
  nucp t;
  nucp n;
  nucp ia;
  nucp ic;
  nucp ig;
  nucp it;
  nucp in;
  int *dcount;
  int length;
  int *flag;
} *counterp;

typedef struct pos {
  int start;
  int score;
  char strand;
  int count;
} *posp;

counterp allocateCounter(int size) {
  counterp cp;

  cp = (counterp) malloc(sizeof(struct counter));
  cp->length = size;
  cp->a = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->c = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->g = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->t = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->n = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->ia = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->ic = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->ig = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->it = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->in = (nucp) calloc(size,sizeof(struct nucleotide));
  cp->dcount = (int *) calloc(size,sizeof(int));
  cp->flag = (int *) calloc(size,sizeof(int));
  return cp;
}

void readHitPositions(dictionary dict,FILE *hits,int searchType) {
  char line[LINELENGTH];
  char strand[3],qname[255];
  char *qptr;
  int mat,qlen,qstart,qend,tstart,tend,tmp;
  char *blatFormat = "%d %*d %*d %*d %*d %*d %*d %*d %s %s %d %d %d %*s %*d %d %d";
  char *exonFormat = "%*s %s %d %d %*s %*s %d %d %s %d";
  int count;
  int linecount,nonunique,kept,conflict; 
  posp p;
  
  linecount = 0;
  nonunique = 0;
  kept = 0;
  conflict = 0;
  while (fgets(line,LINELENGTH,hits)) {
    if (searchType == BLAT) {
      count = sscanf(line,blatFormat,&mat,strand,qname,&qlen,&qstart,&qend,&tstart,&tend);
      if (count != 8) {
        fprintf(stderr,"WARNING: got %d fields in sscanf on line '%s'\n",
                count,line);
        continue;
      }
    } else if (searchType == EXONERATE) {
      count = sscanf(line,exonFormat,qname,&qstart,&qend,&tstart,&tend,strand,&mat);
      if (count != 7) {
        fprintf(stderr,"WARNING: got %d fields in sscanf on line '%s'\n",
                count,line);
        continue;
      }
      if (strand[0] == '-') {
        tmp = tstart;
        tstart = tend;
        tend = tmp;
      }
      qlen = 45;
    }
    qptr = strchr(qname,':')+1;
    if (dict_contains(dict,qptr)) {
      p = (posp) dict_get(dict,qptr);
      if (p->score < mat) {
        p->score = mat;
        p->strand = strand[0];
        if (p->strand == '+') {
          p->start = tstart - qstart;
        } else {
          p->start = tstart - (qlen-qend);
        }
        p->count = 1;
      } else if (p->score == mat) {
        fprintf(stderr,"INFO: multiple matches for %s: %d, %d (s=%d)\n",
                       qptr,p->start,tstart,mat);
        conflict++;
        p->count++;
      }
      nonunique++;
    } else {
      p = (posp) malloc(sizeof(struct pos));
      p->strand = strand[0];
      if (p->strand == '+') {
        p->start = tstart - qstart;
      } else {
        p->start = tstart - (qlen-qend);
      }
      p->score = mat;
      p->count = 1;
      dict_add(dict,qptr,p);
      kept++;
    }
    linecount++;
  }
  fprintf(stderr,"read %d hitpos lines, kept %d, %d nonunique, %d conflicts\n",linecount,kept,nonunique,conflict);
  return;
}

void tabulate(nucp n,int offset,int qval) {
  float delta;
  n[offset].count++;
  delta = qval - n[offset].mean;
  n[offset].mean = n[offset].mean + delta / n[offset].count;
  n[offset].m2 = n[offset].m2 + delta * (qval - n[offset].mean);
}

int countMismatches(char *str1,char *str2) {
  int i,l,c;
  l = strlen(str1);
  c = 0;
  for (i=0;i<l;i++) {
    if (str1[i] != str2[i]) {
      c++;
    }
  }
  return c;
}

char tb1[200],tb2[200];
void align(char *read,int *qual,char *target,int position,counterp counts,int* posErrors,int *used) {
  /* align against chunk. */
  /* figure out where alignment starts (position + seq1pos) */
  /* go through traceback, increment counters as applicable. */
  int score,genomeOffset,readStart,alignLen,i,readPosn,mm;
/*  fprintf(stderr,"lengths: read=%d targ=%d\n",strlen(read),strlen(target));*/
/*  fflush(stderr); */
/*  fprintf(stderr,"source:\n         %s\n    %s\n",read,target);*/
  score = calignPartialFree(read,target);
  calignTraceback(tb1,tb2);
  mm = countMismatches(tb1,tb2);
  *used = 0;
  if (mm != 1) {
    return;
  }
  *used = 1;
 /* fprintf(stderr,"alignment:\n    %s\n    %s\n",tb1,tb2);*/
  readStart = calignPosition2start();
  alignLen = calignLength();
/*  fprintf(stderr,"readstart=%d alnlen=%d\n",readStart,alignLen); */
  genomeOffset = position + readStart;
/*  fprintf(stderr,"genomeOffset: %d+%d=%d\n",position,readStart,genomeOffset);*/
  readPosn = 0;
  for (i=0;i<alignLen;i++) {
    if (tb2[i] == '-') {
      switch (tb1[i]) {
        case 'A':
        case 'a':
          tabulate(counts->ia,genomeOffset,qual[readPosn++]);
          break;
        case 'C':
        case 'c':
          tabulate(counts->ic,genomeOffset,qual[readPosn++]);
          break;
        case 'G':
        case 'g':
          tabulate(counts->ig,genomeOffset,qual[readPosn++]);
          break;
        case 'T':
        case 't':
          tabulate(counts->it,genomeOffset,qual[readPosn++]);
          break;
        default:
          tabulate(counts->in,genomeOffset,qual[readPosn++]);
          break;
      }
    } else {
      if (tb1[i] != tb2[i]) {
        posErrors[readPosn]++;
      }
      switch (tb1[i]) {
        case 'A':
        case 'a':
          tabulate(counts->a,genomeOffset++,qual[readPosn++]);
/*          counts->acount[genomeOffset++]++; */
          break;
        case 'C':
        case 'c':
          tabulate(counts->c,genomeOffset++,qual[readPosn++]);
/*          counts->ccount[genomeOffset++]++; */
          break;
        case 'G':
        case 'g':
          tabulate(counts->g,genomeOffset++,qual[readPosn++]);
/*          counts->gcount[genomeOffset++]++; */
          break;
        case 'T':
        case 't':
          tabulate(counts->t,genomeOffset++,qual[readPosn++]);
/*          counts->tcount[genomeOffset++]++; */
          break;
        case '-':
          counts->dcount[genomeOffset++]++;
          break;
        default:
          tabulate(counts->n,genomeOffset++,qual[readPosn++]);
      }
    }
/*
    fprintf(stderr,"%c %c %d %d %d %d %d %d %d\n",tb1[i],tb2[i],
             counts->acount[genomeOffset-1],
             counts->ccount[genomeOffset-1],
             counts->gcount[genomeOffset-1],
             counts->tcount[genomeOffset-1],
             counts->ncount[genomeOffset-1],
             counts->icount[genomeOffset-1],
             counts->dcount[genomeOffset-1]);
*/
  }
}

void printHTMLheader(FILE *fd) {
  fprintf(fd,"<html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"solexa.css\" /><title>PhiX Analysis</title></head>\n<body>\n");
}

void printHTMLfooter(FILE *fd) {
  fprintf(fd,"</body></html>");
}

void dumpCountsHTML(char *phix,int phixlen,counterp cp,FILE *fd) {
  int i;
  char *rowstart,*rowentry,*rowend,*hdrstr,*rowstartflag;

  rowstart = "<tr><td class='zork'>%c</td>";
  rowstartflag = "<tr class='alarm'><td class='zork'>%c</td>";
  rowentry = "<td>%d</td><td>%3.1f</td><td class='zork'>%3.1f</td>";
  rowend = "</tr>\n";
  hdrstr = "<th>count</th><th>qmean</th><th>stdev</th>";
  fprintf(fd,"<table><tr><th>PhiX</th><th colspan=3>A</th><th colspan=3>C</th><th colspan=3>G</th><th colspan=3>T</th><th colspan=3>N</th></tr>\n");
  fprintf(fd,"<tr><th></th>%s%s%s%s%s</tr>\n",hdrstr,hdrstr,hdrstr,hdrstr,hdrstr);
  for (i=0;i<phixlen;i++) {
    if (cp->flag[i] > 0) {
      fprintf(fd,rowstartflag,phix[i]);
    } else {
      fprintf(fd,rowstart,phix[i]);
    }
    fprintf(fd,rowentry,cp->a[i].count,cp->a[i].mean,sqrt(cp->a[i].m2/(cp->a[i].count-1)));
    fprintf(fd,rowentry,cp->c[i].count,cp->c[i].mean,sqrt(cp->c[i].m2/(cp->c[i].count-1)));
    fprintf(fd,rowentry,cp->g[i].count,cp->g[i].mean,sqrt(cp->g[i].m2/(cp->g[i].count-1)));
    fprintf(fd,rowentry,cp->t[i].count,cp->t[i].mean,sqrt(cp->t[i].m2/(cp->t[i].count-1)));
    fprintf(fd,rowentry,cp->n[i].count,cp->n[i].mean,sqrt(cp->n[i].m2/(cp->n[i].count-1)));
    fprintf(fd,rowend);
  }
  fprintf(fd,"</table>\n");
}

void dumpEntry(nucp x) {
  fprintf(stdout,"%d\t%f\t%f\t",
          x->count,x->mean,sqrt(x->m2/(x->count-1)));
}

void dumpCounts(char *phix,int phixlen,counterp cp) {
  int i;
  char *s;

  fprintf(stdout,"PhiX\tA\t\t\tC\t\t\tG\t\t\tT\t\t\tN\t\t\tDeletions\tIA\t\t\tIC\t\t\tIG\t\t\tIT\t\t\tIN\n");
  s = "count\tqmean\tstdev\t";
  fprintf(stdout,"\t%s%s%s%s%s\t%s%s%s%s%s\n",s,s,s,s,s,s,s,s,s,s);
  for (i=0;i<phixlen;i++) {
    fprintf(stdout,"%c\t",phix[i]);
    dumpEntry(cp->a+i);
    dumpEntry(cp->c+i);
    dumpEntry(cp->g+i);
    dumpEntry(cp->t+i);
    dumpEntry(cp->n+i);
    fprintf(stdout,"%d\t",cp->dcount[i]);
    dumpEntry(cp->ia+i);
    dumpEntry(cp->ic+i);
    dumpEntry(cp->ig+i);
    dumpEntry(cp->it+i);
    dumpEntry(cp->in+i);
    fprintf(stdout,"\n");
  }
}

char maxNucl(nucp a,nucp c, nucp g, nucp t, nucp n) {
  int max = a->count;
  char nuc = 'A';
  if (c->count > max) {
    max = c->count;
    nuc = 'C';
  }
  if (g->count > max) {
    max = g->count;
    nuc = 'G';
  }
  if (t->count > max) {
    max = t->count;
    nuc = 'T';
  }
  if (n->count > max) {
    nuc = 'N';
  }
  return nuc;
}

void flagRows(char *phix,int phixlen,counterp cp) {
  int i;

  for (i=0;i<phixlen;i++) {
    /* first check if max count is mismatch */
    if (maxNucl(cp->a+i,cp->c+i,cp->g+i,cp->t+i,cp->n+i) != phix[i]) {
      cp->flag[i]++;
      fprintf(stderr,".");
    }
  }
}

void processSequences(dictionary positions,FILE *reads,sequence phix) {
/*  int *acount,*ccount,*gcount,*tcount,*ncount,*icount,*dcount; */
  char *nptr;
  int okay,missing;
  counterp counter;
  posp node;
  sequence chunk;
  int realstart;
  fastqp seq;
  char chunkstr[200];
  int qual[200];
  char *phixSeq;
  int phixlen;
  int positionErrors[200],i;
  int wantProgress = 0;
  int skipping = 0;
  int usedCount = 0,used = 0;

  if (isatty(fileno(stderr))) {
    wantProgress = 1;
  }
  seq = fq_alloc(100);
  chunk = seq_alloc();
  counter = allocateCounter(phix->length+2);
  calignInit(200,2,-2,-100,-1);
  okay = 0;
  missing = 0;
  for (i=0;i<100;i++) {
    positionErrors[i] = 0;
  }
  fprintf(stderr,"Aligning reads...\n");
  while (fq_next(seq,reads)) {
/*    fprintf(stderr,"id: %s\nseq:  %s\nqual: %s\n",seq->id,seq->data,seq->qual);
    fflush(stderr);
*/
    nptr = strchr(seq->id,':') + 1;
    if ((long) nptr < 2) {
      fprintf(stderr,"ERROR: couldn't find colon in '%s'\n",seq->id);
      exit(-1);
    }
    if (dict_contains(positions,nptr)) {
      node = (posp) dict_get(positions,nptr);
      phredQStr2Int(qual,seq->qual);
      if (node->strand == '-') {
        reverseComplementInPlace(seq->data);
      }
      if (node->start-SLACK < 0) {
        skipping++;
        continue;
      }
      realstart = node->start-SLACK;
      chunk = seq_substr(chunk,phix,realstart,150);
      seq_nstring(chunkstr,chunk,100);
/*      fprintf(stderr,"start: %d  actual: %d\n",node->start,realstart); */
      align(seq->data,qual,chunkstr,realstart,counter,positionErrors,&used); 
      if (used) {
        usedCount++;
      }
      okay++;
    } else {
      missing++;
    }
    if (wantProgress && (okay+missing) % 100000 == 0) {
      fprintf(stderr,"%d\n",okay+missing);
    }
  }
  fprintf(stderr,"%d okay, %d missing, %d skipped, %d used\n",okay,missing,skipping,usedCount);
  fprintf(stderr,"Errors by position:\n");
  for (i=0;i<45;i++) {
    fprintf(stderr,"%5d  %9d\n",i+1,positionErrors[i]);
  }
  fprintf(stderr,"Summarizing results...\n");
  
  phixSeq = seq_string(phix);
  phixlen = strlen(phixSeq);
/*  flagRows(phixSeq,phixlen,counter); */
  dumpCounts(phixSeq,phixlen,counter);
/*
  printHTMLheader(stdout);
  dumpCountsHTML(phixSeq,phixlen,counter,stdout);
  printHTMLfooter(stdout);
*/
}

int main(int argc,char **argv) {
  fastareader phixreader;
  sequence phix;
  char *phixfile,*readfile,*blatfile;
  FILE *hits,*reads;
  dictionary positions;
  char c;
  int searchType = BLAT;

/*
  if (argc != 4) {
    fprintf(stderr,"usage: %s <reference seq> <reads fastq> <blat hits>\n",argv[0]);
    exit(-1);
  }
*/
  while ((c=getopt(argc,argv,"e")) != -1) {
    if (c == 'e') {
      searchType = EXONERATE;
    }
  }
  
  phixfile = argv[optind];
  readfile = argv[optind+1];
  blatfile = argv[optind+2];

  phixreader = fr_open(phixfile);
  phix = fr_next(phixreader);
  fr_close(phixreader);
  fprintf(stderr,"Reference: %s, %d bp\n",seq_getName(phix),phix->length);
  
  positions = dict_alloc("0123456789:");
  hits = fopen(blatfile,"r");
  readHitPositions(positions,hits,searchType);

  reads = fopen(readfile,"r");
  processSequences(positions,reads,phix);
  return 0;
}
