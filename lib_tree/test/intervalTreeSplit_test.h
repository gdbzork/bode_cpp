#include <cxxtest/TestSuite.h>
#include <time.h>
#include <cstdlib>
#include "bode/intervalTree.h"
#include "bode/intervalTreeSplit.h"

class IntervalTreeSplitTest: public CxxTest::TestSuite {
  private:
    bode::IntervalTreeSplit *x;

  public:
    void setUp(void) {
      x = new bode::IntervalTreeSplit();
   
      TS_ASSERT_EQUALS(x->getCount(),0);
      x->insert(10,20);
      x->insert(40,50);
      x->insert(15,25);
      x->insert(16,24);
    }
    void tearDown(void) {
      delete x;
    }

    void testNodeCount(void) {
      TS_ASSERT_EQUALS(x->getCount(),4);
    }
    void testCoverage(void) {
      TS_ASSERT_EQUALS(x->coverage(5),0);
      TS_ASSERT_EQUALS(x->coverage(9),0);
      TS_ASSERT_EQUALS(x->coverage(10),1);
      TS_ASSERT_EQUALS(x->coverage(14),1);
      TS_ASSERT_EQUALS(x->coverage(15),2);
      TS_ASSERT_EQUALS(x->coverage(16),3);
      TS_ASSERT_EQUALS(x->coverage(19),3);
      TS_ASSERT_EQUALS(x->coverage(20),2);
      TS_ASSERT_EQUALS(x->coverage(23),2);
      TS_ASSERT_EQUALS(x->coverage(24),1);
      TS_ASSERT_EQUALS(x->coverage(25),0);
      TS_ASSERT_EQUALS(x->coverage(26),0);
      TS_ASSERT_EQUALS(x->coverage(39),0);
      TS_ASSERT_EQUALS(x->coverage(40),1);
      TS_ASSERT_EQUALS(x->coverage(49),1);
      TS_ASSERT_EQUALS(x->coverage(50),0);
    }
    void testReadCount(void) {
      TS_ASSERT_EQUALS(x->reads(2,10),0);
      TS_ASSERT_EQUALS(x->reads(2,11),0);
      TS_ASSERT_EQUALS(x->reads(11,11),1);
      TS_ASSERT_EQUALS(x->reads(5,60),4);
      TS_ASSERT_EQUALS(x->reads(30,35),0);
      TS_ASSERT_EQUALS(x->reads(10,17),1);
      TS_ASSERT_EQUALS(x->reads(2,17),1);
      TS_ASSERT_EQUALS(x->reads(24,41),0);
      TS_ASSERT_EQUALS(x->reads(23,41),0);
      TS_ASSERT_EQUALS(x->reads(25,40),0);
      TS_ASSERT_EQUALS(x->reads(25,41),0);
      TS_ASSERT_EQUALS(x->reads(15,20),3);
      TS_ASSERT_EQUALS(x->reads(15,21),3);
      TS_ASSERT_EQUALS(x->reads(12,19),2);
      TS_ASSERT_EQUALS(x->reads(12,18),2);
      TS_ASSERT_EQUALS(x->reads(12,17),1);
      TS_ASSERT_EQUALS(x->reads(21,40),0);
      TS_ASSERT_EQUALS(x->reads(16,40),2);
    }
};
