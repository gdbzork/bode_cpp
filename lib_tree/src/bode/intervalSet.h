#ifndef __INTERVALSET_H
#define __INTERVALSET_H

#include <map>
#include <string>
#include <cstring>
#include "bode/interval.h"
#include "bode/intervalTree.h"

namespace bode {

/*
struct chromComp {
  bool operator()(const std::string &lhs,const std::string &rhs) const {
    int an = 0,bn = 0;
    const char *lc,*rc;
    lc = lhs.c_str();
    rc = rhs.c_str();
    an = atoi(lc+3);
    bn = atoi(rc+3);
    if (an > 0 && bn > 0) {
      return an < bn;
    } else {
      return lhs < rhs;
    }
  }
};
*/

struct chromComp {
  bool operator()(const std::string &lhs,const std::string &rhs) const {
    int ln = 0,rn = 0;
    char lc[128],rc[128];
    char *ltrailing,*rtrailing;
    strncpy(lc,lhs.c_str(),128);
    strncpy(rc,rhs.c_str(),128);

    if (strncmp(lc,"chr",3) == 0) {
      ln = strtol(lc+3,&ltrailing,10);
    } else {
      ln = strtol(lc,&ltrailing,10);
    }
    if (strncmp(rc,"chr",3) == 0) {
      rn = strtol(rc+3,&rtrailing,10);
    } else {
      rn = strtol(rc,&rtrailing,10);
    }

    if (ln > 0 && rn > 0) {
      if (ln == rn) {
        return strcmp(ltrailing,rtrailing) < 0;
      } else {
        return ln < rn;
      }
    } else {
      return lhs < rhs;
    }
  }
};

/**
 * \brief <span class=stat_bad>red</span> Stores a set of genomic intervals in a set of red/black trees (one per chromosome).
 */
class IntervalSet {
  public:
    IntervalSet();
    virtual ~IntervalSet();
    virtual void insert(Interval const *inter);
    int coverage(std::string const &chrom,int point) const;
    void density(std::string const &chrom,int left,int right,int *dest) const;
    int overlapping(Interval const *inter) const;
    int count() const;
    int realCount() const;
    int chromCount() const;
    std::map<std::string,IntervalTree *,chromComp>::iterator chromIter();

  protected:
    std::map<std::string,IntervalTree *,chromComp> *chroms;
};

}

#endif
