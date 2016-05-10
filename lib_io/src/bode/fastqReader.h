#ifndef __FASTQREADER_H
#define __FASTQREADER_H

#include <fstream>
#include "bode/sequence.h"
#include "bode/sequenceReader.h"
#include "bode/fastq.h"

namespace bode {

/** \brief <span class=stat_bad>red</span> A simple, fast reader for fastq sequences.
 *
 * Typical usage is to
 * allocate one object (or however many you need at the same time), and
 * use that to read and process successive Fastq sequences.
 * 
 * Assumptions:
 *  a) No sequence or header will be longer than "maxseqlen".
 *  b) Each sequence will consist of exactly four lines: the header line
 *     one line of sequence data, the second header, and the quality line.
 *     The second header should either be a single "+", or exactly match
 *     the first header, except for the "@" and "+".
 *
 *     N.B.  This is not according to the Fastq spec.  It should support
 *     multi-line sequence and quality lines, using the length of the sequence
 *     line(s) to figure out where the quality values end.
 *     N.B. 2: Note that quality lines can include "@", so there is no
 *     other way (than length) to discriminate correctly between the next
 *     line of a quality line and the header line of the next sequence.
 *  c) The sequence and quality lines should, of course, be the same length.
 *
 * Warning: This class does essentially no error checking.  It is
 * designed for speed, not safety.
 */
class FastqReader: public SequenceReader {
  public:
    FastqReader(std::string const &fn,int ml);
    ~FastqReader(void);
    virtual Sequence *nextS();
    virtual void close(void);
    virtual bool eof(void);
    virtual void *header(void);
    virtual Writer *getWriter(void);
  protected:
    std::string _filename;
    std::ifstream *_fd;
    std::string temp;
    char *buffer;
    int maxlen;
    Fastq *_fq;
/*
    void zeroQual(unsigned start,unsigned length);
 */
};

}

#endif
