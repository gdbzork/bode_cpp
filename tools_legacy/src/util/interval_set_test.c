#include <stdio.h>

#include "interval_set.h"

int input[][2] = {{10,20},
                  {15,30},
                  {19,40},
                  {50,60},
                  {70,80},
                  {75,85},
                  {75,85},
                  {0,0}};

int testPoint[][2] = {{11,1},
                      {15,2},
                      {19,3},
                      {5,0},
                      {25,2},
                      {35,1},
                      {40,0},
                      {0,0}};

int testInterval[][3] = {{5,35,3},
                         {5,18,2},
                         {35,55,2},
                         {55,65,1},
                         {60,69,0},
                         {70,80,3},
                         {-10,9,0},
                         {90,100,0},
                         {75,80,3},
                         {80,81,2},
                         {0,0,0}};

int main() {
  interval_set iset;
  int x,pos,passed,total;

  iset = iset_new();
  pos = 0;
  while (input[pos][0] != 0 || input[pos][1] != 0) {
    iset_insert(iset,input[pos][0],input[pos][1]);
    pos += 1;
  }
  iset_dump(iset,stderr);
  fprintf(stderr,"---\n");
  total = 0;
  passed = 0;
  pos = 0;
  while (testPoint[pos][0] != 0 || testPoint[pos][1] != 0) {
    x = iset_depth_at_point(iset,testPoint[pos][0]);
    if (x != testPoint[pos][1]) {
      fprintf(stderr,"failed %d: depth=%d, should=%d pos=%d\n",
                     total,x,testPoint[pos][1],testPoint[pos][0]);
    } else {
      passed++;
    }
    total++;
    pos += 1;
  }
  pos = 0;
  while (testInterval[pos][0] != 0 || testInterval[pos][1] != 0) {
    x = iset_coverage(iset,testInterval[pos][0],testInterval[pos][1]);
    if (x != testInterval[pos][2]) {
      fprintf(stderr,"failed %d: depth=%d, should=%d pos=%d-%d\n",
                     total,x,testInterval[pos][2],testInterval[pos][0],
                     testInterval[pos][1]);
    } else {
      passed++;
    }
    total++;
    pos += 1;
  }
  fprintf(stderr,"passed %d of %d\n",passed,total);
  return (passed == total);
}
