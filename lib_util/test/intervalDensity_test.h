#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include <stdexcept>

#include "bode/intervalDensity.h"

class IntervalDensityTest: public CxxTest::TestSuite {
  public:

    void testSanity(void) {
      bode::IntervalDensity id(512);
      TS_ASSERT_EQUALS(id.depth(256),0);
      id.set(128,300);
      TS_ASSERT_EQUALS(id.depth(256),1);
    }

    void testRange(void) {
      bode::IntervalDensity id(10);
      TS_ASSERT_THROWS(id.set(11,15),std::out_of_range);
    }

    void testDepth(void) {
      bode::IntervalDensity id(512);
      id.set(128,300);
      id.set(100,150);
      id.set(135,200);
      id.set(140,350);
      TS_ASSERT_EQUALS(id.depth(50),0);
      TS_ASSERT_EQUALS(id.depth(110),1);
      TS_ASSERT_EQUALS(id.depth(130),2);
      TS_ASSERT_EQUALS(id.depth(137),3);
      TS_ASSERT_EQUALS(id.depth(145),4);
      TS_ASSERT_EQUALS(id.depth(175),3);
      TS_ASSERT_EQUALS(id.depth(250),2);
      TS_ASSERT_EQUALS(id.depth(325),1);
      TS_ASSERT_EQUALS(id.depth(400),0);
    }

    void testSummit(void) {
      int pos;
      unsigned int hgt;
      bode::IntervalDensity id(512);
      id.set(128,300);
      id.set(100,150);
      id.set(135,200);
      id.set(140,350);
      id.summit(pos,hgt);
      TS_ASSERT_EQUALS(pos,144);
      TS_ASSERT_EQUALS(hgt,4);
    }

    void testEdges(void) {
      bode::IntervalDensity id(512);
      id.set(128,300);
      id.set(100,150);
      id.set(135,200);
      id.set(140,350);
      TS_ASSERT_EQUALS(id.depth(0),0);
      TS_ASSERT_EQUALS(id.depth(99),0);
      TS_ASSERT_EQUALS(id.depth(100),1);
      TS_ASSERT_EQUALS(id.depth(127),1);
      TS_ASSERT_EQUALS(id.depth(128),2);
      TS_ASSERT_EQUALS(id.depth(134),2);
      TS_ASSERT_EQUALS(id.depth(135),3);
      TS_ASSERT_EQUALS(id.depth(139),3);
      TS_ASSERT_EQUALS(id.depth(140),4);
      TS_ASSERT_EQUALS(id.depth(149),4);
      TS_ASSERT_EQUALS(id.depth(150),3);
      TS_ASSERT_EQUALS(id.depth(199),3);
      TS_ASSERT_EQUALS(id.depth(200),2);
      TS_ASSERT_EQUALS(id.depth(299),2);
      TS_ASSERT_EQUALS(id.depth(300),1);
      TS_ASSERT_EQUALS(id.depth(349),1);
      TS_ASSERT_EQUALS(id.depth(350),0);
      TS_ASSERT_EQUALS(id.depth(511),0);
    }

   void testOverflow(void) {
      bode::IntervalDensity id(512);
      id.set(-10,10);
      id.set(500,550);
      TS_ASSERT_EQUALS(id.depth(0),1);
      TS_ASSERT_EQUALS(id.depth(9),1);
      TS_ASSERT_EQUALS(id.depth(10),0);
      TS_ASSERT_EQUALS(id.depth(499),0);
      TS_ASSERT_EQUALS(id.depth(500),1);
      TS_ASSERT_EQUALS(id.depth(511),1);
      TS_ASSERT_THROWS(id.depth(-1),std::out_of_range);
      TS_ASSERT_THROWS(id.depth(512),std::out_of_range);
  }
};
