#ifndef __BODEWRITER_H
#define __BODEWRITER_H

#include <string>
#include <fstream>

#include "bode/interval.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Writes intervals to files.  Should probably be named "IntervalWriter".
 */
class Writer {
  public:
    Writer(std::string const &filename,void *header);
    Writer(void);
    virtual ~Writer(void);

    virtual void close(void);
    virtual bool isOpen(void)                                 { return _open; };
    virtual void write(Interval const &i);
    virtual Writer *open(std::string const &filename,void *header);

  protected:
    std::ofstream *_fd;
    bool _open;
};

}

#endif
