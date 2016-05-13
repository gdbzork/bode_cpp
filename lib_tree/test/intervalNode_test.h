#include <cxxtest/TestSuite.h>
#include "bode/intervalNode.h"

class IntervalNodeTest: public CxxTest::TestSuite {
  private:
    bode::IntervalNode *x,*y,*z;

  public:
    void setUp(void) {
      x = new bode::IntervalNode(10,20);
      y = new bode::IntervalNode(10,20);
      z = new bode::IntervalNode(10,15);
    }
    void tearDown(void) {
      delete x;
      delete y;
      delete z;
    }

    void testSanity(void) {

      x->incrementCount();
      TS_ASSERT(x->getCount()==2);
    }
    void testOverlap(void) {
      TS_ASSERT(x->overlap(15,25));
    }
    void testEqualsOp(void) {
      TS_ASSERT(*x==*y);
    }
    void testGTOp(void) {
      TS_ASSERT(*x>*z);
    }
    void testLTOp(void) {
      TS_ASSERT(*z<*x);
    }
};
