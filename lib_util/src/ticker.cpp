#include <unistd.h>

#include "bode/ticker.h"

#define STDERR 2

bode::Ticker::Ticker(int frequency=10000) {
  _freq = frequency;
  _wantOutput = isatty(STDERR);
  _ind = 0;
}

bode::Ticker::Ticker(int frequency, bode::TickWhen when) {
  _freq = frequency;
  _wantOutput = when==NEVER ? false : (when==ALWAYS ? true : isatty(STDERR));
  _ind = 0;
}
