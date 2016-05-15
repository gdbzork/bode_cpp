#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc,char **argv) {
  long x;
  short *y;

  x = strtol(argv[1],NULL,10);
  fprintf(stderr,"x=%ld\n",x);
  y = calloc(x,sizeof(short));
  if (y != NULL) {
    fprintf(stderr,"ok\n");
  } else {
    fprintf(stderr,"failed\n");
  }
  fprintf(stderr,"long max=%ld (%ld)\n",LONG_MAX,sizeof(long));
  fprintf(stderr,"shrt max=%d (%ld)\n",SHRT_MAX,sizeof(short));
  fprintf(stderr,"int  max=%d (%ld)\n",INT_MAX,sizeof(int));
  return 0;
}
