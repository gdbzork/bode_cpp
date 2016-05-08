#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include <string>
#include <sam/bam.h>
#include "bode/cigar.h"

class CigarTest: public CxxTest::TestSuite {
  public:

  void testSanity(void) {
    uint32_t sample[2];
    sample[0] = 5 << BAM_CIGAR_SHIFT | BAM_CSOFT_CLIP;
    sample[1] = 20 << BAM_CIGAR_SHIFT | BAM_CMATCH;
    bode::Cigar x(2,sample);

    TS_ASSERT_EQUALS(x.repr(),"5S20M");
    TS_ASSERT_EQUALS(x.refLength(),20);
  }

  void testUnknownChar(void) {
    TS_ASSERT_THROWS(bode::Cigar("5S20Q"),bode::illegalCigarEx);
  }

  void testDoubleChar(void) {
    TS_ASSERT_THROWS(bode::Cigar("5S20MM"),bode::illegalCigarEx);
  }

  void testMissingChar(void) {
    TS_ASSERT_THROWS(bode::Cigar("5S20"),bode::illegalCigarEx);
  }

  void testTooManyElements(void) {
    std::string s;
    bode::Cigar cig;
    s.reserve(2050);
    for (int i=0;i<1025;i++) {
      s.append("1M");
    }
    TS_ASSERT_THROWS(cig.assign(s),bode::cigarTooLongEx);
  }

  void testVoid(void) {
    bode::Cigar cig;
    uint32_t target[1];
    TS_ASSERT_EQUALS(cig.refLength(),0);
    TS_ASSERT_EQUALS(cig.asBam(target),0);
    TS_ASSERT_EQUALS(cig.repr(),"");
  }

  void testConversion(void) {
    bode::Cigar cig("1M2I3D4N5S6H7P8=9X10B");
    uint32_t target[10];
    uint32_t n_elem;
    n_elem = cig.asBam(target);
    TS_ASSERT_EQUALS(n_elem,10);
    TS_ASSERT_EQUALS(target[0]&BAM_CIGAR_MASK,BAM_CMATCH);
    TS_ASSERT_EQUALS(target[1]&BAM_CIGAR_MASK,BAM_CINS);
    TS_ASSERT_EQUALS(target[2]&BAM_CIGAR_MASK,BAM_CDEL);
    TS_ASSERT_EQUALS(target[3]&BAM_CIGAR_MASK,BAM_CREF_SKIP);
    TS_ASSERT_EQUALS(target[4]&BAM_CIGAR_MASK,BAM_CSOFT_CLIP);
    TS_ASSERT_EQUALS(target[5]&BAM_CIGAR_MASK,BAM_CHARD_CLIP);
    TS_ASSERT_EQUALS(target[6]&BAM_CIGAR_MASK,BAM_CPAD);
    TS_ASSERT_EQUALS(target[7]&BAM_CIGAR_MASK,BAM_CEQUAL);
    TS_ASSERT_EQUALS(target[8]&BAM_CIGAR_MASK,BAM_CDIFF);
    TS_ASSERT_EQUALS(target[9]&BAM_CIGAR_MASK,BAM_CBACK);
    TS_ASSERT_EQUALS(target[0]>>BAM_CIGAR_SHIFT,1);
    TS_ASSERT_EQUALS(target[1]>>BAM_CIGAR_SHIFT,2);
    TS_ASSERT_EQUALS(target[2]>>BAM_CIGAR_SHIFT,3);
    TS_ASSERT_EQUALS(target[3]>>BAM_CIGAR_SHIFT,4);
    TS_ASSERT_EQUALS(target[4]>>BAM_CIGAR_SHIFT,5);
    TS_ASSERT_EQUALS(target[5]>>BAM_CIGAR_SHIFT,6);
    TS_ASSERT_EQUALS(target[6]>>BAM_CIGAR_SHIFT,7);
    TS_ASSERT_EQUALS(target[7]>>BAM_CIGAR_SHIFT,8);
    TS_ASSERT_EQUALS(target[8]>>BAM_CIGAR_SHIFT,9);
    TS_ASSERT_EQUALS(target[9]>>BAM_CIGAR_SHIFT,10);
  }

  void testRepr(void) {
    uint32_t src[10];
    src[0] = 1<<BAM_CIGAR_SHIFT | BAM_CMATCH;
    src[1] = 2<<BAM_CIGAR_SHIFT | BAM_CINS;
    src[2] = 3<<BAM_CIGAR_SHIFT | BAM_CDEL;
    src[3] = 4<<BAM_CIGAR_SHIFT | BAM_CREF_SKIP;
    src[4] = 5<<BAM_CIGAR_SHIFT | BAM_CSOFT_CLIP;
    src[5] = 6<<BAM_CIGAR_SHIFT | BAM_CHARD_CLIP;
    src[6] = 7<<BAM_CIGAR_SHIFT | BAM_CPAD;
    src[7] = 8<<BAM_CIGAR_SHIFT | BAM_CEQUAL;
    src[8] = 9<<BAM_CIGAR_SHIFT | BAM_CDIFF;
    src[9] = 10<<BAM_CIGAR_SHIFT | BAM_CBACK;
    bode::Cigar cig(10,src);
    TS_ASSERT_EQUALS(cig.repr(),"1M2I3D4N5S6H7P8=9X10B");
  }

  void testLength(void) {
    bode::Cigar cig("1M2I3D4N5S6H7P8=9X10B");
    TS_ASSERT_EQUALS(cig.refLength(),32);
  }
 
  void testAssignBam(void) {
    uint32_t src[10];
    bode::Cigar cig;
    src[0] = 1<<BAM_CIGAR_SHIFT | BAM_CMATCH;
    src[1] = 2<<BAM_CIGAR_SHIFT | BAM_CINS;
    src[2] = 3<<BAM_CIGAR_SHIFT | BAM_CDEL;
    src[3] = 4<<BAM_CIGAR_SHIFT | BAM_CREF_SKIP;
    src[4] = 5<<BAM_CIGAR_SHIFT | BAM_CSOFT_CLIP;
    src[5] = 6<<BAM_CIGAR_SHIFT | BAM_CHARD_CLIP;
    src[6] = 7<<BAM_CIGAR_SHIFT | BAM_CPAD;
    src[7] = 8<<BAM_CIGAR_SHIFT | BAM_CEQUAL;
    src[8] = 9<<BAM_CIGAR_SHIFT | BAM_CDIFF;
    src[9] = 10<<BAM_CIGAR_SHIFT | BAM_CBACK;
    TS_ASSERT_THROWS_NOTHING(cig.assign(10,src));
    TS_ASSERT_EQUALS(cig.refLength(),32);
    TS_ASSERT_EQUALS(cig.repr(),"1M2I3D4N5S6H7P8=9X10B");
  }

};
