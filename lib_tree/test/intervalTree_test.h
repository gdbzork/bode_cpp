#include <cxxtest/TestSuite.h>
#include <time.h>
#include <cstdlib>
#include "bode/intervalTree.h"

class IntervalTreeTest: public CxxTest::TestSuite {
  private:
    bode::IntervalTree *x;

  public:
    void setUp(void) {
      x = new bode::IntervalTree();
   
      TS_ASSERT_EQUALS(x->getCount(),0);
      x->insert(10,20);
      x->insert(40,50);
      x->insert(15,25);
      x->insert(16,24);
    }
    void tearDown(void) {
      delete x;
    }

    void testNodeCount(void) {
      TS_ASSERT_EQUALS(x->getCount(),4);
    }
    void testCoverage(void) {
      TS_ASSERT_EQUALS(x->coverage(5),0);
      TS_ASSERT_EQUALS(x->coverage(9),0);
      TS_ASSERT_EQUALS(x->coverage(10),1);
      TS_ASSERT_EQUALS(x->coverage(14),1);
      TS_ASSERT_EQUALS(x->coverage(15),2);
      TS_ASSERT_EQUALS(x->coverage(16),3);
      TS_ASSERT_EQUALS(x->coverage(19),3);
      TS_ASSERT_EQUALS(x->coverage(20),2);
      TS_ASSERT_EQUALS(x->coverage(23),2);
      TS_ASSERT_EQUALS(x->coverage(24),1);
      TS_ASSERT_EQUALS(x->coverage(25),0);
      TS_ASSERT_EQUALS(x->coverage(26),0);
      TS_ASSERT_EQUALS(x->coverage(39),0);
      TS_ASSERT_EQUALS(x->coverage(40),1);
      TS_ASSERT_EQUALS(x->coverage(49),1);
      TS_ASSERT_EQUALS(x->coverage(50),0);
    }
    void testReadCount(void) {
      TS_ASSERT_EQUALS(x->reads(2,10),0);
      TS_ASSERT_EQUALS(x->reads(2,11),1);
      TS_ASSERT_EQUALS(x->reads(11,11),1);
      TS_ASSERT_EQUALS(x->reads(5,60),4);
      TS_ASSERT_EQUALS(x->reads(30,35),0);
      TS_ASSERT_EQUALS(x->reads(10,17),3);
      TS_ASSERT_EQUALS(x->reads(2,17),3);
      TS_ASSERT_EQUALS(x->reads(24,41),2);
      TS_ASSERT_EQUALS(x->reads(23,41),3);
      TS_ASSERT_EQUALS(x->reads(25,40),0);
      TS_ASSERT_EQUALS(x->reads(25,41),1);
    }
    void EXCLUDE_testLong(void) {
      clock_t start,end;
      bode::IntervalTree *x;
      int i,total=10000000;
      double elapsed;
      char buffer[1024];

      start = clock();
      x = new bode::IntervalTree();
      for (i=0;i<total;i++) {
        x->insert(i,i+40);
      }
      TS_ASSERT_EQUALS(x->getCount(),total);
      end = clock();
      elapsed = (double)(end - start) / CLOCKS_PER_SEC;
      sprintf(buffer,"tree elapsed: %5.3f sec (%d intervals)\n",elapsed,total);
      TS_TRACE(buffer);
    }
    void testRandom(void) {
      int *saved;
      int i,j,k;
      int range = 10000;
      int samples = 4000;
      int length = 40;
      bode::IntervalTree *tree;
 
      saved = new int[range];
      tree = new bode::IntervalTree();
      for (i=0;i<range;i++) {
        saved[i] = 0;
      }
      for (i=0;i<samples;i++) {
        j = rand() % (range - length);
        for (k=j;k<j+length;k++) {
          saved[k]++;
        }
        tree->insert(j,j+length);
      }
      for (i=0;i<range;i++) {
        TS_ASSERT_EQUALS(saved[i],tree->coverage(i));
      }

      delete[] saved;
      delete tree;
    }

    void testDensitySmall(void) {
      int *dtest;

      dtest = new int[60];
      for (int i=0;i<60;i++) {
        dtest[i] = 0;
      }
      x->density(0,60,dtest);
      TS_ASSERT_EQUALS(dtest[5],0);
      TS_ASSERT_EQUALS(dtest[9],0);
      TS_ASSERT_EQUALS(dtest[10],1);
      TS_ASSERT_EQUALS(dtest[14],1);
      TS_ASSERT_EQUALS(dtest[15],2);
      TS_ASSERT_EQUALS(dtest[16],3);
      TS_ASSERT_EQUALS(dtest[19],3);
      TS_ASSERT_EQUALS(dtest[20],2);
      TS_ASSERT_EQUALS(dtest[23],2);
      TS_ASSERT_EQUALS(dtest[24],1);
      TS_ASSERT_EQUALS(dtest[25],0);
      TS_ASSERT_EQUALS(dtest[26],0);
      TS_ASSERT_EQUALS(dtest[39],0);
      TS_ASSERT_EQUALS(dtest[40],1);
      TS_ASSERT_EQUALS(dtest[49],1);
      TS_ASSERT_EQUALS(dtest[50],0);
      TS_ASSERT_EQUALS(dtest[59],0);
      delete[] dtest;
    }

    void testDensityOffset(void) {
      int *dtest;

      dtest = new int[12];
      for (int i=0;i<12;i++) {
        dtest[i] = 0;
      }
      x->density(15,25,dtest);
      TS_ASSERT_EQUALS(dtest[0],2);
      TS_ASSERT_EQUALS(dtest[1],3);
      TS_ASSERT_EQUALS(dtest[4],3);
      TS_ASSERT_EQUALS(dtest[5],2);
      TS_ASSERT_EQUALS(dtest[8],2);
      TS_ASSERT_EQUALS(dtest[9],1);
      TS_ASSERT_EQUALS(dtest[10],0);
      TS_ASSERT_EQUALS(dtest[11],0);
      delete[] dtest;
    }

    void testDensityRandom(void) {
      int *saved;
      int *dtest;
      int i,j,k;
      int range = 10000;
      int samples = 4000;
      int length = 40;
      bode::IntervalTree *tree;
 
      saved = new int[range];
      dtest = new int[range];
      tree = new bode::IntervalTree();
      for (i=0;i<range;i++) {
        saved[i] = 0;
        dtest[i] = 0;
      }
      for (i=0;i<samples;i++) {
        j = rand() % (range - length);
        for (k=j;k<j+length;k++) {
          saved[k]++;
        }
        tree->insert(j,j+length);
      }
      tree->density(0,range,dtest);
      int bad = 0;
      for (i=0;i<range;i++) {
        TS_ASSERT_EQUALS(saved[i],dtest[i]);
        if (saved[i] != dtest[i]) {
          bad++;
        }
      }
      if (bad > 0) {
        fprintf(stderr,"bad = %d\n",bad);
      }
      
      delete[] saved;
      delete[] dtest;
      delete tree;
    }

    void testDensityRandomSlice(void) {
      int *saved;
      int *dtest;
      int i,j,k;
      int range = 10000;
      int samples = 4000;
      int length = 40;
      bode::IntervalTree *tree;
 
      saved = new int[range];
      dtest = new int[100];
      tree = new bode::IntervalTree();
      for (i=0;i<range;i++) {
        saved[i] = 0;
      }
      for (i=0;i<100;i++) {
        dtest[i] = 0;
      }
      for (i=0;i<samples;i++) {
        j = rand() % (range - length);
        for (k=j;k<j+length;k++) {
          saved[k]++;
        }
        tree->insert(j,j+length);
      }
      tree->density(5000,5100,dtest);
      int bad = 0;
      for (i=0;i<100;i++) {
        TS_ASSERT_EQUALS(saved[i+5000],dtest[i]);
        if (saved[i+5000] != dtest[i]) {
          bad++;
        }
      }
      if (bad > 0) {
        fprintf(stderr,"bad = %d\n",bad);
      }
      
      delete[] saved;
      delete[] dtest;
      delete tree;
    }
};
