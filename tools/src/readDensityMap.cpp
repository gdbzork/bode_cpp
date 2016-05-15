#include <fstream>
#include <iostream>
#include <map>
#include <cmath>

#include "bode/intervalReader.h"
#include "bode/intervalTree.h"
#include "bode/intervalTreeSplit.h"
#include "bode/intervalReader.h"
#include "bode/histogram.h"
#include "bode/ticker.h"

#define HIST_MAX 1024
#define BIN_WIDTH 1024
#define MIN_BINS 2

long loadGenomeMap(std::string const &fn,std::map<std::string,int> &sizes) {
  FILE *fd;
  char buffer[1024];
  int clen;
  char cname[512];
  long total;
  fd = fopen(fn.c_str(),"r");
  total = 0;
  while (fgets(buffer,1024,fd)) {
    sscanf(buffer,"%s %d",cname,&clen);
    sizes[cname] = clen;
    total += clen;
  }
  fclose(fd);
  return total;
}

int loadReads(std::string const &fn,
              std::map<std::string,bode::IntervalTree *> &data) {
  bode::IntervalReader *irdr;
  bode::Interval *ival;
  bode::Ticker t(100000);
  int count;

  irdr = bode::IntervalReader::open(fn);
  count = 0;
  while ((ival = irdr->nextI())) {
    if (ival->isMapped() && data.count(ival->chrom()) > 0) {
      data[ival->chrom()]->insert(ival->left(),ival->right());
      t.tick();
      count++;
    }
  }
  irdr->close();
  return count;
}

int processChrom(bode::Histogram &hist,int crlen,bode::IntervalTree *data,std::string &chrom,int threshold) {
  int i,c,found;
  int left,right,reads;
//  int n;
//  double m,m2,delta,stdev;

//  n=0; m=0.0; m2=0.0;
  found = 0;
  left = 0;
  right = -1;
  reads = 0;
  for (i=0;i<crlen;i+=BIN_WIDTH) {
    c = data->reads(i,i+BIN_WIDTH);
//    n++;
//    delta = c - m;
//    m = m + delta / n;
//    m2 = m2 + delta * (c-m);
    if (c > threshold) {
//      fprintf(stdout,"%s\t%d\t%d\t%s:%d-%d\t%d\t.\n",chrom.c_str(),i,i+BIN_WIDTH,chrom.c_str(),i,i+BIN_WIDTH,c);
      found++;
      if (right == i) { // its the adjacent bin
        right = i+BIN_WIDTH;
        reads += c;
      } else {
        left = i;
        right = i+BIN_WIDTH;
        reads = c;
      }
    } else {
      if (right == i) {
        fprintf(stdout,"%s\t%d\t%d\t%s:%d-%d\t%d\t.\n",chrom.c_str(),left,right,chrom.c_str(),left,right,reads);
      }
      left = -1;
      right = -1;
      reads = 0;
    }
    if (c > HIST_MAX) {
      c = HIST_MAX;
    }
    hist.increment(c);
  }
//  stdev = sqrt(m2/n);
//  fprintf(stderr,"%s: mean=%f  stdev=%f\n",chrom.c_str(),m,stdev);
  return found;
}

double perc(long a,long b) {
  double d;
  d = ((double) a) / (double) b;
  d = ((int) (d * 100000.0)) / 1000.0;
  return d;
}

int main(int argc,char **argv) {

  std::map<std::string,int> chromSizes;
  std::map<std::string,bode::IntervalTree *> chromData;
  std::map<std::string,int>::iterator chromIt;
  bode::Histogram hist(0,HIST_MAX);
  std::string chrom;
  int c,threshold,marked;
  long total;

  threshold = atoi(argv[2]);
  /* load genome map */
  fprintf(stderr,"loading chrom map... ");
  total = loadGenomeMap(argv[1],chromSizes);
  fprintf(stderr,"%d chromosomes\n",(int)chromSizes.size());
  /* load reads */
  chromIt = chromSizes.begin();
  while (chromIt != chromSizes.end()) {
    chromData[chromIt->first] = new bode::IntervalTreeSplit();
    chromIt++;
  }
  fprintf(stderr,"loading reads... \n");
  c = loadReads(argv[3],chromData);
  chromIt = chromSizes.begin();
  fprintf(stderr,"loaded %d reads.\n",c);
  marked = 0;
  while (chromIt != chromSizes.end()) {
    chrom = chromIt->first;
    marked += processChrom(hist,chromSizes[chrom],chromData[chrom],chrom,threshold);
    chromIt++;
  }
//  for (i=0;i<=HIST_MAX;i++) {
//    fprintf(stdout,"%d\t%d\n",i,hist.get(i));
//  }
  
  fprintf(stderr,"%d\t%d\t%ld\t%.3f%%\n",threshold,marked*BIN_WIDTH,total,perc(marked*BIN_WIDTH,total));
}
