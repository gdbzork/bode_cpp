#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include "bode/chromosome.h"

class ChromosomeTest: public CxxTest::TestSuite {
  public:

    void testSanity(void) {
      bode::Chromosome a((char *)"chr1");
      bode::Chromosome b((char *)"chr2");
      TS_ASSERT(a<b);
    }

    void testTrailing(void) {
      bode::Chromosome a((char *)"chr1");
      bode::Chromosome b((char *)"chr1_stuff");
      TS_ASSERT(a<b);
      TS_ASSERT(!(a==b));
      TS_ASSERT(!(b<a));
    }

    void testX(void) {
      bode::Chromosome a((char *)"chr9");
      bode::Chromosome b((char *)"chrX");
      TS_ASSERT(a<b);
      TS_ASSERT(!(a==b));
      TS_ASSERT(!(b<a));
    }

    void testNumeric(void) {
      bode::Chromosome a((char *)"chr9");
      bode::Chromosome b((char *)"chr21");
      TS_ASSERT(a<b);
      TS_ASSERT(!(a==b));
      TS_ASSERT(!(b<a));
    }

    void testNumericEqual(void) {
      bode::Chromosome a((char *)"chr9");
      bode::Chromosome b((char *)"chr9");
      TS_ASSERT(a==b);
      TS_ASSERT(!(a<b));
      TS_ASSERT(!(b<a));
    }

    void testNumericEqualTrailing(void) {
      bode::Chromosome a((char *)"chr9_trailing");
      bode::Chromosome b((char *)"chr9_trailing");
      TS_ASSERT(a==b);
      TS_ASSERT(!(a<b));
      TS_ASSERT(!(b<a));
    }

    void testNumericStraight(void) {
      bode::Chromosome a((char *)"chr9");
      bode::Chromosome b((char *)"chr21");
      TS_ASSERT(a<b);
      TS_ASSERT(!(a==b));
      TS_ASSERT(!(b<a));
    }

    void testNumericUnequalTrailing(void) {
      bode::Chromosome a((char *)"chr9_Random23");
      bode::Chromosome b((char *)"chr9_Random99");
      TS_ASSERT(a<b);
      TS_ASSERT(!(a==b));
      TS_ASSERT(!(b<a));
    }

};
