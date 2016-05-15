#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "redblackSN.h"
#include "util.h"

void loadNames(char *fn,rbsnTree t) {
  FILE *fd;
  int count;
  char *s;
  char buffer[100];
  char *thing;

  count = 0;
  fd = fopen(fn,"r");
  while (!feof(fd)) {
    thing = fgets(buffer,100,fd);
    if (feof(fd)) {
      break;
    }
    if (thing == NULL) {
      fprintf(stderr,"a ERROR: %d -- %s\n",errno,strerror(errno));
    }
    stripWhiteSpace(buffer);
    s = strdup(buffer);
    rbsn_insert(t,s,1);
    count += 1;
    if (count % 100000 == 0) {
      fprintf(stderr,"%08d\r",count);
    }
  }
  fprintf(stderr,"loaded %d names\n",count);
  fclose(fd);
  return;
}

int main(int argc,char **argv) {
  rbsnTree tree;
  FILE *infd,*outfd;
  char buffer[100];
  int keep,total,i,slen,offset,px;
  char *thing;

  tree = rbsn_new();
  loadNames(argv[1],tree);
  infd = fopen(argv[2],"r");
  outfd = fopen(argv[3],"w");
  total = 0;
  keep = 0;
  
  while (!feof(infd)) {
    thing = fgets(buffer,100,infd);
    if (feof(infd)) {
      break;
    }
    if (thing == NULL) {
      fprintf(stderr,"b ERROR: %d -- %s\n",errno,strerror(errno));
    }
    stripWhiteSpace(buffer);
    slen = strlen(buffer);
    offset = slen - 35;
    if (offset < 1) {
      offset = 1;
    }
    if (rbsn_search(tree,buffer+offset) != 1) {
      fprintf(outfd,"%s\n",buffer);
      for (i=0;i<3;i++) {
        thing = fgets(buffer,100,infd);
        if (thing == NULL) {
          fprintf(stderr,"x ERROR: %d -- %s\n",errno,strerror(errno));
        }
        px = fprintf(outfd,"%s",buffer);
        if (px == 0 || errno != 0) {
          fprintf(stderr,"c ERROR: %d -- %s\n",errno,strerror(errno));
        }
      }
      keep++;
    } else {
      for (i=0;i<3;i++) {
        thing = fgets(buffer,100,infd);
        if (thing == NULL) {
          fprintf(stderr,"d ERROR: %d -- %s\n",errno,strerror(errno));
        }
      }
    }
    total++;
    if (total % 100000 == 0) {
      fprintf(stderr,"%08d\r",total);
    }
  }
  fclose(infd);
  fclose(outfd);
  fprintf(stderr,"kept %d of %d\n",keep,total);
  return(0);
}
