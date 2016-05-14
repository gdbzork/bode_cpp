#include <string.h>
#include "bode/aligner.h"
#include "bode/alignerPartialFree.h"

namespace bode {

/** align_init
 *
 * * allocate a new DP matrix data structure
 * */
AlignerPartialFree::AlignerPartialFree(int ms,int ma,int mi,int go,int ge)
    : Aligner(ms,ma,mi,go,ge) {
  return;
}

/* align_init_local
 *
 * * initialize a DP matrix for local alignment, affine gaps
 * */
void AlignerPartialFree::init(int str1len,int str2len) {
  int i,len;

  matrix[0][0] = 0;
  gap1[0][0] = NEGINF;
  gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    matrix[i][0] = NEGINF;
    matrix[0][i] = NEGINF;
    gap1[i][0] = gapopen + (i-1) * gapextend;
    gap1[0][i] = NEGINF;
    gap2[i][0] = NEGINF;
    gap2[0][i] = 0;
  }
}

int AlignerPartialFree::stopTB(int i,int j) {
  (void) j;
  return i == 0;
}

int AlignerPartialFree::findEnd() {
  int maxscore,score,e1,e2,len1,len2,i;

  len1 = strlen(s1);
  len2 = strlen(s2);
  maxscore = NEGINF;
  e1 = -1;
  e2 = -1;
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
