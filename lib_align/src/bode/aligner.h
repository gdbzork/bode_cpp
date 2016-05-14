#ifndef _ALIGNER_H
#define _ALIGNER_H

#include <ostream>
#include "bode/alignment.h"

namespace bode {

/** Aligns short strings, relatively quickly, in memory.
 *
 * simple, fast alignment routines for short-ish strings (uses O(nm) space)
 *
 */

/**
 * \brief <span class=stat_bad>red</span> Base class of alignment classes.
 *
 *  */
class Aligner {

  public:
    Aligner(int maxstr,int match,int mismatch,int gapopen,int gapextend);
    virtual ~Aligner();
    int align(char *str1,char *str2,Alignment *aln);
    void dump(std::ostream *f,int indent=2,int debug=0,char *label=(char *)"");

  protected:
    int maxstr;    /* longest allowable string */
    int match;     /* match score */
    int mismatch;  /* mismatch score */
    int gapopen;   /* gap-open score */
    int gapextend; /* gap-extend score */

    char *s1;      /* string 1 ("top" string) */
    char *s2;      /* string 2 ("side" string) */
    int end1;
    int end2;
    char *b1;
    char *b2;

    int **matrix;  /* DP matrix */
    int **gap1;    /* gap (side) matrix */
    int **gap2;    /* gap (top) matrix */

    Alignment *aln; /* alignment */

    enum direction { UP, DIAG, LEFT, STOP };
    const static int NEGINF = -1000000;

    virtual void init(int str1len,int str2len) = 0;
    virtual void fill();
    virtual int findEnd() = 0;
    virtual int stopTB(int i,int j) = 0;
    direction getDir(int i,int j,direction prevdir);
    direction initialDir(int i,int j);
    void traceback();

    inline int max3(int a,int b,int c) {
      return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
    }

    inline int max2(int a,int b) {
      return (a > b) ? a : b;
    }

};

}

#endif
