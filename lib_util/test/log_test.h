#include <cxxtest/TestSuite.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex.h>

#include "bode/bodeException.h"
#include "bode/log.h"

class LogTest: public CxxTest::TestSuite {
  public:

    void xtestSanity(void) {
      bode::Log *log;
      char msg[100];

      sprintf(msg,"%s","this is not a %%d test.");
      log = new bode::Log(bode::DEBUG,"ZORK");
      log->addDestination(new bode::Log2Stderr());
      log->warning(msg,9);

      delete log;
    }

    void xtestLog2FileClass(void) {
      bode::Log2File *l2f;
      char fn[128];
      FILE *fd;
      char buffer[1024];
      char err_buffer[1024];
      int mat;
      regex_t rt;
      regmatch_t *pm=NULL;

      sprintf(fn,"testfileL2F_%d.txt",getpid());
      l2f = new bode::Log2File(fn,bode::DEBUG);
      l2f->write(bode::DEBUG,(char *)"this is a l2f test");
      delete l2f;
      fd = fopen(fn,"r");
      fgets(buffer,1024,fd);
      fclose(fd);
      fprintf(stderr,"--%s--\n",buffer);
      mat = regcomp(&rt,"^this is a l2f test\\s*$",REG_NOSUB|REG_EXTENDED);
      if (mat != 0) {
        regerror(mat,&rt,err_buffer,1024);
        fprintf(stderr,"%s",err_buffer);
        TS_FAIL(err_buffer);
      }
      mat = regexec(&rt,buffer,0,pm,0);
      TS_ASSERT_EQUALS(mat,0);
/*      unlink(fn); */
    }

    void testSyslog(void) {
      bode::Log *log;
      char *msg;

      msg = (char *) "this is a %%d test.";
      log = new bode::Log(bode::DEBUG,"ZORK");
      log->addDestination(new bode::Log2Syslog());
      log->warning(msg,9);

      delete log;
    }

    void xtestLog2File(void) {
      bode::Log *log;
      pid_t pid;
      char fn[128];
      char buffer[1024];
      char err_buffer[1024];
      FILE *fd;
      regex_t rt;
      regmatch_t *pm=NULL;
      int mat;
      
      pid = getpid();
      sprintf(fn,"testfile_%d.txt",pid);
      log = new bode::Log(bode::DEBUG,"ZORK");
      log->addDestination(new bode::Log2File(fn));
      log->warning((char *)"this is not a %d test...",9);
      fd = fopen(fn,"r");
      fgets(buffer,1024,fd);
      mat = regcomp(&rt,"\\[[0-9]{4}-[0-9]{2}-[0-9]{2}\\s+[0-9]{2}:[0-9]{2}:[0-9]{2}\\]\\s+ZORK\\s+WARNING\\s+this\\s+is\\s+not\\s+a\\s+9\\s+test\\.\\.\\.\\s*",REG_NOSUB|REG_EXTENDED);
      if (mat != 0) {
        regerror(mat,&rt,err_buffer,1024);
        fprintf(stderr,"%s",err_buffer);
        TS_FAIL(err_buffer);
      }
      mat = regexec(&rt,buffer,0,pm,0);
      TS_ASSERT_EQUALS(mat,0);
/*      unlink(fn); */
      delete log;
    }

    void testFatal(void) {
      bode::Log *log;
      
      log = new bode::Log(bode::DEBUG,"ZORK");
      TS_ASSERT_THROWS(log->fatal((char *)"testing %s...","thing"),bode::LogError);

      delete log;
    }

};
