#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <istream>
#include <cstring>

#include "bode/genomeData.h"

bode::GenomeData::GenomeData(std::istream *fd) {
  std::string chrom;
  int clength;
  
  while (!(fd->eof())) {
    (*fd) >> chrom;
    (*fd) >> clength;
    lengths[chrom] = clength;
  }
  return;
}

//bool bode::GenomeData::chromComp(std::string const &c1,std::string const &c2) {
/*
bool chromComp(std::string const &c1,std::string const &c2) {
  std::stringstream c1conv(c1.substr(3)),c2conv(c2.substr(3));
  int c1val = 0;
  int c2val = 0;
  c1conv >> c1val;
  c2conv >> c2val;
  if (c1val == 0 && c2val == 0) return c1val < c2val;
  if (c1val == 0 && c2val != 0) return false;
  if (c1val != 0 && c2val == 0) return true;
  return c1val < c2val;
}
*/
bool chromComp(std::string const &lhs,std::string const &rhs) {
  int ln = 0,rn = 0;
  char lc[128],rc[128];
  char *ltrailing,*rtrailing;
  strncpy(lc,lhs.c_str(),128);
  strncpy(rc,rhs.c_str(),128);

  if (strncmp(lc,"chr",3) == 0) {
    ln = strtol(lc+3,&ltrailing,10);
  } else {
    ln = strtol(lc,&ltrailing,10);
  }
  if (strncmp(rc,"chr",3) == 0) {
    rn = strtol(rc+3,&rtrailing,10);
  } else {
    rn = strtol(rc,&rtrailing,10);
  }

  if (ln > 0 && rn > 0) {
    if (ln == rn) {
      return strcmp(ltrailing,rtrailing) < 0;
    } else {
      return ln < rn;
    }
  } else {
    return lhs < rhs;
  }
}


std::vector<std::string> bode::GenomeData::chroms(void) {
  std::vector<std::string> clist;
  std::map<std::string,int>::iterator it;
  it = lengths.begin();
  while (it != lengths.end()) {
    clist.push_back(it->first);
  }
  sort(clist.begin(),clist.end(),chromComp);
  return clist;
}

int bode::GenomeData::chromLength(std::string const &chrom) {
  return lengths[chrom];
}

bool bode::GenomeData::exists(std::string const &chrom) {
  return lengths.count(chrom) == 1;
}
