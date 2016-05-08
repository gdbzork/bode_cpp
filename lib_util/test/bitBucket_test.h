#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include "bode/bodeException.h"
#include "bode/bitBucket.h"

class BitBucketTest: public CxxTest::TestSuite {
  public:

    void testSanity(void) {
      bode::BitBucket bb(10);
      TS_ASSERT_EQUALS(bb[2],false);
      bb.set(4);
      TS_ASSERT_EQUALS(bb[4],true);
    }

    void testRange(void) {
      bode::BitBucket bb(10);
      TS_ASSERT_THROWS(bb.set(99),bode::BodeError);
    }

};
