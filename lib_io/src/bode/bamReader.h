#ifndef __BAMREADER_H
#define __BAMREADER_H

#include <htslib/sam.h>

#include "bode/interval.h"
#include "bode/sequence.h"
#include "bode/intervalReader.h"
#include "bode/sequenceReader.h"
#include "bode/bam.h"
#include "bode/writer.h"
#include "bode/bamWriter.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Reads BAM files.
 */
class BamReader: public IntervalReader, public SequenceReader {
  public:
    BamReader(std::string const &filename);
    BamReader(void);
    virtual ~BamReader(void);

    Interval *nextI(void);
    Sequence *nextS(void);
    void close(void);
    bool eof(void)                                      { return _eof; };
    void *header(void)                                  { return _hdr; };
    static BamReader *open(std::string const &filename);
    Writer *getWriter(void)                         { return new BamWriter(); };

  protected:
    samFile *_fd;
    bam_hdr_t *_hdr;
    bam1_t *_seq;
    Bam *_bseq;
    bool _eof;
};

}

#endif
