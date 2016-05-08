#ifndef __TICKER_H
#define __TICKER_H

#include <cstdio>

namespace bode {

enum TickWhen {NEVER,IFTTY,ALWAYS};

/**
 * \brief <span class=stat_bad>red</span> Progress ticker.
 */
class Ticker {

  public:
    Ticker(int frequency);
    Ticker(int frequency,TickWhen when);
    void tick(void) { _ind++;
                      if (_wantOutput && _ind % _freq == 0)
                        fprintf(stderr,"%010d\r",_ind);
                    };

  protected:
    bool _wantOutput;
    int _ind;
    int _freq;

};

}

#endif
