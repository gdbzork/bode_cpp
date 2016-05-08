#define CXXTEST_HAVE_EH
#define CXXTEST_ABORT_TEST_ON_FAIL true
#include <cxxtest/TestSuite.h>
#include <stdexcept>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>

#include <htslib/sam.h>

#include "bode/densitySet.h"
#include "bode/intervalDensity.h"

extern char **environ;

class DensitySetTest: public CxxTest::TestSuite {

  public:

    void setUp(void) {
      dset = new bode::DensitySet(30,chroms,left,right);
    }

    void tearDown(void) {
      delete dset;
    }

    void testSanity(void) {
      std::string chr("chr1");
      int p;
      unsigned int h;
      dset->add(chr,172715670,172715690);
      dset->summit(0,p,h);
      TS_ASSERT_EQUALS(p,172715679);
      TS_ASSERT_EQUALS(h,1);
    }

    void  testCorrectChrom(void) {
      std::string chr1("chr1");
      std::string chr3("chr3");
      std::string chr11("chr11");
      dset->add(chr1,220908037,220908046);
      dset->add(chr1,220908040,220908056);
      dset->add(chr3,158887898,158887905);
      dset->add(chr3,158887900,158887995);
      dset->add(chr11,46295574,46295583);
      dset->add(chr11,46295580,46295584);

      int p;
      unsigned int h;
      dset->summit(10,p,h);
      TS_ASSERT_EQUALS(p,220908042);
      TS_ASSERT_EQUALS(h,2);

      dset->summit(4,p,h);
      TS_ASSERT_EQUALS(p,158887902);
      TS_ASSERT_EQUALS(h,2);

      dset->summit(12,p,h);
      TS_ASSERT_EQUALS(p,46295581);
      TS_ASSERT_EQUALS(h,2);
    }

    void testCorrectInterval(void) {
      std::string chr1("chr1");
      dset->add(chr1,32787534,32787545);
      dset->add(chr1,32787524,32787535);
      dset->add(chr1,32787537,32787575);
      dset->add(chr1,220908136,220908157);
      dset->add(chr1,220908156,220908177);
      dset->add(chr1,172715708,172715739);
      dset->add(chr1,172715718,172715729);

      int p;
      unsigned int h;
      dset->summit(0,p,h);
      TS_ASSERT_EQUALS(p,172715723);
      TS_ASSERT_EQUALS(h,2);

      dset->summit(3,p,h);
      TS_ASSERT_EQUALS(p,32787539);
      TS_ASSERT_EQUALS(h,2);

      dset->summit(5,p,h);
      TS_ASSERT_EQUALS(p,32787539);
      TS_ASSERT_EQUALS(h,2);

      dset->summit(10,p,h);
      TS_ASSERT_EQUALS(p,220908156);
      TS_ASSERT_EQUALS(h,2);
    }

    void testNarrowInterval(void) {
      std::string chr10("chr10");
      int p;
      unsigned int h;
      dset->add(chr10,95242400,95242410);
      dset->add(chr10,95242350,95242450);
      dset->summit(29,p,h);
      TS_ASSERT_EQUALS(p,95242403);
      TS_ASSERT_EQUALS(h,2);
    }

    void loadBed(std::string fn,std::string chrom[],int *left,int *right) {
      std::ifstream bed;
      bed.open(fn.c_str());
      for (int i=0;i<6200;i++) {
        bed >> chrom[i];
        bed >> left[i];
        bed >> right[i];
      }
      bed.close();
    }

    void testBigFile(void) {
      std::string *chroms;
      int *left;
      int *right;
      bode::DensitySet *ds;
      samFile *fd;
      bam1_t *seq;
      bam_hdr_t *header;
      char *srcdir;
      int count;
/*
      int i;
      i = 0;
      while (environ[i] != NULL) {
        fprintf(stdout,"%s\n",environ[i]);
        i++;
      }
*/

      srcdir = getenv("srcdir");
      std::string datadir(getenv("srcdir"));
      std::string bampath = datadir + "/testdata/test_data_chr21.bam";
      std::string bedpath = datadir + "/testdata/test_peaks_chr21.bed";
      
      std::string bamfile(bampath);
      std::string chr(1024,' ');

      chroms = new std::string[6200];
      left = new int[6200];
      right = new int[6200];
      loadBed(bedpath,chroms,left,right);
      ds = new bode::DensitySet(6200,chroms,left,right);
      fd = sam_open(bamfile.c_str(),"rb");
      header = sam_hdr_read(fd);
      seq = bam_init1();
      count = 0;
      while (sam_read1(fd,header,seq) > 0) {
        if (!(seq->core.flag&BAM_FUNMAP)) {
          chr.assign(std::string(header->target_name[seq->core.tid]));
          ds->add(chr,seq->core.pos,seq->core.pos + seq->core.l_qseq);
          count++;
        }
      }
      sam_close(fd);
      TS_ASSERT_EQUALS(count,496159);
    }

  private:
    static std::string chroms[];
    static int left[];
    static int right[];
    bode::DensitySet *dset;
};

std::string DensitySetTest::chroms[] = { "chr1", "chr20", "chr5", "chr1",
                             "chr3", "chr1", "chr4", "chr20", "chr9",
                             "chr3", "chr1", "chr9", "chr11", "chr17",
                             "chr2", "chr5", "chr7", "chr14", "chr10",
                             "chr3", "chr12", "chr22", "chr15", "chr5",
                             "chr7", "chr12", "chr8", "chr7", "chr7",
                             "chr10" };

int DensitySetTest::left[] = { 172715668, 5825189, 38550153, 32787503,
                   158887798, 32787504, 180337151, 35830844, 36924190,
                   135985517, 220908036, 18967522, 46295573, 5736354,
                   60760330, 113994127, 36298215, 54575614, 71463601,
                   58457247, 104997565, 24604171, 66125142, 147525609,
                   95714786, 10548442, 87121083, 5553924, 122831480,
                   95242403 };

int DensitySetTest::right[] = { 172715769, 5826190, 38551154, 32787604,
                    158887999, 32787565, 180338152, 35830945, 36924391,
                    135986518, 220908237, 18967543, 46295594, 5736358,
                    60760531, 113994228, 36299216, 54575615, 71463602,
                    58457248, 104997566, 24604172, 66125143, 147525610,
                    95714787, 10548443, 87121084, 5553925, 122831481,
                    95242404 };
