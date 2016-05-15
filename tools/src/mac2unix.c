#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUFSIZE 4096

char buffer[BUFSIZE];

/******************************************************************************/

void convert(char *buf,int len) {
  int i;
  for (i=0;i<len;i++) {
    if (buf[i] == '\r') {
      buf[i] = '\n';
    }
  }
}

/******************************************************************************/

int main(int argc,char **argv) {
  int inFD,outFD,bytes;
  char last;

  if (argc == 1) { /* stdin and stdout */
    inFD = 0;
    outFD = 1;
  } else if (argc == 2) { /* input from file, out to stdout */
    inFD = open(argv[1],O_RDONLY);
    outFD = 1;
  } else if (argc == 3) { /* in from file, out to file */
    inFD = open(argv[1],O_RDONLY);
    outFD = open(argv[2],O_WRONLY|O_CREAT|O_EXCL,0644);
    if (outFD < 0) {
      perror("Failed to open output file");
      return -1;
    }
  }

  while ((bytes = read(inFD,buffer,BUFSIZE)) > 0) {
    convert(buffer,bytes);
    write(outFD,buffer,bytes);
    last = buffer[bytes-1];
  }
  if (last != '\n') {
    write(outFD,"\n",1); /* trailing newline */
  }
  if (bytes < 0) {
    perror("FAILED: ");
    return bytes;
  }
  return 0;
}
