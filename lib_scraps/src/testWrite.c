#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>
#include <time.h>
#include <stdio.h>

#define BLOCKSIZE 1024
#define WRITES_PER_LOOP 1048576
/* #define WRITES_PER_LOOP 1024 */

int main(int argc,char **argv) {
  char fn[100];
  char buffer[BLOCKSIZE];
  int blocks,i,fd,tag,j;
  struct tms tbuf;
  time_t start,end;
  long ut,st,dur;
  char hostname[100];
  clock_t alpha,omega;

  tag = atoi(argv[1]);
  sprintf(fn,"bytes%04d.txt",tag);
  blocks = atoi(argv[2]);

  for (i=0;i<BLOCKSIZE;i++) {
    buffer[i] = '0';
  }

  start = time(NULL);
  fd = open(fn,O_WRONLY|O_CREAT,0644);
  alpha = times(&tbuf);
  for (i=0;i<blocks;i++) {
    for (j=0;j<WRITES_PER_LOOP;j++) {
      write(fd,buffer,BLOCKSIZE);
    }
    fsync(fd);
    lseek(fd,0,SEEK_SET);
    omega = times(&tbuf);
    fprintf(stderr,"%ld\n",(long)(omega-alpha));
    alpha = omega;
  }
  fsync(fd);
  close(fd);
  end = time(NULL);

  times(&tbuf);
  ut = (long) tbuf.tms_utime;
  st = (long) tbuf.tms_stime;
  dur = (long) (end - start);
  gethostname(hostname,100);
  fprintf(stderr,"\tElapsed\tUTime\tSTime\tTag\tBlocks\tHost\tStart\n");
  fprintf(stderr,"GDB\t%ld\t%ld\t%ld\t%d\t%d\t%s\t%s",dur,ut,st,tag,blocks,hostname,ctime(&start));
  return(0);
}
