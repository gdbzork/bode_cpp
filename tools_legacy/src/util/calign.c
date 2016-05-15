#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calign.h"

#define VERBOSE
#define UP 3
#define DIAG 2
#define LEFT 1
#define STOP 0
#define NEGINF -100000

#define max3(a,b,c) (((a) > (b)) ? (((a) > (c)) ? (a) : (c)) : (((b) > (c)) ? (b) : (c)))
#define max2(a,b) (((a) > (b)) ? (a) : (b))

/*
 * alignment_init
 *
 * * alocate and initialize a new alignment data structure
 * */
alignmentp alignment_init(void) {
  alignmentp data;

  data = malloc(sizeof(struct alignment));
  if (data == NULL) {
#ifdef VERBOSE
    fprintf(stderr,"alignment_init: failed to allocate struct alignment\n");
#endif
    return NULL;
  }

  data->aln1 = NULL;
  data->aln2 = NULL;
  data->start1 = -1;
  data->start2 = -1;
  data->end1 = -1;
  data->end2 = -1;
  data->alignlen = -1;
  data->identity = -1;
  data->score = NEGINF;
  return data;
}

/* align_init
 *
 * * allocate a new DP matrix data structure
 * */
aligndatap align_init(int maxstr,
                      int match,
                      int mismatch,
                      int gapopen,
                      int gapextend) {
  aligndatap data;
  int i;

  data = (aligndatap) malloc(sizeof(struct aligndata));
  if (data == NULL) {
#ifdef VERBOSE
    fprintf(stderr,"align_init: failed to allocate struct aligndata\n");
#endif
    return NULL;
  }

  data->maxstr = maxstr;
  data->match = match;
  data->mismatch = mismatch;
  data->gapopen = gapopen;
  data->gapextend = gapextend;

  data->matrix = calloc(maxstr+1,sizeof(int *));
  data->gap1 = calloc(maxstr+1,sizeof(int *));
  data->gap2 = calloc(maxstr+1,sizeof(int *));

  if (data->matrix == NULL ||
      data->gap1 == NULL ||
      data->gap2 == NULL) {
#ifdef VERBOSE
    fprintf(stderr,"align_init: failed to allocate matrices\n");
#endif
    return NULL;
  }

  for (i=0;i<=maxstr;i++) {
    data->matrix[i] = calloc(maxstr+1,sizeof(int));
    data->gap1[i] = calloc(maxstr+1,sizeof(int));
    data->gap2[i] = calloc(maxstr+1,sizeof(int));
    if (data->matrix[i] == NULL ||
        data->gap1[i] == NULL ||
        data->gap2[i] == NULL) {
#ifdef VERBOSE
      fprintf(stderr,"align_init: failed to allocate sub-matrices (%d)\n",i);
#endif
      return NULL;
    }
  }

  data->alignment = alignment_init();
  if (data->alignment == NULL) {
    return NULL;
  }
  return data;
}

/* align_init_local
 *
 * * initialize a DP matrix for local alignment, affine gaps
 * */
void align_init_local(aligndatap data,int str1len,int str2len) {
  int i,len;

  data->matrix[0][0] = 0;
  data->gap1[0][0] = NEGINF;
  data->gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    data->matrix[i][0] = NEGINF;
    data->matrix[0][i] = NEGINF;
    data->gap1[i][0] = 0;
    data->gap1[0][i] = NEGINF;
    data->gap2[i][0] = NEGINF;
    data->gap2[0][i] = 0;
  }
}

int align_fill_local(aligndatap data) {
  int i,j;
  int score,end1,end2,thisscore;
  int match,mismatch,gapopen,gapextend;
  int **matrix,**gap1,**gap2;
  char *str1,*str2;
  char *str1i,*str2j;
  int len1,len2,temp1,temp2,temp3,temp4,temp5;
  int *matij,*matimjm,*matijm,*matimj,**matrow,**matrowm;
  int *gap1ij,*gap1imjm,*gap1ijm,*gap1imj,**gap1row,**gap1rowm;
  int *gap2ij,*gap2imjm,*gap2ijm,*gap2imj,**gap2row,**gap2rowm;

  /* store matrix values locally, for speed and conciseness */
  match = data->match;
  mismatch = data->mismatch;
  gapopen = data->gapopen;
  gapextend = data->gapextend;
  matrix = data->matrix;
  gap1 = data->gap1;
  gap2 = data->gap2;
  str1 = data->s1;
  str2 = data->s2;
  end1 = -1;
  end2 = -1;

  score = 0;
  len1 = strlen(str1);
  len2 = strlen(str2);
  str1i = str1;
  matrowm = matrix;
  matrow = matrix+1;
  gap1rowm = gap1;
  gap1row = gap1rowm+1;
  gap2rowm = gap2;
  gap2row = gap2rowm+1;
  for (i=1;i<=len1;i++) {
    str2j = str2;
    matijm = *matrow;
    matij = matijm+1;
    matimjm = *matrowm;
    matimj = matimjm+1;
    gap1ijm = *gap1row;
    gap1ij = gap1ijm+1;
    gap1imjm = *gap1rowm;
    gap1imj = gap1imjm+1;
    gap2ijm = *gap2row;
    gap2ij = gap2ijm+1;
    gap2imjm = *gap2rowm;
    gap2imj = gap2imjm+1;
    for (j=1;j<=len2;j++) {
      temp1 = ((*str1i == *str2j) ? match : mismatch) +
              max3(*matimjm,*gap1imjm,*gap2imjm);
      *matij = max2(temp1,0);
      temp2 = gapopen + *matimj;
      temp3 = gapextend + *gap1imj;
      temp4 = gapopen + *gap2imj;
      temp5 = max3(temp2,temp3,temp4);
      *gap1ij = max2(temp5,0);
      temp2 = gapopen + *matijm;
      temp3 = gapextend + *gap2ijm;
      temp4 = gapopen + *gap1ijm;
      temp5 = max3(temp2,temp3,temp4);
      *gap2ij = max2(temp5,0);
      thisscore = max3(*matij,*gap1ij,*gap2ij);
      if (thisscore > score) {
        score = thisscore;
        end1 = i;
        end2 = j;
      }
      str2j++;
      matijm++;
      matimjm++;
      matimj++;
      matij++;
      gap1ijm++;
      gap1imjm++;
      gap1imj++;
      gap1ij++;
      gap2ijm++;
      gap2imjm++;
      gap2imj++;
      gap2ij++;
    }
    str1i++;
    matrowm++;
    matrow++;
    gap1rowm++;
    gap1row++;
    gap2rowm++;
    gap2row++;
  }
  data->alignment->end1 = end1 - 1;
  data->alignment->end2 = end2 - 1;
  data->alignment->score = score;
  return score;
}

int align_stop_local(aligndatap data,int i,int j) {
  return (i == 0 || j == 0 ||
          max3(data->matrix[i][j],data->gap1[i][j],data->gap2[i][j]) <= 0);
}

int align_local(aligndatap data,char *str1,char *str2) {
  data->s1 = str1;
  data->s2 = str2;
  data->alntype = ALN_LOCAL;
  align_init_local(data,strlen(str1),strlen(str2));
  return align_fill_local(data);
}

/* align_init_global
 *
 * * initialize a DP matrix for global alignment, affine gaps
 * */
void align_init_global(aligndatap data,int str1len,int str2len) {
  int i,len;

  data->matrix[0][0] = 0;
  data->gap1[0][0] = NEGINF;
  data->gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    data->matrix[i][0] = NEGINF;
    data->matrix[0][i] = NEGINF;
    data->gap1[i][0] = data->gapopen + (i-1)*data->gapextend;
    data->gap1[0][i] = NEGINF;
    data->gap2[i][0] = NEGINF;
    data->gap2[0][i] = data->gapopen + (i-1)*data->gapextend;
  }
}

int align_fill_nonlocal(aligndatap data,void (*findendpoint)(aligndatap)) {
  int i,j;
  int match,mismatch,gapopen,gapextend;
  int **matrix,**gap1,**gap2;
  char *str1,*str2;
  char *str1i,*str2j;
  int len1,len2,temp2,temp3,temp4;
  int *matij,*matimjm,*matijm,*matimj,**matrow,**matrowm;
  int *gap1ij,*gap1imjm,*gap1ijm,*gap1imj,**gap1row,**gap1rowm;
  int *gap2ij,*gap2imjm,*gap2ijm,*gap2imj,**gap2row,**gap2rowm;

  /* store matrix values locally, for speed and conciseness */
  match = data->match;
  mismatch = data->mismatch;
  gapopen = data->gapopen;
  gapextend = data->gapextend;
  matrix = data->matrix;
  gap1 = data->gap1;
  gap2 = data->gap2;
  str1 = data->s1;
  str2 = data->s2;

  /* score = 0; */
  len1 = strlen(str1);
  len2 = strlen(str2);
  str1i = str1;
  matrowm = matrix;
  matrow = matrix+1;
  gap1rowm = gap1;
  gap1row = gap1rowm+1;
  gap2rowm = gap2;
  gap2row = gap2rowm+1;
  for (i=1;i<=len1;i++) {
    str2j = str2;
    matijm = *matrow;
    matij = matijm+1;
    matimjm = *matrowm;
    matimj = matimjm+1;
    gap1ijm = *gap1row;
    gap1ij = gap1ijm+1;
    gap1imjm = *gap1rowm;
    gap1imj = gap1imjm+1;
    gap2ijm = *gap2row;
    gap2ij = gap2ijm+1;
    gap2imjm = *gap2rowm;
    gap2imj = gap2imjm+1;
    for (j=1;j<=len2;j++) {
      *matij = ((*str1i == *str2j) ? match : mismatch) +
               max3(*matimjm,*gap1imjm,*gap2imjm);
      temp2 = gapopen + *matimj;
      temp3 = gapextend + *gap1imj;
      temp4 = gapopen + *gap2imj;
      *gap1ij = max3(temp2,temp3,temp4);
      temp2 = gapopen + *matijm;
      temp3 = gapextend + *gap2ijm;
      temp4 = gapopen + *gap1ijm;
      *gap2ij = max3(temp2,temp3,temp4);
      str2j++;
      matijm++;
      matimjm++;
      matimj++;
      matij++;
      gap1ijm++;
      gap1imjm++;
      gap1imj++;
      gap1ij++;
      gap2ijm++;
      gap2imjm++;
      gap2imj++;
      gap2ij++;
    }
    str1i++;
    matrowm++;
    matrow++;
    gap1rowm++;
    gap1row++;
    gap2rowm++;
    gap2row++;
  }
/*  data->alignment->end1 = len1 - 1;
  data->alignment->end2 = len2 - 1;
  data->alignment->score = max3(matrix[len1][len2],
                                gap1[len1][len2],
                                gap2[len1][len2]);
*/
  (*findendpoint)(data);
  return data->alignment->score;
}

int align_stop_global(aligndatap data,int i,int j) {
  (void) data;
  return (i == 0 && j == 0);
}

void align_endpoint_global(aligndatap data) {
  int e1,e2;

  e1 = strlen(data->s1);
  e2 = strlen(data->s2);
  data->alignment->end1 = e1 - 1;
  data->alignment->end2 = e2 - 1;
  data->alignment->score = max3(data->matrix[e1][e2],
                                data->gap1[e1][e2],
                                data->gap2[e1][e2]);
}

int align_global(aligndatap data,
                char *str1,
                char *str2) {
  data->s1 = str1;
  data->s2 = str2;
  data->alntype = ALN_GLOBAL;
  align_init_global(data,strlen(str1),strlen(str2));
  return align_fill_nonlocal(data,&align_endpoint_global);
}

/* align_init_endsfree
 *
 * * initialize a DP matrix for global alignment, affine gaps
 * */
void align_init_endsfree(aligndatap data,int str1len,int str2len) {
  int i,len;

  data->matrix[0][0] = 0;
  data->gap1[0][0] = NEGINF;
  data->gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    data->matrix[i][0] = NEGINF;
    data->matrix[0][i] = NEGINF;
    data->gap1[i][0] = 0;
    data->gap1[0][i] = NEGINF;
    data->gap2[i][0] = NEGINF;
    data->gap2[0][i] = 0;
  }
}

int align_stop_endsfree(aligndatap data,int i,int j) {
  (void) data;
  return (i == 0 || j == 0);
}

void align_endpoint_endsfree(aligndatap data) {
  int maxscore,e1,e2,i,len1,len2,score;
  int **matrix,**gap1,**gap2;
  matrix = data->matrix;
  gap1 = data->gap1;
  gap2 = data->gap2;

  len1 = strlen(data->s1);
  len2 = strlen(data->s2);
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
  data->alignment->end1 = e1 - 1;
  data->alignment->end2 = e2 - 1;
  data->alignment->score = maxscore;
}

int align_endsfree(aligndatap data,
                   char *str1,
                   char *str2) {
  data->s1 = str1;
  data->s2 = str2;
  data->alntype = ALN_ENDSFREE;
  align_init_endsfree(data,strlen(str1),strlen(str2));
  return align_fill_nonlocal(data,&align_endpoint_endsfree);
}

/* align_init_partialfree
 *
 * * initialize a DP matrix for partially-free alignment, affine gaps
 * */
void align_init_partialfree(aligndatap data,int str1len,int str2len) {
  int i,len;

  data->matrix[0][0] = 0;
  data->gap1[0][0] = NEGINF;
  data->gap2[0][0] = NEGINF;
  len = max2(str1len,str2len);
  for (i=1;i<=len;i++) {
    data->matrix[i][0] = NEGINF;
    data->matrix[0][i] = NEGINF;
    data->gap1[i][0] = data->gapopen + ((i-1)*data->gapextend);
    data->gap1[0][i] = NEGINF;
    data->gap2[i][0] = NEGINF;
    data->gap2[0][i] = 0;
  }
}

int align_stop_partialfree(aligndatap data,int i,int j) {
  (void) data;
  (void) j;
  /* fprintf(stdout,"asp..."); fflush(stdout); */
  return i == 0;
}

void align_endpoint_partialfree(aligndatap data) {
  int maxscore,score,e1,e2,len1,len2,i;
  int **matrix,**gap1,**gap2;

  matrix = data->matrix;
  gap1 = data->gap1;
  gap2 = data->gap2;
  len1 = strlen(data->s1);
  len2 = strlen(data->s2);
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
  data->alignment->end1 = e1 - 1;
  data->alignment->end2 = e2 - 1;
  data->alignment->score = maxscore;
}

int align_partialfree(aligndatap data,
                      char *str1,
                      char *str2) {
  data->s1 = str1;
  data->s2 = str2;
  data->alntype = ALN_PARTIALFREE;
  align_init_partialfree(data,strlen(str1),strlen(str2));
  return align_fill_nonlocal(data,&align_endpoint_partialfree);
}

int initial_dir(aligndatap data,int i,int j) {
  int mat,g1,g2,dir;

  mat = data->matrix[i][j];
  g1 = data->gap1[i][j];
  g2 = data->gap2[i][j];
  if (mat >= g1 && mat >= g2) {
    dir = DIAG;
  } else if (g1 >= g2) {
    dir = UP;
  } else {
    dir = LEFT;
  }
  return dir;
}

int direction(aligndatap data,int i,int j,int prevdir) {
  int dir,mat,g1,g2;
  int match,mismatch,gapo,gape;
  char *s1,*s2;
  int val,vale,valo;

  dir = -1;
  mat = data->matrix[i][j];
  g1 = data->gap1[i][j];
  g2 = data->gap2[i][j];
  match = data->match;
  mismatch = data->mismatch;
  gapo = data->gapopen;
  gape = data->gapextend;
  s1 = data->s1;
  s2 = data->s2;
  switch (prevdir) {
    case DIAG:
      val = mat - ((s1[i-1] == s2[j-1]) ? match : mismatch);
      if (val == data->matrix[i-1][j-1]) {
        dir = DIAG;
      } else if (val == data->gap1[i-1][j-1]) {
        dir = UP;
      } else if (val == data->gap2[i-1][j-1]) {
        dir = LEFT;
      } else {
        fprintf(stderr,"SHIT 1 (%d,%d)\n",i,j);
      }
      break;
    case UP:
      vale = g1 - gape;
      valo = g1 - gapo;
      if (valo == data->matrix[i-1][j]) {
        dir = DIAG;
      } else if (vale == data->gap1[i-1][j]) {
        dir = UP;
      } else if (valo == data->gap2[i-1][j]) {
        dir = LEFT;
      } else {
        fprintf(stderr,"SHIT 2 (%d,%d)\n",i,j);
      }
      break;
    case LEFT:
      vale = g2 - gape;
      valo = g2 - gapo;
      if (valo == data->matrix[i][j-1]) {
        dir = DIAG;
      } else if (valo == data->gap1[i][j-1]) {
        dir = UP;
      } else if (vale == data->gap2[i][j-1]) {
        dir = LEFT;
      } else {
        fprintf(stderr,"SHIT 3 (%d,%d)\n",i,j);
      }
      break;
  }
  return dir;
}

int align_traceback(aligndatap data,
                    char *aln1,
                    char *aln2,
                    int bufferlen,
                    int (*stop)(aligndatap,int,int)) {
  int i,j,k,identity,dir,oi,oj;
  int alnpos,alnlen;
  alignmentp aobj;

/*
  if (stop == &align_stop_partialfree) {
    fprintf(stdout,"OK asp");
  } else if (stop == &align_stop_endsfree) {
    fprintf(stdout,"OK ase");
  } else if (stop == &align_stop_global) {
    fprintf(stdout,"OK asg");
  } else if (stop == &align_stop_local) {
    fprintf(stdout,"OK asl");
  } else {
    fprintf(stdout,"Lost value for stop: %x",stop);
  }
  fflush(stdout);
*/
    
/*   fprintf(stdout,"start traceback: str1=%s str2=%s\n",data->s1,data->s2);  */
  /* fprintf(stdout,"3.1..."); fflush(stdout); */
  aobj = data->alignment;

  identity = 0;
  i = aobj->end1+1;
  j = aobj->end2+1;
  /* fprintf(stdout," end1=%d   end2=%d\n",i,j); fflush(stdout); */
  /* fprintf(stdout,"3.2 (e1=%d, e2=%d)...",i,j); fflush(stdout); */
  alnpos = bufferlen-1;
  dir = initial_dir(data,i,j);
  while (!(*stop)(data,i,j)) {
    if (alnpos < 0) {
      fprintf(stderr,"traceback: alignment buffers too small, aborting\n");
      return -1;
    }
    oi = i; oj = j;
    switch (dir) {
      case UP:
        aln1[alnpos] = data->s1[--i];
        aln2[alnpos--] = '-';
        break;
      case DIAG:
        if (data->s1[--i] == data->s2[--j]) { identity++; }
        aln1[alnpos] = data->s1[i];
        aln2[alnpos--] = data->s2[j];
        break;
      case LEFT:
        aln1[alnpos] = '-';
        aln2[alnpos--] = data->s2[--j];
        break;
      default:
        fprintf(stderr,"dir(%d,%d) = %d\n",i,j,dir);
        exit(-1);
    }
    /* fprintf(stdout,"<%d,%d,%d",i,j,dir); fflush(stdout); */
    dir = direction(data,oi,oj,dir);
    /* fprintf(stdout,">"); fflush(stdout); */
    /* fprintf(stderr,"aln1[%d]: %c  aln2[%d]: %c\n",alnpos+1,aln1[alnpos+1],alnpos+1,aln2[alnpos+1]); */
  }
  aobj->start1 = i;
  aobj->start2 = j;
  /* fprintf(stdout,"3.3..."); fflush(stdout); */

  alnlen = bufferlen - alnpos - 1;
  for (k=0;k<alnlen;k++) {
    aln1[k] = aln1[k+alnpos+1];
    aln2[k] = aln2[k+alnpos+1];
  }
  /* fprintf(stdout,"3.4..."); fflush(stdout); */
  aln1[alnlen] = '\0';
  aln2[alnlen] = '\0';
  /* fprintf(stderr,"alnlen: %d\naln1: %s\naln2: %s\n",alnlen,aln1,aln2); */
  aobj->aln1 = aln1;
  aobj->aln2 = aln2;
  aobj->alignlen = alnlen;
  aobj->identity = identity;

  return alnlen;
}

void align_dump(aligndatap data,int anum,FILE *dest,int indent) {
  alignmentp aobj;

  aobj = data->alignment;
  fprintf(dest,"%*s%d score=%d length=%d identity=%d a1=[%d-%d] a2=[%d-%d]\n",
               indent," ",anum,aobj->score,aobj->alignlen,aobj->identity,
               aobj->start1,aobj->end1,aobj->start2,aobj->end2);
  if (aobj->aln1 != NULL) {
    fprintf(dest,"%*s%s\n%*s%s\n",
                 indent," ",aobj->aln1,indent," ",aobj->aln2);
  }
  return;
}

void align_debugdump(aligndatap data,FILE *dest,int indent,char *label) {
  int i,j;
  int s1len,s2len;

  fprintf(dest,"%*sAlignment Dump: %s\n",indent,"",label);
  fprintf(dest,"%*s   maxstr=%d match=%d mismatch=%d gapopen=%d gapextend=%d\n",
               indent,"",data->maxstr,data->match,data->mismatch,
               data->gapopen,data->gapextend);
  fprintf(dest,"%*s   s1=%s\n%*s   s2=%s\n%*smatrices:\n",
               indent,"",data->s1,indent,"",data->s2,indent,"");
  s1len = strlen(data->s1);
  for (i=0;i<=s1len;i++) {
    fprintf(dest,"%*s   %2d",indent,"",i);
    s2len = strlen(data->s2);
    for (j=0;j<=s2len;j++) {
      fprintf(dest," [%3d,%3d,%3d]",data->matrix[i][j],data->gap1[i][j],
                   data->gap2[i][j]);
    }
    fprintf(dest,"\n");
  }
  align_dump(data,0,dest,indent+3);
  return;
}

/******************************************************************************/

/* Python interface stuff goes here */

static aligndatap cur;
static char *str1;
static char *str2;
static char *aln1stat;
static char *aln2stat;
typedef int (*STOPPROC)(aligndatap,int,int);
static STOPPROC stopproc[ALN_TYPES];

int calignInit(int maxstr,int match, int mismatch, int gapopen, int gapextend) {
  /* fprintf(stdout,"0... "); fflush(stdout); */
  cur = align_init(maxstr,match,mismatch,gapopen,gapextend);
  /* fprintf(stdout,"0.1... "); fflush(stdout); */
  if (cur == NULL) {
    return -1;
  } else {
    aln1stat = (char *) malloc(maxstr*2+1);
    aln2stat = (char *) malloc(maxstr*2+1);
    str1 = (char *) malloc(maxstr+1);
    str2 = (char *) malloc(maxstr+1);
    if (aln1stat == NULL || aln2stat == NULL ||
        str1 == NULL || str2 == NULL) {
      return -1;
    } else {
      stopproc[ALN_GLOBAL] = &align_stop_global;
      stopproc[ALN_LOCAL] = &align_stop_local;
      stopproc[ALN_ENDSFREE] = &align_stop_endsfree;
      stopproc[ALN_PARTIALFREE] = &align_stop_partialfree;
      return 0;
    }
  }
}

int calignEndsFree(char *s1,char *s2) {
  int score;

  strcpy(str1,s1);
  strcpy(str2,s2);
  score = align_endsfree(cur,str1,str2);
  return score;
}

int calignPartialFree(char *s1,char *s2) {
  int score;

  strcpy(str1,s1);
  strcpy(str2,s2);
  score = align_partialfree(cur,str1,str2);
  return score;
}

void calignTraceback(char *res1,char *res2) {
  align_traceback(cur,res1,res2,cur->maxstr*2,stopproc[cur->alntype]);
}

int calignLength() {
  return cur->alignment->alignlen;
}

int calignPosition2start() {
  return cur->alignment->start2;
}

int calignPosition2end() {
  return cur->alignment->end2;
}

int calignIdentity() {
  return cur->alignment->identity;
}

