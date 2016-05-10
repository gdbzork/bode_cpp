#ifndef __BAMWRITER_H
#define __BAMWRITER_H

#include <htslib/sam.h>

#include "bode/writer.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Writes BAM files.
 */
class BamWriter: public Writer {
  public:
    BamWriter(std::string const &filename,void *header);
    BamWriter(void);
    virtual ~BamWriter(void);

    void close(void);
    bool isOpen(void) { return _open; };
    void write(Interval const &i);
    Writer *open(std::string const &filename,void *header);

  protected:
    samFile *_fd;
    bam_hdr_t *_hdr;
    bool _open;
};

}

#endif
