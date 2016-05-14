#include <cxxtest/TestSuite.h>
#include "bode/alignment.h"

class AlignmentTest: public CxxTest::TestSuite {
  public:
    void testIdentity(void) {
      bode::Alignment *a;

      a = new bode::Alignment(2,100);
      a->addSeq((char *)"ABCDEFGHIJKLMNOP",5,21);
      a->addSeq((char *)"ABCDEFGZIZKLMNOP",0,16);
      TS_ASSERT(a->identity() == 14);
    }

    void testAlignLen(void) {
      bode::Alignment *a;
      a = new bode::Alignment(2,100);
      a->addSeq((char *)"ABCDEFGHIJKLMNOP",5,21);
      a->addSeq((char *)"ABCDEFGZIZKLMNOP",0,16);
      TS_ASSERT(a->alignlen() == 16);
    }

    void testSeqLength(void) {
      bode::Alignment *a;
      a = new bode::Alignment(1,5);
      TS_ASSERT_THROWS_ANYTHING( a->addSeq((char *) "ABCDEF",0,5));
    }
};
