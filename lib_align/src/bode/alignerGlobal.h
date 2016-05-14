#ifndef _ALIGNERGLOBAL_H
#define _ALIGNERGLOBAL_H

#include "aligner.h"
#include "alignment.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Aligns strings using global alignment.
 *
 * */
class AlignerGlobal: public Aligner {

  public:
    AlignerGlobal(int ms,int ma,int mi,int go,int ge); 

  protected:
    void init(int str1len,int str2len);
    int stopTB(int i,int j);
    int findEnd();
};

}

#endif
