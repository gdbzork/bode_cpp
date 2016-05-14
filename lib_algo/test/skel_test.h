#include <cxxtest/TestSuite.h>

class SkelTest: public CxxTest::TestSuite {
  public:
    void testSanity(void) {
      TS_ASSERT(1);
    }
};
