#include <string>
#include <set>
#include <map>

#include "bode/interval.h"
#include "bode/intervalReader.h"
#include "bode/chromCounter.h"
#include "bode/genomeData.h"

bode::ChromCounter::ChromCounter(GenomeData *gd) {
  if (gd) {
    std::vector<std::string> chlist = gd->chroms();
    std::vector<std::string>::iterator chit;
    chit = chlist.begin();
    while (chit != chlist.end()) {
      data[*chit] = 0;
      chit++;
    }
    gdata = gd;
  } else {
    gdata = NULL;
  }
}

void bode::ChromCounter::count(bode::IntervalReader *source) {
  bode::Interval *i;
  while ((i = source->nextI())) {
    std::string const chrom = i->chrom();
    if (data.count(chrom) == 0) {
      data[chrom] = 0;
    }
    data[chrom] = data[chrom] + 1;
  }
}

int bode::ChromCounter::get(std::string const &chrom) {
  if (data.count(chrom) == 0) {
    return 0;
  } else {
    return data[chrom];
  }
}

std::set<std::string> bode::ChromCounter::chroms(void) {
  std::set<std::string> clist;
  std::map<std::string,int>::iterator it;
  it = data.begin();
  while (it != data.end()) {
    clist.insert((*it).first);
    it++;
  }
  return clist;
}
