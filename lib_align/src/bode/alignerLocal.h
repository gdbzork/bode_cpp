#ifndef _ALIGNERLOCAL_H
#define _ALIGNERLOCAL_H

#include "alignment.h"
#include "aligner.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Aligns sequences using local alignment
 *
 * */
class AlignerLocal: public Aligner {

  public:
    AlignerLocal(int ms,int ma,int mi,int go,int ge);

  protected:
    void init(int str1len,int str2len);
    int stopTB(int i,int j);
    int findEnd();
    void fill();
};

}

#endif
