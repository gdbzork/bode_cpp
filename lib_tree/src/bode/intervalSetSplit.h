#ifndef __INTERVALSETSPLIT_H
#define __INTERVALSETSPLIT_H

#include "bode/interval.h"
#include "bode/intervalSet.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> IntervalSet which does something I don't remember.
 */
class IntervalSetSplit: public IntervalSet {
  public:
    void insert(Interval const *inter);
};

}

#endif
