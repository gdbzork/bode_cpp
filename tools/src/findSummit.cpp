#include <fstream>
#include <iostream>

#include "bode/intervalReader.h"
#include "bode/bedReader.h"
#include "bode/writer.h"
#include "bode/interval.h"
#include "bode/intervalSet.h"
#include "bode/intervalSetSplit.h"

#define BINWIDTH 100

bode::IntervalSet *loadReads(bode::IntervalReader *ir) {
  bode::Interval *i;
  bode::IntervalSet *is = new bode::IntervalSetSplit();
  while (i = ir->nextI()) {
    is->insert(i);
  }
  return is;
}

int getBedWidth(std::string fn) {
  bode::BedReader *fd = new bode::BedReader(fn);
  bode::Interval *i;
  int width = 0;

  while (i=fd->nextI()) {
    int w = i->right() - i->left();
    if (w > width) {
      width = w;
    }
  }
  fd->close();
  return width;
}

int summit(int positions,int *counts) {
  int i,p,h;
  p = 0;
  h = 0;
  for (i=0;i<positions;i++) {
    if (counts[i] > h) {
      h = counts[i];
      p = i;
    }
  }
  if (h < 3) { // wasn't much of a summit
    p = positions / 2; // so just pick the middle
  }
  return p;
}

int main(int argc,char **argv) {
  int count,j,s,t;
  bode::Interval *i;
  std::string readsFN(argv[1]);
  std::string inFN(argv[2]);
  std::string outFN(argv[3]);
  int *target;
  int width;
  bode::Interval z;

  width = getBedWidth(inFN);
  std::cerr << "max bed width = " << width << std::endl;

  target = new int[width];
  bode::IntervalReader *reads = bode::IntervalReader::open(readsFN);
  bode::BedReader *in = new bode::BedReader(inFN);
  std::ofstream *out = new std::ofstream(outFN.c_str(),std::ofstream::out);

  bode::IntervalSet *is = loadReads(reads);
  reads->close();
  count = 0;
  while (i=in->nextI()) {
    count++;
    for (j=0;j<width;j++) {
      target[j] = 0;
    }
    int left = i->left();
    int right = i->right();
    is->density(i->chrom(),left,right,target);
    int p = summit(right-left,target) + left;
    int middle = (right - left) / 2 + left;
    int offset = p - middle;
    *out << i->chrom() << "\t" << p << "\t" << p+1 << std::endl;
  }
  in->close();
  out->close();

  std::cerr << "processed " << count << " intervals" << std::endl;
  return 0;
}
