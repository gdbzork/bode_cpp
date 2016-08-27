#include <map>
#include <fstream>
#include <string.h>

#include "bode/lineType.h"

std::string bode::TextTypeChecker::text(bode::TextType tt) {
  switch (tt) {
    case UNIX: return "unix";
    case DOS: return "dos";
    case MAC: return "mac";
    case MIXED: return "mixed";
    case UNKNOWN: return "unknown";
    case FAIL: return "failed to detect";
  }
}

bode::TextType bode::TextTypeChecker::sget(std::string fn,unsigned limit) {
  char buffer[limit];
  unsigned count;
  char fn_c[1024];
  std::ifstream fd;

  strcpy(fn_c,fn.c_str());
  fd.open(fn_c);
  if (fd.bad()) {
    return FAIL;
  }
  fd.read(buffer,limit);
  if (fd.bad()) {
    return FAIL;
  }
  count = fd.gcount();
  return get(buffer,count);
}

bode::TextType bode::TextTypeChecker::fget(std::istream &fd,unsigned limit) {
  char buffer[limit];
  unsigned count;

  fd.read(buffer,limit);
  if (fd.bad()) {
    return FAIL;
  }
  count = fd.gcount();
  return get(buffer,count);
}

bode::TextType bode::TextTypeChecker::get(char *buffer,unsigned limit) {
  std::map<bode::TextType,int> counts;
  bool skip;

  counts[UNIX] = 0;
  counts[DOS] = 0;
  counts[MAC] = 0;

  skip = false;
  for (int i=0;i<limit-1;i++) {
    if (skip) {
      skip = false;
    } else if (buffer[i] == '\r') {
      if (buffer[i+1] == '\n') {
        counts[DOS]++;
        skip = true;
      } else {
        counts[MAC]++;
      }

    } else if (buffer[i] == '\n') {
      counts[UNIX]++;
    }
  }

  bode::TextType rv;
  if        (counts[UNIX] > 0 && counts[DOS] == 0 && counts[MAC] == 0) {
    rv = UNIX;
  } else if (counts[UNIX] == 0 && counts[DOS] > 0 && counts[MAC] == 0) {
    rv = DOS;
  } else if (counts[UNIX] == 0 && counts[DOS] == 0 && counts[MAC] > 0) {
    rv = MAC;
  } else if (counts[UNIX] == 0 && counts[DOS] == 0 && counts[MAC] == 0) {
    rv = UNKNOWN;
  } else {
    rv = MIXED;
  }
  return rv;
}
