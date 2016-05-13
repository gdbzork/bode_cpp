#ifndef __INTERVALTREESPLIT_H
#define __INTERVALTREESPLIT_H

#include "bode/intervalNode.h"
#include "bode/intervalTree.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> A tree of genomic intervals with a difference.
 */
class IntervalTreeSplit: public IntervalTree {

  public:
    virtual ~IntervalTreeSplit(void) {};

  private:
    int i_countIntervals(IntervalNode *n,int left,int right);
};

}

#endif
