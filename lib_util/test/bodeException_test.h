#include <cxxtest/TestSuite.h>
#include <vector>
#include <string>
#include <sstream>
#include "bode/bodeException.h"

class BodeExceptionTest: public CxxTest::TestSuite {
  public:

    void testMakeError0(void) {
      bode::BodeError b;
      TS_ASSERT_EQUALS(b.msg(),"Generic Message");
      TS_ASSERT_EQUALS(b.lbl(),"BodeError");
    }

    void testMakeError1(void) {
      bode::BodeError b("This is a bode error");
      TS_ASSERT_EQUALS(b.msg(),"This is a bode error");
      TS_ASSERT_EQUALS(b.lbl(),"BodeError");
    }

    void testMakeError2(void) {
      bode::BodeError b("ErLabel","This is a bode error");
      TS_ASSERT_EQUALS(b.msg(),"This is a bode error");
      TS_ASSERT_EQUALS(b.lbl(),"ErLabel");
    }

    void testMakeException0(void) {
      bode::BodeException b;
      TS_ASSERT_EQUALS(b.msg(),"Generic Message");
      TS_ASSERT_EQUALS(b.lbl(),"BodeException");
    }

    void testMakeException1(void) {
      bode::BodeException b("This is a bode exception");
      TS_ASSERT_EQUALS(b.msg(),"This is a bode exception");
      TS_ASSERT_EQUALS(b.lbl(),"BodeException");
    }

    void testMakeException2(void) {
      bode::BodeException b("ExLabel","This is a bode exception");
      TS_ASSERT_EQUALS(b.msg(),"This is a bode exception");
      TS_ASSERT_EQUALS(b.lbl(),"ExLabel");
    }

    void testMakeFlagError0(void) {
      bode::FlagError b;
      TS_ASSERT_EQUALS(b.msg(),"Generic Message");
      TS_ASSERT_EQUALS(b.lbl(),"FlagError");
    }

    void testMakeFlagError1(void) {
      bode::FlagError b("This is a flag error");
      TS_ASSERT_EQUALS(b.msg(),"This is a flag error");
      TS_ASSERT_EQUALS(b.lbl(),"FlagError");
    }

    void testMakeFlagError2(void) {
      bode::FlagError b("FlLabel","This is a flag error");
      TS_ASSERT_EQUALS(b.msg(),"This is a flag error");
      TS_ASSERT_EQUALS(b.lbl(),"FlLabel");
    }

    void testMakeFlagException0(void) {
      bode::FlagException b;
      TS_ASSERT_EQUALS(b.msg(),"Generic Message");
      TS_ASSERT_EQUALS(b.lbl(),"FlagException");
    }

    void testMakeFlagException1(void) {
      bode::FlagException b("This is a Flag exception");
      TS_ASSERT_EQUALS(b.msg(),"This is a Flag exception");
      TS_ASSERT_EQUALS(b.lbl(),"FlagException");
    }

    void testMakeFlagException2(void) {
      bode::FlagException b("FlLabel","This is a Flag exception");
      TS_ASSERT_EQUALS(b.msg(),"This is a Flag exception");
      TS_ASSERT_EQUALS(b.lbl(),"FlLabel");
    }

    void testWriteError(void) {
      bode::BodeError b("WTest","Alpha Bravo Charlie");
      std::ostringstream out;
      out << b;
      TS_ASSERT_EQUALS(out.str(),"WTest: Alpha Bravo Charlie");
    }

    void testThrowing(void) {
      try {
        throw bode::BodeException("Zork");
      } catch (bode::BodeException b) {
        TS_ASSERT_EQUALS(b.msg(),"Zork");
        return;
      } catch (...) {
        TS_FAIL("Why didn't we catch that??");
        return;
      }
      TS_FAIL("Throwing the exception seems not to have worked.");
    }

    void testSubclassDupes(void) {
      try {
        throw bode::FlagException();
      } catch (bode::BodeException be) {
        TS_ASSERT_EQUALS(be.msg(),"Generic Message");
        return;
      } catch (...) {
        TS_FAIL("Why didn't we catch that??");
        return;
      }
      TS_FAIL("Failed to catch exception we were intended to catch.");
    }

    void testSubclassError(void) {
      try {
        throw bode::FlagError();
      } catch (bode::BodeException be) {
        TS_FAIL("caught error we should have missed");
        return;
      } catch (bode::BodeError be) {
        TS_ASSERT_EQUALS(be.msg(),"Generic Message");
        return;
      }
      TS_FAIL("Failed to catch error we were intended to catch.");
    }

};
