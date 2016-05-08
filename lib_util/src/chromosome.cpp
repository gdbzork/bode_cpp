#include <string>
#include <cstring>
#include <cstdlib>

#include "bode/chromosome.h"

bode::Chromosome::Chromosome(std::string &src) {
  strncpy(data,src.c_str(),maxlen);
}

bode::Chromosome::Chromosome(char *src) {
  strncpy(data,src,maxlen);
}

bode::Chromosome &bode::Chromosome::operator=(Chromosome const &rhs) {
  strncpy(data,rhs.data,maxlen);
  return *this;
}

bool bode::Chromosome::operator==(Chromosome const &rhs) const {
  return strcmp(data,rhs.data) == 0;
}

bool bode::Chromosome::operator<(Chromosome const &rhs) const {
  int ln = 0,rn = 0;
  char *ltrailing,*rtrailing;

  if (strncmp(data,"chr",3) == 0) {
    ln = strtol(data+3,&ltrailing,10);
  } else {
    ln = strtol(data,&ltrailing,10);
  }
  if (strncmp(rhs.data,"chr",3) == 0) {
    rn = strtol(rhs.data+3,&rtrailing,10);
  } else {
    rn = strtol(rhs.data,&rtrailing,10);
  }

  if (ln > 0 && rn > 0) {
    if (ln == rn) {
      return strcmp(ltrailing,rtrailing) < 0;
    } else {
      return ln < rn;
    }
  } else {
    return strcmp(data,rhs.data) < 0;
  }
}
