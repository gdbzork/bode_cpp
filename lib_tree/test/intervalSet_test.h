#include <cxxtest/TestSuite.h>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string>
#include "bode/interval.h"
#include "bode/intervalSet.h"

class IntervalSetTest: public CxxTest::TestSuite {
  private:
    bode::IntervalSet *x;
    std::string ch;

  public:

    void setUp(void) {
      x = new bode::IntervalSet();
      bode::Interval *y;
      ch = "chr1";
   
      TS_ASSERT_EQUALS(x->count(),0);
      
      y = new bode::Interval(ch,10,20);
      x->insert(y);
      y->update(ch,40,50);
      x->insert(y);
      y->update(ch,15,25);
      x->insert(y);
      y->update(ch,16,24);
      x->insert(y);
      delete y;
    }

    void tearDown(void) {
      delete x;
    }

    void testNodeCount(void) {
      TS_ASSERT_EQUALS(x->count(),4);
      TS_ASSERT_EQUALS(x->chromCount(),1);
    }

    void testCoverage(void) {
      TS_ASSERT_EQUALS(x->coverage(ch,5),0);
      TS_ASSERT_EQUALS(x->coverage(ch,9),0);
      TS_ASSERT_EQUALS(x->coverage(ch,10),1);
      TS_ASSERT_EQUALS(x->coverage(ch,14),1);
      TS_ASSERT_EQUALS(x->coverage(ch,15),2);
      TS_ASSERT_EQUALS(x->coverage(ch,16),3);
      TS_ASSERT_EQUALS(x->coverage(ch,19),3);
      TS_ASSERT_EQUALS(x->coverage(ch,20),2);
      TS_ASSERT_EQUALS(x->coverage(ch,23),2);
      TS_ASSERT_EQUALS(x->coverage(ch,24),1);
      TS_ASSERT_EQUALS(x->coverage(ch,25),0);
      TS_ASSERT_EQUALS(x->coverage(ch,26),0);
      TS_ASSERT_EQUALS(x->coverage(ch,39),0);
      TS_ASSERT_EQUALS(x->coverage(ch,40),1);
      TS_ASSERT_EQUALS(x->coverage(ch,49),1);
      TS_ASSERT_EQUALS(x->coverage(ch,50),0);
    }

    void testReadCount(void) {
      bode::Interval *y;
      y = new bode::Interval(ch,2,10);
      TS_ASSERT_EQUALS(x->overlapping(y),0);
      y->update(ch,2,11);
      TS_ASSERT_EQUALS(x->overlapping(y),1);
      y->update(ch,11,11);
      TS_ASSERT_EQUALS(x->overlapping(y),1);
      y->update(ch,5,60);
      TS_ASSERT_EQUALS(x->overlapping(y),4);
      y->update(ch,30,35);
      TS_ASSERT_EQUALS(x->overlapping(y),0);
      y->update(ch,10,17);
      TS_ASSERT_EQUALS(x->overlapping(y),3);
      y->update(ch,2,17);
      TS_ASSERT_EQUALS(x->overlapping(y),3);
      y->update(ch,24,41);
      TS_ASSERT_EQUALS(x->overlapping(y),2);
      y->update(ch,23,41);
      TS_ASSERT_EQUALS(x->overlapping(y),3);
      y->update(ch,25,40);
      TS_ASSERT_EQUALS(x->overlapping(y),0);
      y->update(ch,25,41);
      TS_ASSERT_EQUALS(x->overlapping(y),1);
      delete y;
    }

    void testRandom(void) {
      int **saved;
      int i,j,k;
      int range = 10000;
      int samples = 80000;
      int length = 40;
      std::string chroms[] = {"chr1","chr2","chr3","chr4","chr5","chr6"};
      int nchroms = 6;
      bode::IntervalSet *tree;
      bode::Interval *bi;
 
      saved = new int *[nchroms];
      for (i=0;i<nchroms;i++) {
        saved[i] = new int[range];
        for (j=0;j<range;j++) {
          saved[i][j] = 0;
        }
      }
      tree = new bode::IntervalSet();
      bi = new bode::Interval();
      for (i=0;i<samples;i++) {
        int c = rand() % nchroms;
        j = rand() % (range - length);
        for (k=j;k<j+length;k++) {
          saved[c][k]++;
        }
        bi->update(chroms[c],j,j+length);
        tree->insert(bi);
      }
      for (int c=0;c<nchroms;c++) {
        for (i=0;i<range;i++) {
          TS_ASSERT_EQUALS(saved[c][i],tree->coverage(chroms[c],i));
        }
      }
      TS_ASSERT_EQUALS(samples,tree->realCount());
      TS_ASSERT_EQUALS(samples,tree->count());

      delete tree;
      for (i=0;i<nchroms;i++) {
        delete[] saved[i];
      }
      delete[] saved;
      delete bi;
    }

    void testChromosomeOrder(void) {
      const std::string chroms[] = {"chr1","chr2","chr3","chr4","chr5","chr6","chr7",
                              "chr8","chr9","chr10","chr11","chr12","chr13",
                              "chr14","chr15","chr16","chr17","chr18","chr19",
                              "chr20","chr21","chr22","chrM","chrX","chrY",
                              "scaffold_23"};
      int chromNum = 26;
      bode::IntervalSet *tree;
      int i,j;
      std::vector<std::string> cshuff;
      std::map<std::string,bode::IntervalTree *,bode::chromComp>::iterator it;
      cshuff.assign(chroms,chroms+chromNum);
      srand((unsigned)time(NULL));
      for (i=0;i<100;i++) {
        random_shuffle(cshuff.begin(),cshuff.end());
        tree = new bode::IntervalSet();
        bode::Interval *bi = new bode::Interval();
        for (j=0;j<chromNum;j++) {
          bi->update(cshuff[j],10,30);
          tree->insert(bi);
        }
        delete bi;
        it = tree->chromIter();
        for (j=0;j<chromNum;j++) {
          TS_ASSERT_EQUALS(chroms[j],it->first);
          it++;
        }
        delete tree;
      }
    }
};
