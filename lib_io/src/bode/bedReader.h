#ifndef __BEDREADER_H
#define __BEDREADER_H

#include <string.h>
#include <zlib.h>

#include "bode/interval.h"
#include "bode/intervalReader.h"
#include "bode/bed.h"
#include "bode/writer.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Reads BED-format files.
 */
class BedReader: public IntervalReader {
  public:
    BedReader(std::string const &filename);
    virtual ~BedReader(void);

    virtual Interval *nextI(void);
    virtual void close(void);
    virtual bool eof(void)                                     { return _eof; };
    virtual void *header(void)                                 { return NULL; };
    static BedReader *open(std::string const &filename);
    virtual Writer *getWriter(void)                    { return new Writer(); };

  protected:
    void trimLF(char *str);
    int splits(char *str,char **dest,int max);
    static int const maxLine = 1024;
    gzFile _fd;
    Bed *_bseq;
    char *_buffer;
    bool _eof;
};

}

#endif
