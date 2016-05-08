#ifndef __HISTOGRAM_H
#define __HISTOGRAM_H

#include <ostream>

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> A very simple histogram class.
 */
class Histogram {
  public:
    Histogram(void);
    Histogram(int low,int high);
    ~Histogram(void);
    void increment(int bin);
    void add(int bin,int count);
    int get(int bin);

  private:
    int *bins;
    int l;
    int h;

};

}

#endif
