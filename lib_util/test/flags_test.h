#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include <vector>
#include <string>
#include <iostream>
#include "bode/flags.h"
#include "bode/bodeException.h"

class FlagsTest: public CxxTest::TestSuite {
  public:

    void testSanity(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"zork","the zork option!");
      char * const commands[] = {(char *)"executable",
                                 (char *)"--zork",
                                 (char *)"alpha",
                                 (char *)"bravo",
                                 (char *)"charlie"};
      flagset.parse(5,commands);
      std::vector<std::string> x = flagset.positionalArgs();
      TS_ASSERT_EQUALS(x.size(),3);
      TS_ASSERT_EQUALS(x[0],"executable");
      TS_ASSERT_EQUALS(x[1],"bravo");
      TS_ASSERT_EQUALS(x[2],"charlie");
      TS_ASSERT_EQUALS(flagset.isSet("zork"),true);
      TS_ASSERT_EQUALS(flagset.getStr("zork"),"alpha");
    }

    void testDuplicateFlags4(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"zork","the zork option!");
      TS_ASSERT_THROWS(flagset.add("zork",bode::STRING,"zork","the zork option!"),bode::FlagError);
    }

    void testDuplicateFlagsText4(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"zork","the zork option!");
      try {
        flagset.add("zork",bode::STRING,"zork","the zork option!");
      } catch (bode::FlagError fe) {
        TS_ASSERT_EQUALS(fe.msg(),"duplicate flag: 'zork'");
      } catch (bode::BodeError be) {
        TS_FAIL("caught BodeError we shouldn't have");
      }
    }

    void testDuplicateFlags3(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"the zork option!");
      TS_ASSERT_THROWS(flagset.add("zork",bode::STRING,"the zork option!"),bode::FlagError);
    }

    void testDuplicateFlagsText3(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"the zork option!");
      try {
        flagset.add("zork",bode::STRING,"the zork option!");
      } catch (bode::FlagError fe) {
        TS_ASSERT_EQUALS(fe.msg(),"duplicate flag: 'zork'");
      } catch (bode::BodeError be) {
        TS_FAIL("caught BodeError we shouldn't have");
      }
    }

    void testIllegalFlag(void) {
      bode::Flags flagset;
      TS_ASSERT_THROWS(flagset.add("Zork",bode::STRING,"zork","the zork option!"),bode::FlagError);
    }

    void testIllegalFlagText(void) {
      bode::Flags flagset;
      bool passed = false;
      try {
        flagset.add("Zork",bode::STRING,"zork","the zork option!");
      } catch (bode::FlagError fe) {
        TS_ASSERT_EQUALS(fe.msg(),"illegal characters in 'Zork'");
        passed = true;
      } catch (bode::BodeError be) {
        TS_FAIL("Unexpected BodeError: "+be.msg());
      }
      if (!passed) {
        TS_FAIL("Missed expected FlagException.");
      }
    }

    void testEndOfOpts(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"zork","the zork option!");
      char * const commands[] = {(char *)"executable",
                                 (char *)"--",
                                 (char *)"--zork",
                                 (char *)"alpha",
                                 (char *)"bravo",
                                 (char *)"charlie"};
      flagset.parse(6,commands);
      std::vector<std::string> x = flagset.positionalArgs();
      TS_ASSERT_EQUALS(x.size(),5);
      TS_ASSERT_EQUALS(x[0],"executable");
      TS_ASSERT_EQUALS(x[1],"--zork");
      TS_ASSERT_EQUALS(x[2],"alpha");
      TS_ASSERT_EQUALS(x[3],"bravo");
      TS_ASSERT_EQUALS(x[4],"charlie");
      TS_ASSERT_EQUALS(flagset.isSet("zork"),false);
    }

    void testUnknownFlag(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::STRING,"zork","the zork option!");
      char * const commands[] = {(char *)"executable",
                                 (char *)"--zark",
                                 (char *)"zarkopt"};
      TS_ASSERT_THROWS(flagset.parse(3,commands),bode::FlagException);
    }

    void testUnknownFlagText(void) {
      bode::Flags flagset;
      bool passed = false;
      flagset.add("zork",bode::STRING,"zork","the zork option!");
      try {
        char * const commands[] = {(char *)"executable",
                                   (char *)"--zark",
                                   (char *)"zarkopt"};
        flagset.parse(3,commands);
      } catch (bode::FlagException fe) {
        TS_ASSERT_EQUALS(fe.msg(),"unknown flag: 'zark'");
        passed = true;
      } catch (bode::BodeError be) {
        TS_FAIL("Unexpected BodeError: "+be.msg());
      }
      if (!passed) { // didn't execute catch block
        TS_FAIL("Missed expected FlagException");
      }
    }

    void testValidBool(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::BOOL,"false","the zork option!");
      char * const commands[] = {(char *)"executable",
                                 (char *)"--zork",
                                 (char *)"true"};
      TS_ASSERT_THROWS_NOTHING(flagset.parse(3,commands));
      TS_ASSERT_EQUALS(flagset.getBool("zork"),true);
    }

    void testValidBoolFalse(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::BOOL,"false","the zork option!");
      char * const commands[] = {(char *)"executable",
                                 (char *)"--nozork",
                                 (char *)"Bork"};
      TS_ASSERT_THROWS_NOTHING(flagset.parse(3,commands));
      TS_ASSERT_EQUALS(flagset.getBool("zork"),false);
    }

    void testBoolNoArgument(void) {
      bode::Flags flagset;
      flagset.add("zork",bode::BOOL,"false","the zork option!");
      char * const commands[] = {(char *)"executable",
                                 (char *)"--nozork",
                                 (char *)"Bork"};
      TS_ASSERT_THROWS_NOTHING(flagset.parse(3,commands));
      TS_ASSERT_EQUALS(flagset.getBool("zork"),false);
      std::vector<std::string> x = flagset.positionalArgs();
      TS_ASSERT_EQUALS(x.size(),2);
      TS_ASSERT_EQUALS(x[0],"executable");
      TS_ASSERT_EQUALS(x[1],"Bork");
    }

};
