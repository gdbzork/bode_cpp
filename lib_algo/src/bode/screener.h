#ifndef _SCREENER_H
#define _SCREENER_H

#include "bode/alignerPartialFree.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Looks for linker sequence in reads.
 */
class Screener {

  public:
    Screener(char *fp,char *tp,int ms,float id,int ml,int tl);
    ~Screener();
    int isLinker(char *str,int &start, int &length);
    void stats(int &tot,int &clean,int &fp,int &tp,int &endm);

  protected:
    char *fiveplinker;
    char *threeplinker;
    int maxstr;    /* longest allowable string */
    float identity;  /* percent identity required to be linker */
    int matchlen;  /* length of match required to be linker */
    int taillen;   /* length of match required to be tail linker */
    static const int match = 2;      /* match score */
    static const int mismatch = -2;  /* mismatch score */
    static const int gapopen = -6;   /* gap-open score */
    static const int gapextend = -1; /* gap-extend score */
    char *tb1;
    char *tb2;

    Aligner *aligner;

    int readCount;
    int cleanCount;
    int fivepCount;
    int threepCount;
    int endMatchCount;
    int wholeMatchCount;

    int alignLinker();
    int foundLinker();
};

}

#endif
