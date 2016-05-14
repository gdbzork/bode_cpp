/*
 * Fast linker screening, version 2.0 by Gord Brown.
 *
 * This code is in the public domain.  Do whatever you like with it, but
 * please do not remove my name.  Unless a bug in the code causes a terrible
 * tragedy in which thousands perish.  Then please do remove my name.
 *
 * No guarantees, expressed or implied.
 */
#include <string.h>
#include "bode/screener.h"
#include "bode/alignerEndsFree.h"

namespace bode {

Screener::Screener(char *fp,char *tp,int ms,float id,int ml,int tl) {
  aligner = new AlignerEndsFree(ms,match,mismatch,gapopen,gapextend);
  readCount = 0;
  cleanCount = 0;
  fivepCount = 0;
  threepCount = 0;
  endMatchCount = 0;
  wholeMatchCount = 0;
  maxstr = ms;
  fiveplinker = strdup(fp);
  threeplinker = strdup(tp);
  identity = id;
  matchlen = ml;
  taillen = tl;
  
}

Screener::~Screener() {
  delete aligner;
  delete fiveplinker;
  delete threeplinker;
}

/*
int Screener::alignLinker(char *seq,char *linker,int *len, int *id,int *start,int *end) {
  int score;
  score = calignEndsFree(linker,seq);
  calignTraceback(tb1,tb2);
  *len = strlen(tb1);
  *id = calignIdentity();
  *start = calignPosition2start();
  *end = calignPosition2end();
  return score;
}

int Screener::foundLinker(char *seq,char *link1,char *link2,int *st,int *en,float *pId,
                int allowTail) {
  int score,alnlen,id,start,end,coverage;
  int score1,alnlen1,id1,start1,end1,coverage1;
  int score2,alnlen2,id2,start2,end2,coverage2;
  int found,found1,found2;
  float percentId,percentId1,percentId2;
  char *link;

  found1 = found2 = found = 0;

  score1 = alignLinker(seq,link1,&alnlen1,&id1,&start1,&end1);
  score2 = alignLinker(seq,link2,&alnlen2,&id2,&start2,&end2);
  percentId1 = ((float) id1) / ((float) alnlen1);
  percentId2 = ((float) id2) / ((float) alnlen2);
  coverage1 = end1-start1+1;
  coverage2 = end2-start2+1;

  if (percentId1 >= identity &&
      (coverage1 >= matchLen || (allowTail && strlen(seq)-end1 <= 2 &&
                                coverage1 >= minLengthToCallLinker))) {
    found1 = 1;
  }
  if (percentId2 >= identity &&
      (coverage2 >= matchLen || (allowTail && strlen(seq)-end2 <= 2 &&
                                coverage2 >= minLengthToCallLinker))) {
    found2 = 1;
  }

  if (verbose > 1) {
    if (found1) {
      fprintf(stderr,"found1: %s score=%d id=%d alnlen=%d\n",seq,score1,id1,alnlen1);
      fprintf(stderr,"        %*s%s\n",start1,"",link1);
    } 
    if (found2) {
      fprintf(stderr,"found2: %s score=%d id=%d alnlen=%d\n",seq,score2,id2,alnlen2);
      fprintf(stderr,"        %*s%s\n",start2,"",link2);
    } 
  }
  if (found1 && (!found2 || score1 > score2)) {
    found = 1;
    score = score1;
    alnlen = alnlen1;
    id = id1;
    start = start1;
    end = end1;
    coverage = coverage1;
    percentId = percentId1;
    link = link1;
  } else if ((!found1 || score2 >= score1) && found2) {
    found = 1;
    score = score2;
    alnlen = alnlen2;
    id = id2;
    start = start2;
    end = end2;
    coverage = coverage2;
    percentId = percentId2;
    link = link2;
  } else if (!found1 && !found2) {
    found = 0;
  } else {
    fprintf(stderr,"ERROR: ambiguous case in foundLinker\n");
    fprintf(stderr,"  seq:  %s\n",seq);
    fprintf(stderr,"  lnk1: %s score1=%d id1=%d start1=%d end1=%d alnlen1=%d\n",
            link1,score1,id1,start1,end1,alnlen1);
    fprintf(stderr,"  lnk2: %s score2=%d id2=%d start2=%d end2=%d alnlen2=%d\n",
            link2,score2,id2,start2,end2,alnlen2);
    found = 0;
  }
  if (found) {
    *pId = percentId;
    *st = start;
    *en = end;
    if (verbose) {
      fprintf(stderr,"final: %s score=%d id=%d alnlen=%d\n",seq,score,id,alnlen);
      fprintf(stderr,"       %*s%s\n",start,"",link);
    }
  } else {
    if (verbose) fprintf(stderr,"no match %s\n",seq);
  }
  return found;
}

int Screener::isLinker(char *str,int &start,int &length) {
  int start,end;
  float pId;
  int fstart,fend;

  if (foundLinker(seq,link1->data,link2->data,&start,&end,&pId,1)) {
    if (start < minLengthToKeep) {
      sprintf(reason," start=%d  length=%d  id=%f",start,end-start+1,pId);
      return 1; // it's all linker 
    } else {
      if (wholeRead) {
        *position = -1;
        return 0;
      }
      strncpy(scrap,seq,start);
      scrap[start] = 0;
      if (verbose > 2) {
        fprintf(stderr,"second alignment (start=%d):\n       %s\n       %s\n",
                start,seq,scrap);
      }
      if (foundLinker(scrap,link1->data,link2->data,&fstart,&fend,&pId,0)) {
        if (fstart < minLengthToKeep) {
          sprintf(reason," start=%d  length=%d  id=%f (double linker)",fstart,fend-fstart+1,pId);
          return 1; // it's all linker
        } else {
          *position = fstart;
          return 0;
        }
      } else {
        *position = start;
        return 0;
      }
    }
  } else {
    *position = -1;
    return 0;
  }
}

void Screener::stats(int *tot,int *clean,int *fp,int *tp,int *endm) {
}
*/

}
