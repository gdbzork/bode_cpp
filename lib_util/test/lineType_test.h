#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include "bode/lineType.h"

class LineTypeTest: public CxxTest::TestSuite {
  public:

    void testUnix(void) {
      std::string fn = "testdata/sheet_unix.txt";
      bode::TextType lt = bode::TextTypeChecker::sget(fn);
      TS_ASSERT_EQUALS(bode::UNIX,lt);
    }

    void testMac(void) {
      std::string fn = "testdata/sheet_mac.txt";
      bode::TextType lt = bode::TextTypeChecker::sget(fn);
      TS_ASSERT_EQUALS(bode::MAC,lt);
    }

    void testDos(void) {
      std::string fn = "testdata/sheet_dos.txt";
      bode::TextType lt = bode::TextTypeChecker::sget(fn);
      TS_ASSERT_EQUALS(bode::DOS,lt);
    }

    void testMixed(void) {
      std::string fn = "testdata/sheet_mixed.txt";
      bode::TextType lt = bode::TextTypeChecker::sget(fn);
      TS_ASSERT_EQUALS(bode::MIXED,lt);
    }

};

