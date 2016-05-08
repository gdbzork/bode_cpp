#include <cxxtest/TestSuite.h>
#include "bode/histogram.h"

class HistogramTest: public CxxTest::TestSuite {
  public:

    void testIncrement(void) {
      bode::Histogram *x;
      x = new bode::Histogram();
      int n;
      x->increment(5);
      x->increment(5);
      x->increment(5);
      x->increment(5);
      n = x->get(5);
      TS_ASSERT_EQUALS(n,4);
      delete x;
    }

    void testZeros(void) {
      bode::Histogram *x;
      x = new bode::Histogram();
      int n;
      x->increment(5);
      x->increment(5);
      x->increment(5);
      x->increment(5);
      n = x->get(6);
      TS_ASSERT_EQUALS(n,0);
      delete x;
    }

    void testOffset(void) {
      int i,j;
      bode::Histogram *y;
      y = new bode::Histogram(5,10);
      y->increment(5);
      y->increment(10);
      i = y->get(5);
      j = y->get(10);
      TS_ASSERT_EQUALS(i,1);
      TS_ASSERT_EQUALS(j,1);
      delete y;
    }

};
