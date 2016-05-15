#include <iostream>

#include "bode/intervalReader.h"
#include "bode/histogram.h"

int main(int argc,char **argv) {
  bode::Histogram *h;
  bode::IntervalReader *r;
  std::string fn;
  bode::Interval *i;
  int j;
  int line;

  fn = std::string(argv[1]);
  h = new bode::Histogram(0,100);
  r = bode::IntervalReader::open(fn);

  i = r->nextI();
  line = 0;
  while (i != NULL) {
    int s;
    line++;
    s = (int) i->score();
    if (s < 0 || s > 100) {
      std::cerr << "Score = " << s << " at line " << line << std::endl;
    } else {
      h->increment((int)i->score());
    }
    i = r->nextI();
  }

  for (j=0;j<=100;j++) {
    std::cout << j << "\t" << h->get(j) << std::endl;
  }
  return 0;
}
