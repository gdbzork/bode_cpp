#include <cxxtest/TestSuite.h>
#include "bode/bed.h"

class UtilTest: public CxxTest::TestSuite {
  public:
    void testSanity(void) {
      bode::Bed *x;
      x = new bode::Bed();
      delete x;
    }
};
