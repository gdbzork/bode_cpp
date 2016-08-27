#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <ctype.h>

#include <htslib/sam.h>

typedef struct {
  uint32_t val;
  uint32_t op;
} cigmatch;

int validCigar(std::string const &src) {
  const char *csrc = src.c_str();
  int clen = src.length();
  bool dig = false;
  uint32_t n_elem = 0;
  
  for (int i=0;i<clen;i++) {
    if (isdigit(csrc[i])) {
      dig = true;
    } else if (strchr(BAM_CIGAR_STR,csrc[i]) != NULL) {
      if (!dig) { // haven't seen a digit yet
        return -1; // invalid, 2 non-digits in a row
      }
      dig = false;
      n_elem += 1;
    } else {
      return -1; // unknown character
    }
  }
  if (dig) {
    return -1; // must end with non-digit
  }
  return n_elem;
}

int parseCigar(std::string src,int ndest,cigmatch *mat) {
  const char *csrc = src.c_str();
  int clen = src.length();
  bool dig = false;
  uint32_t n_elem = 0;
  int ind;
  
  ind = 0;
  for (int i=0;i<clen;i++) {
    if (isdigit(csrc[i])) {
      dig = true;
    } else {
      const char *bcs = BAM_CIGAR_STR;
      const char *p = strchr(bcs,csrc[i]);
      if (p != NULL) {
        if (!dig) { // haven't seen a digit yet
          return -1; // invalid, 2 non-digits in a row
        }
        dig = false;
        mat[n_elem].op = p - bcs; // offset of char into BAM_CIGAR_STR
        mat[n_elem].val = atoi(csrc+ind);
        ind = i+1;
        n_elem += 1;
      } else {
        return -1; // invalid character
      }
    }
  }
  if (dig) {
    return -1; // must end with non-digit
  }
  return n_elem;
}

int main(int argc,char **argv) {
  int rc;
  cigmatch mat[10];

  for (int i=1;i<argc;i++) {
    std::string src(argv[i]);
    int nel = validCigar(src);
    if (nel < 0) {
      std::cout << argv[i] << ": no match" << std::endl;
    } else {
      std::cout << argv[i] << ": " << nel << " elements" << std::endl;
    }
    nel = parseCigar(src,10,mat);
    if (nel < 0) {
      std::cout << argv[i] << ": no match" << std::endl;
    } else {
      std::cout << argv[i] << ": ";
      for (int j=0;j<nel;j++) {
        std::cout << "[" << mat[j].val << "," << BAM_CIGAR_STR[mat[j].op] << "] ";
      }
      std::cout << std::endl;
    }
  }
  return 0;
}
