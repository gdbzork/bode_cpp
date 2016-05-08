#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include <string>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>

#include "bode/configuration.h"

class ConfigurationTest: public CxxTest::TestSuite {
  public:

    void setUp(void) {
      strcpy(testfile,"/tmp/borkborkXXXXXX");
      int fd = mkstemp(testfile);
      FILE *fstream = fdopen(fd,"w");
      fprintf(fstream,"alpha: bravo\ncharlie : delta : echo\nfoxtrot:golf\n");
      fprintf(fstream,"    hotel    :     india    \n");
      fclose(fstream);
    }

    void tearDown(void) {
      remove(testfile);
    }

    void testSanity(void) {
      bode::Configuration c;
      c.load(std::string(testfile));
      TS_ASSERT_EQUALS(c.get("alpha"),"bravo");
    }

    void testInternalColon(void) {
      bode::Configuration c;
      c.load(std::string(testfile));
      TS_ASSERT_EQUALS(c.get("charlie"),"delta : echo");
    }

    void testStrippingWhite(void) {
      bode::Configuration c;
      c.load(std::string(testfile));
      TS_ASSERT_EQUALS(c.get("hotel"),"india");
    }

    void testMultipleFiles(void) {
      char testfile2[1024];
      char testfile3[1024];
      strcpy(testfile2,"/tmp/borkborkXXXXXX");
      int fd = mkstemp(testfile2);
      FILE *fstream = fdopen(fd,"w");
      fprintf(fstream,"alpha: charlie\ncharlie : echo : echo\n");
      fclose(fstream);

      strcpy(testfile3,"/tmp/borkborkXXXXXX");
      fd = mkstemp(testfile3);
      fstream = fdopen(fd,"w");
      fprintf(fstream,"fred : wilma\n");
      fclose(fstream);

      std::string target = std::string(testfile);
      target.append(":");
      target.append(testfile2);
      target.append(":");
      target.append(testfile3);
      bode::Configuration c;
      c.load(target);
      TS_ASSERT_EQUALS(c.get("alpha"),"charlie");
      TS_ASSERT_EQUALS(c.get("foxtrot"),"golf");
      TS_ASSERT_EQUALS(c.get("fred"),"wilma");

      remove(testfile2);
      remove(testfile3);
    }

    void testMissingFiles(void) {
      std::string target = std::string(testfile);
      target.append(":/zorkzork");
      bode::Configuration c;
      c.load(target);
      TS_ASSERT_EQUALS(c.get("alpha"),"bravo");
    }

    void testInitializationConstants(void) {
      std::string sample[][2] = { {"lima","mike"},
                                  {"november","oscar"},
                                  {"",""} };
      bode::Configuration c(sample);
      TS_ASSERT_EQUALS(c.get("lima"),"mike");
      TS_ASSERT_EQUALS(c.get("november"),"oscar");
    }

    void testInitializationStruct(void) {
      struct bode::param p[] = { {"lima","mike",bode::STRING},
                                 {"november","oscar",bode::STRING},
                                 {"","",bode::STRING} };
      bode::Configuration c(p);
      TS_ASSERT_EQUALS(c.get("lima"),"mike");
      TS_ASSERT_EQUALS(c.get("november"),"oscar");
    }

    void testIsIntA(void) {
      TS_ASSERT_EQUALS(bode::Configuration::isInt("10"),true);
    }

    void testIsIntB(void) {
      TS_ASSERT_EQUALS(bode::Configuration::isInt("-20"),true);
    }

    void testIsntInt(void) {
      TS_ASSERT_EQUALS(bode::Configuration::isInt("-20x"),false);
    }

    void testIsDoubleA(void) {
      TS_ASSERT_EQUALS(bode::Configuration::isDouble("10.0"),true);
    }

    void testIsDoubletB(void) {
      TS_ASSERT_EQUALS(bode::Configuration::isDouble("-20.935e5"),true);
    }

    void testIsntDouble(void) {
      TS_ASSERT_EQUALS(bode::Configuration::isDouble("-20.935q5"),false);
    }

  private:
    char testfile[1024];

};
