#ifndef __INTERVAL_READER_H
#define __INTERVAL_READER_H

#include <string>

#include "bode/interval.h"
#include "bode/writer.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Abstract base class for readers of interval-based data formats (bed, bam).
 */
class IntervalReader {
  public:
    virtual ~IntervalReader(void);
    virtual Interval *nextI(void) = 0;
    virtual void close(void) = 0;
    virtual bool eof(void) = 0;
    static IntervalReader *open(std::string const &filename);
    virtual void *header(void) = 0;
    virtual Writer *getWriter(void) = 0;
};

}

#endif
