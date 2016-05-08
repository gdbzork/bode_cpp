#include <string>
#include <vector>
#include <map>
#include <istream>

#ifndef GENOME_DATA_H
#define GENOME_DATA_H

namespace bode {

class GenomeData {
  public:
    GenomeData(std::istream *fd);
    std::vector<std::string> chroms(void);
    int chromLength(std::string const &chrom);
    bool exists(std::string const &chrom);
  private:
//    bool chromComp(std::string const &c1,std::string const &c2);
    std::map<std::string,int> lengths;
};

}

#endif
