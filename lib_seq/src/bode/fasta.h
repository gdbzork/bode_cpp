#ifndef __FASTA_H
#define __FASTA_H

#include <ostream>
#include "sequence.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Store a Fasta sequence.
 */
class Fasta: public bode::Sequence {

  public:
    Fasta(std::string const &n,std::string const &d,std::string const &s);
    Fasta(void);
    Fasta(Fasta const &other);
    void write(std::ostream &out);
    std::string format(void) const;
    std::string descr(void) const       { return _descr; };
    void setDescr(std::string const &d) { _descr = d; };

  protected:
    std::string _descr;
};

}

#endif
