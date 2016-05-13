#include <string>

#include "bode/interval.h"
#include "bode/intervalTreeLeft.h"
#include "bode/intervalSetLeft.h"

void bode::IntervalSetLeft::insert(Interval const *inter) {
  const std::string chrom = inter->chrom();
  if (chroms->count(chrom) == 0) {
    (*chroms)[chrom] = new IntervalTreeLeft();
  }
  (*chroms)[chrom]->insert(inter->left(),inter->right());
}
