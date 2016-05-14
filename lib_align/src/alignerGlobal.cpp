#include <string.h>
#include "bode/aligner.h"
#include "bode/alignerGlobal.h"

namespace bode {
/** global alignment initialization
 *
 * initialize a DP matrix for global alignment, affine gaps
 */

AlignerGlobal::AlignerGlobal(int ms,int ma,int mi,int go,int ge):Aligner(ms,ma,mi,go,ge) {
  return;
}

void AlignerGlobal::init(int str1len,int str2len) {
  int i,len;

  matrix[0][0] = 0;
  gap1[0][0] = NEGINF;
  gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    matrix[i][0] = NEGINF;
    matrix[0][i] = NEGINF;
    gap1[i][0] = gapopen + (i-1)*gapextend;
    gap1[0][i] = NEGINF;
    gap2[i][0] = NEGINF;
    gap2[0][i] = gapopen + (i-1)*gapextend;
  }
}

int AlignerGlobal::stopTB(int i,int j) {
  return (i == 0 && j == 0);
}

int AlignerGlobal::findEnd() {
  int e1,e2;

  e1 = strlen(s1);
  e2 = strlen(s2);
  end1 = e1 - 1;
  end2 = e2 - 1;
  aln->setScore(max3(matrix[e1][e2],gap1[e1][e2],gap2[e1][e2]));
  return aln->score();
}

}
