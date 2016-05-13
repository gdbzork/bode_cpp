#include <string>

#include "bode/interval.h"
#include "bode/intervalTreeSplit.h"
#include "bode/intervalSetSplit.h"

void bode::IntervalSetSplit::insert(Interval const *inter) {
  const std::string chrom = inter->chrom();
  if (chroms->count(chrom) == 0) {
    (*chroms)[chrom] = new IntervalTreeSplit();
  }
  (*chroms)[chrom]->insert(inter->left(),inter->right());
}
