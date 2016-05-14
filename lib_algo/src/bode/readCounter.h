#ifndef __READCOUNTER_H
#define __READCOUNTER_H

#include <string>
#include <fstream>

#include "bode/intervalReader.h"
#include "bode/intervalSetSplit.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Counts reads on intervals.
 */
class ReadCounter {
  public:
    ReadCounter(IntervalReader *reads);
    ~ReadCounter(void);
    void countReadsOnIntervals(IntervalReader *in,std::ofstream *out);
  private:
    IntervalSetSplit *_iset;
};

}

#endif
