#include <iostream>
#include "bode/chromCounter.h"
#include "bode/intervalReader.h"

int main(int argc,char **argv) {
  std::string fname(argv[1]);
  bode::ChromCounter cd(NULL);
  
  bode::IntervalReader *src;

  src = bode::IntervalReader::open(fname);
  cd.count(src);

  std::set<std::string> clist = cd.chroms();
  std::set<std::string>::iterator it;
  it = clist.begin();
  while (it != clist.end()) {
    std::cout << cd.get(*it) << "\t" << *it << std::endl;
    it++;
  }
}
