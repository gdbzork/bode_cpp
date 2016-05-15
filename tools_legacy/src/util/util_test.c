#include <stdio.h>
#include <string.h>

#include "util.h"

char *input[] = {"abcdefg",
                 " abcdefg",
                 "abcdefg   ",
                 "  abc   abcdef   ",
                 "    ",
                 "",
                 NULL};

char *output[] = {"abcdefg",
                  "abcdefg",
                  "abcdefg",
                  "abc   abcdef",
                  "",
                  "",
                  NULL};

char *revcomp_input[] =  {"", "A", "a", "AC", "ACCCGATTGNYQZT",
                         NULL};

char *revcomp_output[] = {"", "T", "t", "GT", "AZQRNCAATCGGGT",
                          NULL};

char *splitIn[] = {"now is the winter of our discontent",
                   "\t\tnow is the\nwinter of our discontent   ",
                   " one two three four five six seven eight nine ten eleven twelve thirteen",
                   "  ",
                   NULL};

char *splitOut[][10] = {{"now","is","the","winter","of","our","discontent"},
                        {"now","is","the","winter","of","our","discontent"},
                        {"one","two","three","four","five","six","seven","eight","nine","ten eleven twelve thirteen"},
                        {""}};
int splitCount[] = {7,7,10,0};

char *splitsOut[][20] = {{"now","is","the","winter","of","our","discontent"},
                         {"","","now","is","the","winter","of","our","discontent","","",""},
                         {"","one","two","three","four","five","six","seven","eight","nine","ten","eleven twelve thirteen"},
                        {"","",""}};
int splitsCount[] = {7,12,12,3};

void splittest(int *total,int *failed) {
  char *s;
  int i,j,k;
  char *r[10];

  i = 0;
  while (splitIn[i] != NULL) {
    *total += 1;
    j = split(strdup(splitIn[i]),r,10);
    if (j != splitCount[i]) {
      fprintf(stderr,"failed split test %d: in='%s' count='%d' correct='%d'\n",i,splitIn[i],j,splitCount[i]);
      *failed += 1;
      i++;
      continue;
    }
    for (k=0;k<j;k++) {
      if (strcmp(r[k],splitOut[i][k]) != 0) {
        fprintf(stderr,"failed split test %d/%d: out='%s' correct='%s'\n",i,k,r[k],splitOut[i][k]);
        *failed += 1;
        break;
      }
    }
    i++;
  }
}

void splitstest(int *total,int *failed) {
  char *s;
  int i,j,k;
  char *r[10];

  i = 0;
  while (splitIn[i] != NULL) {
    *total += 1;
    j = splits(strdup(splitIn[i]),r,12);
    if (j != splitsCount[i]) {
      fprintf(stderr,"failed splits test %d: in='%s' count='%d' correct='%d'\n",i,splitIn[i],j,splitsCount[i]);
      *failed += 1;
      i++;
      continue;
    }
    for (k=0;k<j;k++) {
      if (strcmp(r[k],splitsOut[i][k]) != 0) {
        fprintf(stderr,"failed splits test %d/%d: out='%s' correct='%s'\n",i,k,r[k],splitsOut[i][k]);
        *failed += 1;
        break;
      }
    }
    i++;
  }
}

int main(int argc,char **argv) {
  int i,failed,count;
  char *in,*out;

  i = 0;
  count = 0;
  failed = 0;
  while (input[i] != NULL) {
    in = strdup(input[i]);
    out = stripWhiteSpace(in);
    if (out == NULL || strcmp(out,output[i]) != 0) {
      fprintf(stderr,"failed trim test %d: in='%s' out='%s' correct='%s'\n",i,input[i],out,output[i]);
      failed++;
    }
    i++;
    count++;
  }
  i = 0;
  while (revcomp_input[i] != NULL) {
    in = strdup(revcomp_input[i]);
    reverseComplementInPlace(in);
    if (strcmp(in,revcomp_output[i]) != 0) {
      fprintf(stderr,"failed revcomp test %d: in='%s' out='%s' correct='%s'\n",count,revcomp_input[i],in,revcomp_output[i]);
      failed++;
    }
    i++;
    count++;
  }
  splittest(&count,&failed);
  splitstest(&count,&failed);
  fprintf(stderr,"failed %d/%d tests\n",failed,count);
  return failed;
}
