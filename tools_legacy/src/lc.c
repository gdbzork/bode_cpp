#include <stdio.h>

#define BUFSIZE 4096

int main(int argc, char **argv) {
  FILE *fd;
  int c,i,j,r;
  char buffer[BUFSIZE];

  for (i=1;i<argc;i++) {
    c = 0;
    fd = fopen(argv[i],"r");
    while ((r = fread(buffer,1,BUFSIZE,fd)) > 0) {
      for (j=0;j<r;j++) {
        if (buffer[j] == '\n') {
          c++;
        }
      }
    }
    fclose(fd);
    printf("%d %s\n",c,argv[i]);
  }
  return(0);
}
