#ifndef __SEQUENCE_READER_H
#define __SEQUENCE_READER_H

#include <string>

#include "bode/sequence.h"
#include "bode/writer.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Abstract superclass for classes that read sequences (Fasta, Fastq, BAM)
 */
class SequenceReader {
  public:
    virtual ~SequenceReader(void);
    virtual Sequence *nextS(void) = 0;
    virtual void close(void) = 0;
    virtual bool eof(void) = 0;
    static SequenceReader *open(std::string const &filename);
    virtual void *header(void) = 0;
    virtual Writer *getWriter(void) = 0;
};

}

#endif
