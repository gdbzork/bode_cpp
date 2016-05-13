#ifndef __INTERVALTREELEFT_H
#define __INTERVALTREELEFT_H

#include "bode/intervalNode.h"
#include "bode/intervalTree.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> A tree of genomic intervals with a difference.
 */
class IntervalTreeLeft: public IntervalTree {

  public:
    virtual ~IntervalTreeLeft(void) {};

  private:
    int i_countIntervals(IntervalNode *n,int left,int right);
};

}

#endif
