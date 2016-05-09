#ifndef __FASTQ_H
#define __FASTQ_H

#include <string>
#include <ostream>
#include "sequence.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Stores a Fastq sequence.
 */
class Fastq: public Sequence {
  public:
    Fastq(std::string const &n,std::string const &s,std::string const &q);
    Fastq(void);
    Fastq(Fastq const &other);
    virtual ~Fastq(void);
    void writeFasta(std::ostream &f);
    void write(std::ostream &f);
    std::string format(void) const;
    std::string formatFasta(void);
    void setQual(std::string const &q) { _qual = q; };
    std::string qual(void)             { return _qual; };
  protected:
    std::string _qual;
};

}

#endif
