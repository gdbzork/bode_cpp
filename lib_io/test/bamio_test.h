#include <cxxtest/TestSuite.h>
#include <unistd.h>
#include <cstdlib>

#include "bode/bamReader.h"
#include "bode/bamWriter.h"
#include "bode/interval.h"
#include "bode/bam.h"
#include "bode/util.h"

class BamTest: public CxxTest::TestSuite {
  private:
    static const std::string inputFN;
    static const std::string samFN;
    static const std::string outputFN;
    static const int inputPositions[];
    static const std::string samSeqs[];


  public:
    void testSanity(void) {
      bode::BamReader *br;
      bode::Interval *i;

      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      br = bode::BamReader::open(inputPath);
      TS_ASSERT_EQUALS(((bam_hdr_t *)br->header())->n_targets,25);
      TS_ASSERT_EQUALS(strcmp(((bam_hdr_t *)br->header())->target_name[12],"chr21"),0);
      TS_ASSERT_EQUALS(((bam_hdr_t *)br->header())->target_len[12],(uint32_t)46944323);
      i = br->nextI();
      TS_ASSERT_EQUALS(i->left(),9721288);
      TS_ASSERT_EQUALS(i->right(),i->left()+36);
      br->close();
      delete br;
    }

    void testReadCount(void) {
      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      bode::BamReader br(inputPath);
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

      bode::BamReader br(inputPath);
      bode::Interval *i;
      int count = 0;
      std::string x;
      while ((i = br.nextI())) {
        bode::Bam *bm = dynamic_cast<bode::Bam *>(i);
        TS_ASSERT_EQUALS(inputPositions[count],i->left());
        bm->seq(x);
        TS_ASSERT_EQUALS(samSeqs[count],x);
        count += 1;
      }
      TS_ASSERT_EQUALS(count,14);
      TS_ASSERT(br.eof());
      br.close();
    }

    void testWriting(void) {
      std::string datadir(getenv("srcdir"));
      std::string inputPath = datadir + inputFN;

      bode::BamReader br(inputPath);
      bode::BamWriter *bw;
      bode::Interval *i;

      bw = new bode::BamWriter(outputFN,br.header());
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

const std::string BamTest::inputFN = "/testdata/test_input.bam";
const std::string BamTest::samFN = "/testdata/test_input.sam";
const std::string BamTest::outputFN = "test_output.bam";
const int BamTest::inputPositions[] = {9721288, 9721288, 9721288, 9721305,
                                       9721368, 9721368, 9721384, 9721384,
                                       9721396, 9721396, 9721396, 9721399,
                                       9721399, 9721399};
const std::string BamTest::samSeqs[] = {"ATAAAAAATAGACAGAAGCATTGTGAGAAACATCTC",
                                        "ATAAAAAATAGACAGAAGCATTGTGAGAAACATCTC",
                                        "ATAAAAAATAGACAGAAGCATTGTGAGAAACATCTC",
                                        "TCATTGTGAGAAACATCTCTGTGATGTGTGCATTCA",
                                        "TGAGCAGTTTGGAAACAGTCCTTTTGTAGAATCTGC",
                                        "TGAGCAGTTTGGAAACAGTCCTTTTGTAGAATCTGC",
                                        "AGTACTTTTGTAGAATCTGTAAAGGGATATTTCTGA",
                                        "AGTACTTTTGTAGAATCTGTAAAGGGATATTTCTGA",
                                        "GAATCTGCAAAGGGATATTTCTGAGCCCATTGAGGC",
                                        "GAATCTGCAAAGGGATATTTCTGAGCCCATTGAGGC",
                                        "GAATCTGCAAAGGGATATTTCTGAGCCCATTGAGGC",
                                        "TCTGCAAAGGGATATTTCTGAGCCCATTGAAGGCTA",
                                        "TCTGCAAAGGGATATTTCTGAGCCCATTGAAGGCTA",
                                        "TCTGCAAAGGGATATTTCTGAGCCCATTGAAGGCTA"};
