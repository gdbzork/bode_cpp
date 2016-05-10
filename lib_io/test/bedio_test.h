#include <cxxtest/TestSuite.h>
#include <unistd.h>
#include <cstdlib>
#include "bode/bedReader.h"
#include "bode/writer.h"
#include "bode/interval.h"
#include "bode/bed.h"
#include "bode/util.h"

class BedTest: public CxxTest::TestSuite {
  private:
    static const std::string inputFN;
    static const std::string outputFN;
    static const int inputPositions[];
    static const std::string bedSeqs[];


  public:
    void testSanity(void) {
      bode::BedReader *br;
      bode::Interval *i;

      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      br = bode::BedReader::open(inputPath);
      i = br->nextI();
      TS_ASSERT_EQUALS(i->left(),9721288);
      TS_ASSERT_EQUALS(i->right(),i->left()+36);
      br->close();
      delete br;
    }

    void testReadCount(void) {
      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      bode::BedReader br(inputPath);
      bode::Interval *i;
      int count = 0;
      while ((i = br.nextI())) {
        count += 1;
      }
      TS_ASSERT_EQUALS(count,14);
      TS_ASSERT(br.eof());
      br.close();
    }

    void testReading(void) {
      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      bode::BedReader br(inputPath);
      bode::Interval *i;
      int count = 0;
      std::string x;
      while ((i = br.nextI())) {
        bode::Bed *bm = dynamic_cast<bode::Bed *>(i);
        TS_ASSERT_EQUALS(inputPositions[count],i->left());
        x = bm->name();
        TS_ASSERT_EQUALS(bedSeqs[count],x);
        count += 1;
      }
      TS_ASSERT_EQUALS(count,14);
      TS_ASSERT(br.eof());
      br.close();
    }

    void testWriting(void) {
      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      bode::BedReader br(inputPath);
      bode::Writer *bw;
      bode::Interval *i;

      bw = new bode::Writer(outputFN,br.header());
      while ((i = br.nextI())) {
        bw->write(*i);
      }
      br.close();
      bw->close();
      delete bw;
      std::string md1 = bode::md5(inputPath);
      std::string md2 = bode::md5(outputFN);
      TS_ASSERT_EQUALS(md1,md2);
      if (md1 == md2) {
        unlink(outputFN.c_str());
      }
    }

};

const std::string BedTest::inputFN = "/testdata/test_input.bed";
const std::string BedTest::outputFN = "test_output.bed";
const int BedTest::inputPositions[] = {9721288, 9721288, 9721288, 9721305,
                                       9721368, 9721368, 9721384, 9721384,
                                       9721396, 9721396, 9721396, 9721399,
                                       9721399, 9721399};
const std::string BedTest::bedSeqs[] = {"IL34_5480:8:22:12893:20493",
                                        "IL34_5480:8:52:13482:10897",
                                        "IL34_5480:8:96:16900:11829",
                                        "IL34_5480:8:102:4991:6802",
                                        "IL34_5480:8:32:2114:7952",
                                        "IL34_5480:8:90:8287:19682",
                                        "IL34_5480:8:1:14584:11764",
                                        "IL34_5480:8:19:14852:10952",
                                        "IL34_5480:8:7:6961:18711",
                                        "IL34_5480:8:33:9748:16445",
                                        "IL34_5480:8:79:6629:12340",
                                        "IL34_5480:8:30:13735:6787",
                                        "IL34_5480:8:74:10982:17331",
                                        "IL34_5480:8:109:14140:2153"};
