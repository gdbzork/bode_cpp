#include <string.h>
#include "bode/alignment.h"
#include "bode/alignerLocal.h"

namespace bode {

/** align_init_local
 *
 * initialize a DP matrix for local alignment, affine gaps
 */
void AlignerLocal::init(int str1len,int str2len) {
  int len;

  matrix[0][0] = 0;
  gap1[0][0] = NEGINF;
  gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (int i=1;i<=len;i++) {
    matrix[i][0] = NEGINF;
    matrix[0][i] = NEGINF;
    gap1[i][0] = 0;
    gap1[0][i] = NEGINF;
    gap2[i][0] = NEGINF;
    gap2[0][i] = 0;
  }
}

void AlignerLocal::fill() {
  int i,j;
  int score,thisscore;
  int len1,len2;

  end1 = -1;
  end2 = -1;

  score = 0;
  len1 = strlen(s1);
  len2 = strlen(s2);
  for (i=1;i<=len1;i++) {
    for (j=1;j<=len2;j++) {
      matrix[i][j] = max2(0,
                          (s1[i-1]==s2[j-1]?match:mismatch)+
                          max3(matrix[i-1][j-1],gap1[i-1][j-1],gap2[i-1][j-1]));
      gap1[i][j] = max2(0,max3(gapopen+matrix[i-1][j],
                               gapextend+gap1[i-1][j],
                               gapopen+gap2[i-1][j]));
      gap2[i][j] = max2(0,max3(gapopen+matrix[i][j-1],
                               gapextend+gap2[i][j-1],
                               gapopen+gap1[i][j-1]));
      thisscore = max3(matrix[i][j],gap1[i][j],gap2[i][j]);
      if (thisscore > score) {
        score = thisscore;
        end1 = i;
        end2 = j;
      }
    }
  }
  return;
}

int AlignerLocal::stopTB(int i,int j) {
  return (i == 0 || j == 0 || max3(matrix[i][j],gap1[i][j],gap2[i][j]) <= 0);
}

int AlignerLocal::findEnd() {
  return max3(matrix[end1][end2],gap1[end1][end2],gap2[end1][end2]);
}

}
