#include <fstream>
#include <iostream>

#include "bode/intervalReader.h"
#include "bode/bedReader.h"
#include "bode/cffReader.h"
#include "bode/writer.h"
#include "bode/interval.h"
#include "bode/intervalSet.h"
#include "bode/intervalSetLeft.h"
#include "bode/intervalSetSplit.h"
#include "bode/flags.h"
#include "bode/bodeException.h"
#include "bode/bed.h"

#define VERSION "1.0"
#define NORM "20000000"
#define MAXINTERVAL 16384

bode::Flags *processCmdLine(int argc,char **argv) {
  bode::Flags *fset = new bode::Flags();

  try {
    fset->add("norm",bode::INT,NORM,"number of reads to normalize to");
    fset->add("version",bode::BOOL,"false","show version of the program");
    fset->add("help",bode::BOOL,"false","display help on program");
    fset->parse(argc,argv);
  } catch (bode::FlagException fe) {
    std::cerr << fe.msg() << std::endl;
  }
  if (fset->isSet("version") && fset->getBool("version")) {
    std::cerr << "version: " << VERSION << std::endl;
    exit(0);
  }
  if (fset->positionalArgs().size() != 4 || !fset->okay() || fset->isSet("help")) {
    std::cerr << "version: " << VERSION << std::endl;
    std::cerr << "usage: makeCDT [options] <reads> <regions> <output>" << std::endl;
  std::cerr << "options:" << std::endl;
    fset->printHelp(std::cerr);
    exit(0);
  }
  return fset;
}

bode::IntervalSet *loadReads(bode::IntervalReader *ir,bode::Flags *fset) {
  bode::Interval *i;
  bode::IntervalSet *is;
  int count;

  is = new bode::IntervalSetSplit();
  count = 0;
  while ((i = ir->nextI())) {
    is->insert(i);
    count++;
    if (count % 1000000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
  }
  return is;
}

int main(int argc,char **argv) {
  int count,j,s;
  int norm;
  bode::Interval *i;
  bode::Bed *b;
  std::string readsFN;
  std::string inFN;
  std::string outFN;
  int readsTotal;
  double readsScale;
  int *target;
  bode::Flags *fset;

  fset = processCmdLine(argc,argv);
  readsFN = fset->positionalArgs()[1];
  inFN = fset->positionalArgs()[2];
  outFN = fset->positionalArgs()[3];
  norm = fset->getInt("norm");

  target = new int[MAXINTERVAL];
  bode::IntervalReader *reads = bode::IntervalReader::open(readsFN);
  bode::BedReader *in = new bode::BedReader(inFN);
  std::ofstream *out = new std::ofstream(outFN.c_str(),std::ofstream::out);

  fprintf(stderr,"loading reads...\n");
  bode::IntervalSet *is = loadReads(reads,fset);
  readsTotal = is->count();
  fprintf(stderr,"loaded %d reads.\n",readsTotal);
  readsScale = ((double) norm) / (double) readsTotal;
  reads->close();
  count = 0;
  fprintf(stderr,"processing intervals...\n");
  while ((i=in->nextI())) {
    count++;
    if (count % 1000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
    int width = i->right() - i->left();
    for (j=0;j<width;j++) {
      target[j] = 0;
    }
    is->density(i->chrom(),i->left(),i->right(),target);
    if (i->strand() == '-') {
      for (int k=0;k<width/2;k++) {
        int x = target[k];
        target[k] = target[width-k-1];
        target[width-k-1] = x;
      }
    }
    double nc = target[0] * readsScale;
    b = static_cast<bode::Bed *>(i);
    *out << b->name();
    for (s=0;s<width;s++) {
      nc = target[s] * readsScale;
      *out << "\t" << nc;
    }
    *out << std::endl;
  }
  in->close();
  out->close();

  std::cerr << "processed " << count << " intervals." << std::endl;
  return 0;
}
