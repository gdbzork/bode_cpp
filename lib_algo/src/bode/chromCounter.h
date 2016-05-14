#include <string>
#include <set>
#include <map>
#include "bode/intervalReader.h"
#include "bode/genomeData.h"

#ifndef CHROM_COUNTER_H
#define CHROM_COUNTER_H

namespace bode {

class ChromCounter {
  public:
    ChromCounter(GenomeData *gd);
    void count(IntervalReader *source);
    int get(std::string const &chrom);
    std::set<std::string> chroms(void);

  private:
    std::map<std::string,int> data;
    GenomeData *gdata;
};

}

#endif
