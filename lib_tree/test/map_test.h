#include <cxxtest/TestSuite.h>

#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
using namespace std;

#include "bode/intervalTree.h"

struct chromComp {
  bool operator()(const string &lhs,const string &rhs) const {
    int an = 0,bn = 0;
    const char *lc,*rc;
    lc = lhs.c_str();
    rc = rhs.c_str();
    an = atoi(lc+3);
    bn = atoi(rc+3);
    if (an > 0 && bn > 0) {
      return an < bn;
    } else {
      return lhs < rhs;
    }
  }
};

class MapTest: public CxxTest::TestSuite {
  public:
    void testMap(void) {
      std::map<std::string,int> *m;
      std::string x;

      m = new std::map<std::string,int>;
      x = "zork";

      (*m)[x] = 3;
      std::cout << (*m)[x] << std::endl;
      std::cout << x[0] << std::endl;
    }
    void testChromSort(void) {
      string a = "chr1";
      string b = "chr5";
      string c = "chr2";
      string d = "chrM";
      string e = "scaffold_1";
      map<string,int,chromComp> x;
      map<string,int,chromComp>::iterator it;

      x[b] = 5;
      x[c] = 2;
      x[a] = 1;
      x[e] = 99;
      x[d] = 50;

      for (it=x.begin();it!=x.end();it++) {
        cout << (*it).first << " == " << (*it).second << endl;
      }
      cout << x.count(a) << endl;
    }
};
