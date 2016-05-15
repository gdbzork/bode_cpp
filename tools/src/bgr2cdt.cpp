#include <stdio.h>
#include <map>
#include <string>
#include <vector>

typedef std::map<std::string,int> chromLengthMap;
typedef std::map<std::string,int>::iterator chromLengthMapIt;
typedef std::map<std::string,int *> chromMap;
typedef std::map<std::string,int *>::iterator chromMapIt;

/******************************************************************************/

bool loadChromSizes(std::string &fn,chromLengthMap &dict) {
  FILE *fd;
  char buffer[1024];
  int flds,chrlen;
  char chrname[128];
  char *rc;

  fd = fopen(fn.c_str(),"r");
  while (!feof(fd)) {
    rc = fgets(buffer,1024,fd);
    if (rc) {
      flds = sscanf(buffer,"%s %d",chrname,&chrlen);
      if (flds == 2) {
        std::string chr(chrname);
        dict[chr] = chrlen;
        fprintf(stderr,"%9d  %s\n",chrlen,chrname);
      } else {
        fprintf(stderr,"failed to parse: %s",buffer);
      }
    }
  }
  fclose(fd);
  return true;
}

bool allocateRegions(chromLengthMap &lengths,chromMap &bgr) {
  chromLengthMapIt it;

  for (it=lengths.begin(); it!=lengths.end(); it++) {
    bgr[it->first] = new int[it->second];
  }
  return true;
}

bool loadBedgraph(std::string &fn,chromMap &bgr) {
  FILE *fd;
  char buffer[1024];
  char chrom[256];
  char *rc;
  int left,right,height;
  std::string pchrom("zork");

  fd = fopen(fn.c_str(),"r");
  fgets(buffer,1024,fd); /* get rid of header line */
  while (!feof(fd)) {
    rc = fgets(buffer,1024,fd);
    if (rc) {
      int flds = sscanf(buffer,"%s %d %d %d",chrom,&left,&right,&height);
      if (flds == 4) {
        std::string chr(chrom);
        if (chr != pchrom) {
          fprintf(stderr,"loading %s\n",chrom);
          pchrom = chr;
        }
        for (int i=left;i<right;i++) {
          bgr[chr][i] = height;
        }
      } else {
        fprintf(stderr,"failed to parse: %s",buffer);
      }
    }
  }
  fclose(fd);
  return true;
}

/******************************************************************************/

/* arg 1: chrom sizes
 * arg 2: bedgraph
 * arg 3: cff
 * arg 4: output filename
 */
int main(int argc,char **argv) {
  std::string sizesFN(argv[1]);
  std::string bgrFN(argv[2]);
  chromLengthMap chromSizes;
  chromMap bgr;

  if (loadChromSizes(sizesFN,chromSizes)) {
    fprintf(stderr,"okay!\n");
  } else {
    fprintf(stderr,"oops...\n");
  }

  fprintf(stderr,"allocating memory...\n");
  allocateRegions(chromSizes,bgr);

  fprintf(stderr,"loading bgr...\n");
  loadBedgraph(bgrFN,bgr);
}
