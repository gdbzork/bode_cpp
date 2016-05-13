#ifndef __INTERVALTREE_H
#define __INTERVALTREE_H

#include "bode/intervalNode.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Stores a tree of genomic intervals.
 */
class IntervalTree {

  public:
    IntervalTree(void);
    virtual ~IntervalTree(void);
    void insert(int left,int right);
    int coverage(int point);
    void density(int left,int right,int *dest);
    int reads(int left,int right);
    int summit(int left,int right);
    int getCount(void);
    int realCount(void);
   
  private:
    IntervalNode *root;
    int count;

    IntervalNode *raw_insert(IntervalNode *node);
    void rebalance(IntervalNode *node);
    void leftRotate(IntervalNode *node);
    void rightRotate(IntervalNode *node);
    int i_coverage(IntervalNode *n,int point);
    void i_density(IntervalNode *n,int left,int right,int *dest);
    virtual int i_countIntervals(IntervalNode *n,int left,int right);
    int i_realCount(IntervalNode *n);
};

}

#endif
