#ifndef __CIGAR_H
#define __CIGAR_H

#include <string>
#include <exception>

#include <htslib/sam.h>

#define MAX_CIG_LEN 1024

namespace bode {

class illegalCigarEx: public std::exception {
  virtual const char *what() const throw() {
    return "illegal CIGAR string";
  }
};

class cigarTooLongEx: public std::exception {
  virtual const char *what() const throw() {
    return "CIGAR string has too many elements";
  }
};

/// \brief <span class=stat_bad>red</span> A basic CIGAR string class.
///
/// Operates on CIGAR strings.
class Cigar {
  public:
    Cigar(std::string const &src);
    Cigar(void);
    Cigar(uint32_t len,uint32_t *data);
    Cigar(bam1_t *bam);

    uint32_t assign(std::string const &src);
    uint32_t assign(uint32_t len,uint32_t *data);
    uint32_t nElements(std::string const &src);
    uint32_t refLength(void);
    uint32_t asBam(uint32_t *dest);
    std::string repr(void);

  private:
    uint32_t cig[MAX_CIG_LEN];
    uint32_t ciglen;
    static illegalCigarEx illegalCigar;
    static cigarTooLongEx cigarTooLong;
    static const char *bam_cigar_str;
};

}

#endif
