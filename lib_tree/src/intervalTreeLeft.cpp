#include <algorithm>

#include "bode/intervalTreeLeft.h"

int bode::IntervalTreeLeft::i_countIntervals(bode::IntervalNode *n,int left,int right) {
  int count = 0;

  if (n == NULL) {
    return 0;
  }
  if (left < n->r()) {
    count += i_countIntervals(n->getLeft(),left,right);
  }
  if (right >= n->l()) {
    count += i_countIntervals(n->getRight(),left,right);
  }
  if (left <= n->l() && right > n->l()) {
    // count if left end is in interval
    count += n->getCount();
  }
  return count;
}

