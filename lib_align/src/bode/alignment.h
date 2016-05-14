#ifndef _ALIGNMENT_H
#define _ALIGNMENT_H

#include <ostream>

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Stores an alignment, calculates some numbers on it.
 * */
class Alignment {

  public:
    Alignment(int cnt,int ml);
    ~Alignment();
    void clear();
    void addSeq(char *seq,int start,int end);
    int score();
    int identity();
    int alignlen();
    void setScore(int scr);
    void dump(std::ostream *f,int indent=2,char *label=(char *)"");

  protected:
    unsigned maxlen;
    unsigned slots;
    unsigned used;
    char **seqs;
    int *starts;
    int *ends;
    unsigned alen;
    int sc;
    static const int NEGINF = -1000000;
};

}

#endif
