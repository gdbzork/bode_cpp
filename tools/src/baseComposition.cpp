/*
 * This software counts base frequencies, per cycle, in a Fastq file.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>

#include <bode/fastq.h>
#include <bode/fastqReader.h>
#include <bode/log.h>
#include <bode/ticker.h>
#include <bode/util.h>

#define LINELENGTH 1024 /* maximum line length */
#define A 0
#define C 1
#define G 2
#define T 3
#define N 4
#define LETTERS 5

int getCycleCount(char *fn) {
  bode::FastqReader *fd;
  int cycles = 0;
  bode::Sequence *s;

  try {
    fd = new bode::FastqReader(std::string(fn),LINELENGTH);
  } catch (std::ios_base::iostate flags) {
    bode::Log::get()->error((char *)"failed to open file '%s'",fn);
    exit(-1);
  }
  
  s = fd->nextS();
  if (s != NULL) {
    cycles = s->seq().length();
  }
  fd->close();
  if (cycles == 0) {
    bode::Log::get()->error((char *)"failed to get cycle count from 1st read.");
    exit(-1);
  }
  return cycles;
}

long **initCounts(int cycles) {
  long **matrix;
  int i;

  matrix = (long **) calloc(cycles,sizeof(long *));
  for (i=0;i<cycles;i++) {
    matrix[i] = (long *) calloc(LETTERS,sizeof(long));
  }
  return matrix;
}

/* read a fastq file, count base composition. */
void processFastq(char *fn,int cycles,long **matrix) {
  bode::Sequence *fq;
  bode::FastqReader *rdr;
  bode::Ticker *tic;
  int count,i;
  std::string data;

  tic = new bode::Ticker(100000);

  rdr = new bode::FastqReader(std::string(fn),LINELENGTH);
  count = 0;
  fq = rdr->nextS();
  while (fq != NULL) {
    tic->tick();
    data = fq->seq();
    for (i=0;i<cycles;i++) {
      switch(data[i]) {
        case 'A': matrix[i][A]++; break;
        case 'C': matrix[i][C]++; break;
        case 'G': matrix[i][G]++; break;
        case 'T': matrix[i][T]++; break;
        default:  matrix[i][N]++;
      }
    }
    fq = rdr->nextS();
  }
  rdr->close();
  delete rdr;
  delete tic;
  return;
}

void dumpMatrix(FILE *fd,int cycles,long **matrix) {
  int i,j,total;
  float ftotal;

  for (i=0;i<cycles;i++) {
    fprintf(fd,"%d",i+1);
    total = 0;
    for (j=0;j<LETTERS;j++) {
      total += matrix[i][j];
    }
    ftotal = (float) total;
    for (j=0;j<LETTERS;j++) {
      fprintf(fd,"\t%f",matrix[i][j]/ftotal);
    }
    fprintf(fd,"\n");
  }
}

int main(int argc,char **argv) {
  char *fn;
  int cycles;
  long **matrix;

  if (argc < 2) {
    fprintf(stderr,"Syntax: baseComposition <file.fq>\n");
    return(-1);
  }
  fn = argv[1];
  
  cycles = getCycleCount(fn);
  matrix = initCounts(cycles);
  processFastq(fn,cycles,matrix);
  fprintf(stdout,"per-cycle base count: %s\n",fn);
  dumpMatrix(stdout,cycles,matrix);
  return 0;
}
