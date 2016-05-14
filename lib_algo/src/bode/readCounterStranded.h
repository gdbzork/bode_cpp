#ifndef __READCOUNTERSTRANDED_H
#define __READCOUNTERSTRANDED_H

#include <string>
#include <fstream>

#include "bode/intervalReader.h"
#include "bode/intervalSetSplit.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Counts reads on intervals.
 */
class ReadCounterStranded {
  public:
    ReadCounterStranded(IntervalReader *reads);
    ~ReadCounterStranded(void);
    void countReadsOnIntervals(IntervalReader *in,std::ofstream *out);
  private:
    IntervalSetSplit *_iset;
};

}

#endif
