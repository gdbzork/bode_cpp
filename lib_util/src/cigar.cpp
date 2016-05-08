#include <string>
#include <sstream>

#include <htslib/sam.h>

#include "bode/cigar.h"

const char *bode::Cigar::bam_cigar_str = BAM_CIGAR_STR;

bode::Cigar::Cigar(std::string const &src) {
  assign(src);
}

bode::Cigar::Cigar(void) {
  ciglen = 0;
}

bode::Cigar::Cigar(uint32_t len,uint32_t *data) {
  for (int i=0;i<len;i++) {
    cig[i] = data[i];
  }
  ciglen = len;
}

bode::Cigar::Cigar(bam1_t *bam) {
  uint32_t *data;
  ciglen = bam->core.n_cigar;
  data = bam1_cigar(bam);
  for (int i=0;i<ciglen;i++) {
    cig[i] = data[i];
  }
}

std::string bode::Cigar::repr(void) {
  std::stringstream dest;
  for (uint32_t i=0;i<ciglen;i++) {
    uint32_t op,val;
    op = cig[i]&BAM_CIGAR_MASK;
    val = cig[i]>>BAM_CIGAR_SHIFT;
    dest << val;
    dest << BAM_CIGAR_STR[op];
  }
  return dest.str();
}

uint32_t bode::Cigar::nElements(std::string const &src) {
  const char *csrc = src.c_str();
  uint32_t clen = src.length();
  bool dig = false;
  uint32_t n_elem = 0;

  for (int i=0;i<clen;i++) {
    if (isdigit(csrc[i])) {
      dig = true;
    } else if (strchr(BAM_CIGAR_STR,csrc[i]) != NULL) {
      if (!dig) { // haven't seen a digit yet
        throw illegalCigar;
      }
      dig = false;
      n_elem += 1;
      if (n_elem > MAX_CIG_LEN) {
        throw cigarTooLong;
      }
    } else {
      throw illegalCigar; // unknown character
    }
  }
  if (dig) {
    throw illegalCigar; // must end with non-digit
  }
  return n_elem;
}

uint32_t bode::Cigar::assign(std::string const &src) {
  const char *csrc = src.c_str();
  uint32_t clen = src.length();
  uint32_t n_elem = nElements(src); // throws if invalid src
  uint32_t ind = 0;
  uint32_t op,val,pos=0;

  for (int i=0;i<clen;i++) {
    if (!isdigit(csrc[i])) {
      const char *p = strchr(bam_cigar_str,csrc[i]);
      op = p - bam_cigar_str; // offset of char into BAM_CIGAR_STR
      val = atoi(csrc+ind);
      cig[pos++] = val<<BAM_CIGAR_SHIFT | op;
      ind = i+1;
    }
  }
  ciglen = n_elem;
  return n_elem;
}

uint32_t bode::Cigar::assign(uint32_t len,uint32_t *data) {
  for (int i=0;i<len;i++) {
    cig[i] = data[i];
  }
  ciglen = len;
}

uint32_t bode::Cigar::refLength(void) {
  uint32_t len = 0;
  for (int i=0;i<ciglen;i++) {
    uint32_t op = cig[i] & BAM_CIGAR_MASK;
    switch (op) {
      case BAM_CMATCH:
      case BAM_CDEL:
      case BAM_CREF_SKIP:
      case BAM_CPAD:
      case BAM_CEQUAL:
      case BAM_CDIFF:
        len += cig[i] >> BAM_CIGAR_SHIFT;
    }
  }
  return len;
}

uint32_t bode::Cigar::asBam(uint32_t *dest) {
  for (int i=0;i<ciglen;i++) {
    dest[i] = cig[i];
  }
  return ciglen;
}
