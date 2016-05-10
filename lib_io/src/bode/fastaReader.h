#ifndef __FASTAREADER_H
#define __FASTAREADER_H

#include <string>
#include "bode/sequenceReader.h"
#include "bode/sequence.h"
#include "bode/fasta.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Reads Fasta files.
 */
class FastaReader: public SequenceReader {
  public:
    FastaReader(std::string const &f);
    ~FastaReader(void);
    virtual Sequence *nextS(void);
    virtual void close(void);
    virtual bool eof(void);
    virtual void *header(void);
    virtual Writer *getWriter(void);
  protected:
    std::string _filename;
    std::ifstream *_fd;
    static size_t const _maxlen = 1024;
    bode::Fasta *_fa;
    char *splitHead(char *s);
    char *stripseq(char *s);
};

}

#endif
