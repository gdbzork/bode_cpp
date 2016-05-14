#ifndef __LOCATIONFILTER_H
#define __LOCATIONFILTER_H

#include "bode/interval.h"
#include "bode/intervalSetSplit.h"
#include "bode/intervalReader.h"
#include "bode/writer.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Filters reads by genomic location.
 */
class LocationFilter {
  public:
    LocationFilter(IntervalReader *regions,bool invert);
    virtual ~LocationFilter(void);
    virtual int filter(IntervalReader *in,Writer *out,int &kept,int &total);
    virtual bool keep(Interval *i);
  private:
    IntervalSetSplit *_iset;
    bool _invert;

};

}

#endif
