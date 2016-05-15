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

#define BINWIDTH "100"
#define BINS "100"
#define NORM "10000000"

bode::Flags *processCmdLine(int argc,char **argv) {
  bode::Flags *fset = new bode::Flags();

  try {
    fset->add("input",bode::STRING,"","background/input file");
    fset->add("norm",bode::INT,NORM,"number of reads to normalize to");
    fset->add("bins",bode::INT,BINS,"number of bins");
    fset->add("binwidth",bode::INT,BINWIDTH,"width of bins");
    fset->add("dom",bode::BOOL,"false","match Dom's version exactly");
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

  if (fset->getBool("dom")) {
    is = new bode::IntervalSetLeft();
  } else {
    is = new bode::IntervalSetSplit();
  }
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

int getBedWidth(std::string fn) {
  bode::BedReader *fd = new bode::BedReader(fn);
  bode::Interval *i;
  int width = 0;

  while ((i=fd->nextI())) {
    int w = i->right() - i->left();
    if (w > width) {
      width = w;
    }
  }
  fd->close();
  return width;
}

int main(int argc,char **argv) {
  int count,j,s;
  int norm;
  bode::Interval *i;
  std::string readsFN;
  std::string inFN;
  std::string outFN;
  std::string inputFN;
  int readsTotal,inputTotal;
  double readsScale,inputScale;
  int *target;
  int bins,binwidth;
  bode::Interval z;
  bode::Flags *fset;

  fset = processCmdLine(argc,argv);
  norm = fset->getInt("norm");
  bins = fset->getInt("bins");
  binwidth = fset->getInt("binwidth");
  inputFN = fset->getStr("input");
  readsFN = fset->positionalArgs()[1];
  inFN = fset->positionalArgs()[2];
  outFN = fset->positionalArgs()[3];

//  width = getBedWidth(inFN);

  target = new int[bins];
  bode::IntervalReader *reads = bode::IntervalReader::open(readsFN);
  bode::CffReader *in = new bode::CffReader(inFN);
  std::ofstream *out = new std::ofstream(outFN.c_str(),std::ofstream::out);
  bode::IntervalReader *input = NULL;
  if (inputFN != "") {
    input = bode::IntervalReader::open(inputFN);
  }

  *out << "GID\tscore\tcoordinates";
  s = 1;
  while (s <= bins) {
    *out << "\t" << s;
    s++;
  }
  *out << std::endl;
  
  fprintf(stderr,"loading reads...\n");
  bode::IntervalSet *is = loadReads(reads,fset);
  readsTotal = is->count();
  fprintf(stderr,"loaded %d reads.\n",readsTotal);
  readsScale = ((double) norm) / (double) readsTotal;
  bode::IntervalSet *background = NULL;
  if (input != NULL) {
    fprintf(stderr,"loading input...\n");
    background = loadReads(input,fset);
    inputTotal = background->count();
    fprintf(stderr,"loaded %d input reads.\n",inputTotal);
    inputScale = ((double) norm) / (double) inputTotal;
  }
    
  reads->close();
  count = 0;
  fprintf(stderr,"processing intervals...\n");
  while ((i=in->nextI())) {
    bode::Bed *bi = static_cast<bode::Bed*>(i);
    count++;
    if (count % 1000 == 0) {
      fprintf(stderr,"%9d\r",count);
    }
    for (j=0;j<bins;j++) {
      target[j] = 0;
    }
    int left = bi->left() - (bins / 2) * binwidth;
    int right = bi->left() + (bins / 2) * binwidth;
//    is->density(i->chrom(),left,right,target);
    (*out) <<bi->name() << "\t"<<bi->score()<<"\t"<<bi->chrom()<<":"<<bi->left();
    s = left;
    while (s < right) {
      z.update(i->chrom(),s,s+binwidth);
      int c = is->overlapping(&z);
      int bk;
      double ncbk;
      if (background != NULL) {
        bk = background->overlapping(&z);
        ncbk = (c * readsScale) - (bk * inputScale);
      } else {
        ncbk = c * readsScale;
      }
      *out << "\t" << ncbk;
      s += binwidth;
    }
/*    for (int j=0;j<right-left;j++) {
      *out << "\t" << target[j];
    }
*/
    *out << std::endl;
  }
  in->close();
  out->close();
  if (input != NULL) {
    input->close();
  }

  std::cerr << "processed " << count << " intervals." << std::endl;
  return 0;
}
