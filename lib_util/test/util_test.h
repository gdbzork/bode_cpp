#include <cxxtest/TestSuite.h>
#include "bode/util.h"

class UtilTest: public CxxTest::TestSuite {
  public:
    void testSanity(void) {
      std::vector<std::string> *x;
      x = bode::splitV("alpha:bravo:charlie",':');
      TS_ASSERT_EQUALS(x->size(),3);
      TS_ASSERT_EQUALS(x->at(0),std::string("alpha"));
      TS_ASSERT_EQUALS(x->at(1),std::string("bravo"));
      TS_ASSERT_EQUALS(x->at(2),std::string("charlie"));
      delete x;
    }

    void testEmptyStrings(void) {
      std::vector<std::string> *x;
      x = bode::splitV("alpha:::charlie",':');
      TS_ASSERT_EQUALS(x->size(),4);
      TS_ASSERT_EQUALS(x->at(0),std::string("alpha"));
      TS_ASSERT_EQUALS(x->at(1),std::string(""));
      TS_ASSERT_EQUALS(x->at(2),std::string(""));
      TS_ASSERT_EQUALS(x->at(3),std::string("charlie"));
      delete x;
    }

    void testNoDelimiters(void) {
      std::vector<std::string> *x;
      x = bode::splitV("zork",':');
      TS_ASSERT_EQUALS(x->size(),1);
      TS_ASSERT_EQUALS(x->at(0),std::string("zork"));
      delete x;
    }

    void testEmptyString(void) {
      std::vector<std::string> *x;
      x = bode::splitV("",':');
      TS_ASSERT_EQUALS(x->size(),1);
      TS_ASSERT_EQUALS(x->at(0),std::string(""));
      delete x;
    }
};
