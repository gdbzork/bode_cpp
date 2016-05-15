#include <fstream>
#include <iostream>

#include "bode/intervalReader.h"
#include "bode/bedReader.h"
#include "bode/writer.h"
#include "bode/interval.h"
#include "bode/intervalSet.h"

bode::IntervalSet *loadReads(bode::IntervalReader *ir) {
  bode::Interval *i;
  bode::IntervalSet *is = new bode::IntervalSet();
  while (i = ir->nextI()) {
    is->insert(i);
  }
  return is;
}

int main(int argc,char **argv) {
  int count;
  bode::Interval *i;
  std::string readsFN(argv[1]);
  std::string inFN(argv[2]);
  std::string outFN(argv[3]);
  int target[16384];

  bode::IntervalReader *reads = bode::IntervalReader::open(readsFN);
  bode::BedReader *in = new bode::BedReader(inFN);
  std::ofstream *out = new std::ofstream(outFN.c_str(),std::ofstream::out);
  
  bode::IntervalSet *is = loadReads(reads);
  reads->close();
  while (i=in->nextI()) {
    int left = i->left();
    int right = i->right();
    if (right - left > 16384) {
      std::cerr << "over-wide interval: "<<i->chrom()<<":"<<left<<"-"<<right<<std::endl;
    } else {
      is->density(i->chrom(),left,right,target);
      *out <<i->chrom()<<":"<<left<<"-"<<right;
      for (int j=0;j<right-left;j++) {
        *out << " " << target[j];
      }
      *out << std::endl;
    }
    count++;
  }
  in->close();
  out->close();

  std::cerr << "processed " << count << " intervals" << std::endl;
  return 0;
}
