/*
 * calign.h
 *
 * simple, fast alignment routines for short-ish strings (uses O(nm) space)
 *
 */
#ifndef ALIGN_H
#define ALIGN_H

#define ALN_TYPES 4
#define ALN_GLOBAL 0
#define ALN_LOCAL 1
#define ALN_ENDSFREE 2
#define ALN_PARTIALFREE 3

/*
 * struct alignment
 *
 * * store information about an alignment
 * */
typedef struct alignment {
  char *aln1;    /* substring of first sequence, with gaps as applicable */
  char *aln2;    /* substring of second sequence, with gaps as applicable */
  int start1;    /* beginning of alignment in 1st sequence */
  int start2;    /* beginning of alignment in 2nd sequence */
  int end1;      /* end of alignment in 1st sequence */
  int end2;      /* end of alignment in 2nd sequence */
  int alignlen;  /* total length of aligned region */
  int identity;  /* number of identical positions in aligned region */
  int score;     /* score of alignment */
} *alignmentp;


/*
 * struct aligndata
 *
 * * store information about pairs of strings, and alignments on them
 * */
typedef struct aligndata {

  int alntype;   /* global, local, ends-free, partial-free */
  int maxstr;    /* longest allowable string */

  int match;     /* match score */
  int mismatch;  /* mismatch score */
  int gapopen;   /* gap-open score */
  int gapextend; /* gap-extend score */

  char *s1;      /* string 1 ("top" string) */
  char *s2;      /* string 2 ("side" string) */

  int **matrix;  /* DP matrix */
  int **gap1;    /* gap (side) matrix */
  int **gap2;    /* gap (top) matrix */

  alignmentp alignment; /* pointer to alignment */

} *aligndatap;


aligndatap align_init(int maxstr,
                      int match,
                      int mismatch,
                      int gapopen,
                      int gapextend);

int align_local(aligndatap data,
                char *str1,
                char *str2);

int align_global(aligndatap data,
                 char *str1,
                 char *str2);

int align_endsfree(aligndatap data,
                   char *str1,
                   char *str2);

int align_traceback(aligndatap data,
                    char *aln1,
                    char *aln2,
                    int bufferlen,
                    int (*stop)(aligndatap,int,int));

void align_dump(aligndatap data,int anum,FILE *dest,int indent);

void align_debugdump(aligndatap data,FILE *dest,int indent,char *label);

int calignInit(int maxstr,int match, int mismatch, int gapopen, int gapextend);
int calignEndsFree(char *s1,char *s2);
int calignPartialFree(char *s1,char *s2);
void calignTraceback(char *res1,char *res2);
int calignLength();
int calignPosition2start();
int calignPosition2end();
int calignPosition1start();
int calignPosition1end();
int calignIdentity();

#endif /* ALIGN_H */
