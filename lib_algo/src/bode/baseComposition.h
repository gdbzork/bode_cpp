#ifndef __BASECOMPOSITION_H
#define __BASECOMPOSITION_H

#include <map>
#include <vector>
#include "bode/sequence.h"
#include "bode/ticker.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Calculates base composition.
 */
class BaseComposition {

  public:
    BaseComposition(void);
    ~BaseComposition(void);
    void add(Sequence s);
    int operator[](char c);
    std::map<char,unsigned long> *operator[](unsigned i);

  protected:
    std::vector<unsigned long> *_totalByCycle;
    std::vector< std::map<char,unsigned long> > *_count;
    Ticker *_progbar;

};

}

#endif
