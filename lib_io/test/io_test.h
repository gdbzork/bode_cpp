#include <cxxtest/TestSuite.h>
#include <unistd.h>
#include <cstdlib>
#include "bode/intervalReader.h"
#include "bode/writer.h"
#include "bode/interval.h"
#include "bode/util.h"

class IOTest: public CxxTest::TestSuite {
  private:
    static const std::string inputBedFN;
    static const std::string inputBamFN;
    static const std::string outputBedFN;
    static const std::string outputBamFN;

  public:
    void testBed(void) {
      bode::IntervalReader *r;
      bode::Writer *w;
      bode::Interval *i;

      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputBedFN;

      r = bode::IntervalReader::open(inputPath);
      w = r->getWriter();
      w->open(outputBedFN,r->header());
      while ((i=r->nextI())) {
        w->write(*i);
      }

      r->close();
      w->close();

      std::string md1 = bode::md5(inputPath);
      std::string md2 = bode::md5(outputBedFN);
      TS_ASSERT_EQUALS(md1,md2);
      if (md1 == md2) {
        unlink(outputBedFN.c_str());
      }
      
      delete r;
      delete w;
    }

    void testBam(void) {
      bode::IntervalReader *r;
      bode::Writer *w;
      bode::Interval *i;

      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputBamFN;

      r = bode::IntervalReader::open(inputPath);
      w = r->getWriter();
      w->open(outputBamFN,r->header());
      while ((i=r->nextI())) {
        w->write(*i);
      }

      r->close();
      w->close();

      std::string md1 = bode::md5(inputPath);
      std::string md2 = bode::md5(outputBamFN);
      TS_ASSERT_EQUALS(md1,md2);
      if (md1 == md2) {
        unlink(outputBamFN.c_str());
      }
      
      delete r;
      delete w;
    }

};

const std::string IOTest::inputBedFN = "/testdata/test_input.bed";
const std::string IOTest::inputBamFN = "/testdata/test_input.bam";
const std::string IOTest::outputBedFN = "test_output.bed";
const std::string IOTest::outputBamFN = "test_output.bam";
