#include "bode/intervalSetSplit.h"
#include "bode/interval.h"
#include "bode/locationFilter.h"
#include "bode/intervalReader.h"
#include "bode/writer.h"

bode::LocationFilter::LocationFilter(bode::IntervalReader *regions,bool invert=false) {
  Interval *i;
  _invert = invert;
  _iset = new IntervalSetSplit();
  while ((i=regions->nextI())) {
    _iset->insert(i);
  }
  return;
}

bode::LocationFilter::~LocationFilter(void) {
  delete _iset;
}

int bode::LocationFilter::filter(IntervalReader *in,Writer *out,int &kept,int &total) {
  bode::Interval *i;

  kept = 0;
  total = 0;
  while ((i = in->nextI())) {
    if (keep(i)) {
      out->write(*i);
      kept++;
    }
    total++;
  }
  return 0; // no errors...
}

bool bode::LocationFilter::keep(Interval *i) {
  int overlap = _iset->overlapping(i);
//  fprintf(stderr,"%s:%d-%d olap=%d\n",i->chrom(),i->left(),i->right(),overlap);
  return (overlap > 0 && !_invert) || (overlap == 0 && _invert);
}
