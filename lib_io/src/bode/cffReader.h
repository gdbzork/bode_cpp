#ifndef __CFFREADER_H
#define __CFFREADER_H

#include <string.h>
#include <zlib.h>

#include "bode/interval.h"
#include "bode/intervalReader.h"
#include "bode/bedReader.h"
#include "bode/writer.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Reads CFF-format files.
 */
class CffReader: public BedReader {
  public:
    CffReader(std::string const &filename);
    virtual ~CffReader(void);

    static CffReader *open(std::string const &filename);
    virtual Interval *nextI(void);

};

}

#endif
