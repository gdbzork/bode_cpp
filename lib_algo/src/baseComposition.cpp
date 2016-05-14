/*
 * Base composition counter, version 1.0 by Gord Brown.
 *
 * This code is in the public domain.  Do whatever you like with it, but
 * please do not remove my name.  Unless a bug in the code causes a terrible
 * tragedy in which thousands perish.  Then please do remove my name.
 *
 * No guarantees, expressed or implied.
 *
 * This software counts base frequencies per cycle, in a Fastq file.
 * Anything other than ACGT is counted as N.
 *
 */
#include "bode/ticker.h"
#include "bode/baseComposition.h"

bode::BaseComposition::BaseComposition(void) {
  _totalByCycle = new std::vector<unsigned long>;
  _count = new std::vector< std::map<char,unsigned long> >;
  _progbar = new Ticker(100000);
}

bode::BaseComposition::~BaseComposition(void) {
  delete _totalByCycle;
  delete _count;
  delete _progbar;
}

void bode::BaseComposition::add(Sequence s) {
  std::string sq = s.seq();
  int i,m;
  m = sq.length();
  for (i=0;i<m;i++) {
    (*_totalByCycle)[i]++;
    (*_count)[i][sq[i]]++;
  }
  _progbar->tick();
}


int bode::BaseComposition::operator[](char c) {
  return 0;
}

std::map<char,unsigned long> *bode::BaseComposition::operator[](unsigned i) {
  return NULL;
}
