#ifndef _ALIGNERENDSFREE_H
#define _ALIGNERENDSFREE_H

#include "aligner.h"
#include "alignment.h"

namespace bode {

/** Aligns short strings, relatively quickly, in memory.
 *
 * simple, fast alignment routines for short-ish strings (uses O(nm) space)
 *
 */

/**
 * \brief <span class=stat_bad>red</span> Aligns short strings, relatively
 * quickly, in memory, using end-gaps-free alignment.
 *
 * */
class AlignerEndsFree: public Aligner {

  public:
    AlignerEndsFree(int ms,int ma,int mi,int go,int ge);

  protected:
    void init(int str1len,int str2len);
    int stopTB(int i,int j);
    int findEnd();
};

}

#endif
