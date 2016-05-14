#include <iostream>
#include <ostream>
#include <iomanip>
#include <string.h>
#include "bode/aligner.h"
#include <stdlib.h>
#include <stdio.h>

// testing...

namespace bode {

/** align_init
 *
 * * allocate a new DP matrix data structure
 * */
Aligner::Aligner(int ms,int ma,int mi,int go,int ge) {
  int max1;

  maxstr = ms;
  match = ma;
  mismatch = mi;
  gapopen = go;
  gapextend = ge;
  max1 = maxstr + 1;

  matrix = new int *[max1]; // calloc(maxstr+1,sizeof(int *));
  gap1 = new int *[max1];   // calloc(maxstr+1,sizeof(int *));
  gap2 = new int *[max1];   // calloc(maxstr+1,sizeof(int *));
  b1 = new char[maxstr*2];
  b2 = new char[maxstr*2];

  for (int i=0;i<=maxstr;i++) {
    matrix[i] = new int[max1]; // calloc(maxstr+1,sizeof(int));
    gap1[i] = new int[max1];   // calloc(maxstr+1,sizeof(int));
    gap2[i] = new int[max1];   // calloc(maxstr+1,sizeof(int));
  }
}

Aligner::~Aligner() {
  for (int i=0;i<=maxstr;i++) {
    delete matrix[i];
    delete gap1[i];
    delete gap2[i];
  }
  delete matrix;
  delete gap1;
  delete gap2;
  delete b1;
  delete b2;
}

int Aligner::align(char *str1, char *str2,Alignment *a) {
  int score;
  s1 = str1;
  s2 = str2;
  aln = a;
  init(strlen(str1),strlen(str2));
  fill();
  score = findEnd();
  traceback();
  return score;
}

void Aligner::fill() {
  int i,j;
  int len1,len2;

  len1 = strlen(s1);
  len2 = strlen(s2);
  for (i=1;i<=len1;i++) {
    for (j=1;j<=len2;j++) {
      matrix[i][j] = ((s1[i-1] == s2[j-1]) ? match : mismatch) +
                       max3(matrix[i-1][j-1],gap1[i-1][j-1],gap2[i-1][j-1]);
      gap1[i][j] = max3(gapopen+matrix[i-1][j],
                        gapextend+gap1[i-1][j],
                        gapopen+gap2[i-1][j]);
      gap2[i][j] = max3(gapopen+matrix[i][j-1],
                        gapextend+gap2[i][j-1],
                        gapopen+gap1[i][j-1]);
    }
  }
  return;
}

Aligner::direction Aligner::initialDir(int i,int j) {
  int mat,g1,g2;
  Aligner::direction dir;

  mat = matrix[i][j];
  g1 = gap1[i][j];
  g2 = gap2[i][j];
  if (mat >= g1 && mat >= g2) {
    dir = DIAG;
  } else if (g1 >= g2) {
    dir = UP;
  } else {
    dir = LEFT;
  }
  return dir;
}

Aligner::direction Aligner::getDir(int i,int j,Aligner::direction prevdir) {
  int mat,g1,g2;
  int val,vale,valo;
  Aligner::direction dir = Aligner::STOP;

  mat = matrix[i][j];
  g1 = gap1[i][j];
  g2 = gap2[i][j];
  switch (prevdir) {
    case DIAG:
      val = mat - ((s1[i-1] == s2[j-1]) ? match : mismatch);
      if (val == matrix[i-1][j-1]) {
        dir = DIAG;
      } else if (val == gap1[i-1][j-1]) {
        dir = UP;
      } else if (val == gap2[i-1][j-1]) {
        dir = LEFT;
      } else {
        std::cerr << "SHIT 1 (" << i << "," << j << ")" << std::endl;
        exit(-1);
      }
      break;
    case UP:
      vale = g1 - gapextend;
      valo = g1 - gapopen;
      if (valo == matrix[i-1][j]) {
        dir = DIAG;
      } else if (vale == gap1[i-1][j]) {
        dir = UP;
      } else if (valo == gap2[i-1][j]) {
        dir = LEFT;
      } else {
        std::cerr << "SHIT 2 (" << i << "," << j << ")" << std::endl;
        exit(-1);
      }
      break;
    case LEFT:
      vale = g2 - gapextend;
      valo = g2 - gapopen;
      if (valo == matrix[i][j-1]) {
        dir = DIAG;
      } else if (valo == gap1[i][j-1]) {
        dir = UP;
      } else if (vale == gap2[i][j-1]) {
        dir = LEFT;
      } else {
        std::cerr << "SHIT 3 (" << i << "," << j << ")" << std::endl;
        exit(-1);
      }
      break;
    case STOP:
      std::cerr << "SHIT 4 ... got STOP at (" << i << "," << j << ")" << std::endl;
      exit(-1);
  }
  return dir;
}

void Aligner::traceback() {
  int i,j,identity,oi,oj,alnpos;
  Aligner::direction dir;

  identity = 0;
  i = end1+1;
  j = end2+1;
  alnpos = maxstr*2-1;
  dir = initialDir(i,j);
  while (!stopTB(i,j)) {
    if (alnpos < 0) {
      fprintf(stderr,"traceback: alignment buffers too small, aborting\n");
      exit(-1);
    }
    oi = i; oj = j;
    switch (dir) {
      case UP:
        b1[alnpos] = s1[--i];
        b2[alnpos--] = '-';
        break;
      case DIAG:
        if (s1[--i] == s2[--j]) { identity++; }
        b1[alnpos] = s1[i];
        b2[alnpos--] = s2[j];
        break;
      case LEFT:
        b1[alnpos] = '-';
        b2[alnpos--] = s2[--j];
        break;
      default:
        fprintf(stderr,"dir(%d,%d) = %d\n",i,j,dir);
        exit(-1);
    }
    dir = getDir(oi,oj,dir);
  }
  aln->addSeq(b1+alnpos+1,i,end1);
  aln->addSeq(b2+alnpos+1,j,end2);

/*
  alnlen = bufferlen - alnpos - 1;
  for (k=0;k<alnlen;k++) {
    aln1[k] = aln1[k+alnpos+1];
    aln2[k] = aln2[k+alnpos+1];
  }
  aln1[alnlen] = '\0';
  aln2[alnlen] = '\0';
*/

  return;
}

void Aligner::dump(std::ostream *f,int indent,int debug,char *label) {
  int s1len, s2len,i,j;

  *f << std::setw(indent) << "" << "Aligner: " << label << std::endl;
  aln->dump(f,indent+2,label);
  if (debug) {
    *f << std::setw(indent) << "" << " maxstr=" << maxstr
                                  << " match=" << match
                                  << " mismatch=" << mismatch
                                  << " gapopen=" << gapopen
                                  << " gapextend=" << gapextend << std::endl;
    *f << std::setw(indent) << "" << " s1=" << s1 << std::endl
       << std::setw(indent) << "" << " s2=" << s2 << std::endl
       << std::setw(indent) << "" << " matrices:" << std::endl;
    
    s1len = strlen(s1);
    for (i=0;i<=s1len;i++) {
      *f << std::setw(indent) << "" << i;
      s2len = strlen(s2);
      for (j=0;j<=s2len;j++) {
        *f << " [" << matrix[i][j] << ","
                   << gap1[i][j] << ","
                   << gap2[i][j] << "]";
      }
      *f << std::endl;
    }
  }
  return;
}

}
