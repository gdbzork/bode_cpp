#include <cstdlib>

#include "ticker.h"

int main(int argc,char **argv) {
  bode::Ticker *p;
  char tag = argv[1][0];
  int freq = atoi(argv[2]);
  int stopat = atoi(argv[3]);
  int i;

  switch (tag) {
    case 'a':
      p = new bode::Ticker(freq,bode::ALWAYS); break;
    case 'n':
      p = new bode::Ticker(freq,bode::NEVER); break;
    case 'i':
      p = new bode::Ticker(freq,bode::IFTTY); break;
    case 'd':
      p = new bode::Ticker(freq); break;
  }

  for (i=0;i<stopat;i++) {
    p->tick();
  }
}
