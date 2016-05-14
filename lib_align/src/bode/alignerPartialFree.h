#ifndef _ALIGNERPARTIAL_H
#define _ALIGNERPARTIAL_H

#include "alignment.h"
#include "aligner.h"

namespace bode {

/** Aligns short strings, relatively quickly, in memory.
 *
 * simple, fast alignment routines for short-ish strings (uses O(nm) space)
 *
 */

/**
 * \brief <span class=stat_bad>red</span> Aligns reads using partial-free alignment.
 *
 * */
class AlignerPartialFree: public Aligner {

  public:
    AlignerPartialFree(int ms,int ma,int mi,int go,int ge);

  protected:
    void init(int str1len,int str2len);
    int stopTB(int i,int j);
    int findEnd();
};

}

#endif
