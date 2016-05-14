#include <string.h>
#include "bode/aligner.h"
#include "bode/alignerEndsFree.h"

namespace bode {

/** align_init
 *
 * * allocate a new DP matrix data structure
 * */
AlignerEndsFree::AlignerEndsFree(int ms,int ma,int mi,int go,int ge)
    : Aligner(ms,ma,mi,go,ge) {
  return;
}

void AlignerEndsFree::init(int str1len,int str2len) {
  int i,len;

  matrix[0][0] = 0;
  gap1[0][0] = NEGINF;
  gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    matrix[i][0] = NEGINF;
    matrix[0][i] = NEGINF;
    gap1[i][0] = 0;
    gap1[0][i] = NEGINF;
    gap2[i][0] = NEGINF;
    gap2[0][i] = 0;
  }
}

int AlignerEndsFree::stopTB(int i,int j) {
  return (i == 0 || j == 0);
}

int AlignerEndsFree::findEnd() {
  int maxscore,e1,e2,i,len1,len2,score;

  len1 = strlen(s1);
  len2 = strlen(s2);
  maxscore = NEGINF;
  e1 = -1;
  e2 = -1;
  for (i=1;i<=len1;i++) {
    score = max3(matrix[i][len2],gap1[i][len2],gap2[i][len2]);
    if (score > maxscore) {
      maxscore = score;
      e1 = i;
      e2 = len2;
    }
  }
  for (i=1;i<=len2;i++) {
    score = max3(matrix[len1][i],gap1[len1][i],gap2[len1][i]);
    if (score > maxscore) {
      maxscore = score;
      e1 = len1;
      e2 = i;
    }
  }
  end1 = e1 - 1;
  end2 = e2 - 1;
  return maxscore;
}

}
