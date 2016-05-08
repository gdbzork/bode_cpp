#ifndef __CHROMOSOME_H
#define __CHROMOSOME_H
#include <iostream>
#include <fstream>
#include <string>

namespace bode {

class Chromosome {
  public:
    Chromosome(std::string &src);
    Chromosome(char *src);
    Chromosome &operator=(Chromosome const &rhs);
    bool operator==(Chromosome const &rhs) const;
    bool operator<(Chromosome const &rhs) const;
  private:
    friend std::ostream& operator<<(std::ostream &os, const Chromosome &obj);
    static const int maxlen = 128;
    char data[maxlen];
};

std::ostream& operator<<(std::ostream &os, const Chromosome &obj) {
  os << obj.data;
  return os;
}

}

#endif // __CHROMOSOME_H
