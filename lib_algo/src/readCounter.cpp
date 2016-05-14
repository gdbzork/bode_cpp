#include <string>
#include <fstream>
#include <iostream>

#include "bode/interval.h"
#include "bode/intervalSetSplit.h"
#include "bode/readCounter.h"
#include "bode/intervalReader.h"
#include "bode/bedReader.h"
#include "bode/writer.h"
#include "bode/util.h"

bode::ReadCounter::ReadCounter(IntervalReader *reads) {
  bode::Interval *i;

  _iset = new bode::IntervalSetSplit();
  while ((i = reads->nextI())) {
    _iset->insert(i);
  }
}

bode::ReadCounter::~ReadCounter(void) {
  delete _iset;
}

void bode::ReadCounter::countReadsOnIntervals(IntervalReader *in,std::ofstream *out) {
  bode::Interval *i;
  int c;
  bode::Bed *bi;
  int tot;

  tot = 0;
  while ((i = in->nextI())) {
    bi = dynamic_cast<bode::Bed *>(i);
    c = _iset->overlapping(i);
/*
    strcpy(tmpnam,bi->name().c_str());
    spnum = splitchar(tmpnam,flds,'_',2);
    if (spnum != 2) {
      fprintf(stderr,"shit!\n");
    }
    (*out) << c << "\t" << flds[0] << "\t" << flds[1] << std::endl;
*/
    (*out) << c << "\t" << bi->name() << std::endl;
/*    *out << bi->chrom() << "\t" << bi->left() << "\t" << bi->right() << "\t" << c << std::endl; */
    tot++;
  }
}
