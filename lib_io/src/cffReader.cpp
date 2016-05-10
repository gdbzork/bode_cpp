#include <string>
#include <fstream>
#include <zlib.h>
#include <iostream>
#include <stdlib.h>

#include "bode/interval.h"
#include "bode/bed.h"
#include "bode/cffReader.h"

/*
void trimLF(char *str) {
  int slen = strlen(str);
  if (slen > 0 && str[slen-1] == '\n') {
    str[slen-1] = '\0';
  }
}

int splits(char *str,char **dest,int max) {
  int index = 0;
  char *start = str;
  if (str == NULL) {
    return 0;
  }
  trimLF(str);
  while (*str != '\0' && index < max) {
    if (isspace(*str)) {
      dest[index++] = start;
      start = str+1;
      if (index < max) {
        *str = '\0';
      }
    }
    str++;
  }
  if (index < max) {
    dest[index++] = start;
  }
  return index;
}
*/

bode::CffReader::CffReader(std::string const &filename):BedReader(filename) {
  /* nothing to see here... */
}

bode::CffReader::~CffReader(void) {
  delete[] _buffer;
  delete _bseq;
  close();
}

bode::CffReader *bode::CffReader::open(std::string const &filename) {
  CffReader *br = new CffReader(filename);
  return br;
}

bode::Interval *bode::CffReader::nextI(void) {
  char *fields[12];
  int count;
  bode::Interval *rv = NULL;

  if (_buffer[0] == '\0') {
    _eof = true;
    _bseq->setUnmapped();
    return rv;
  }
  count = splits(_buffer,fields,12);
  if (count >= 4) {
    int pos = atoi(fields[1]);
    _bseq->update(fields[0],pos,pos+1,fields[2],atof(fields[3]),'+');
    rv = _bseq;
  } else {
    std::cerr << "we shouldn't be here..." << std::endl;
    /* an error... not sure  what to do now... */
    std::cerr << "found " << count << " fields, expecting 4, in this line:" << std::endl;
    std::cerr << _buffer;
  }
  if (gzeof(_fd)) {
    _buffer[0] = '\0';
  } else {
    char *rc;
    rc = gzgets(_fd,_buffer,maxLine);
    if (rc == NULL) {
      _eof = true;
    }
  }
  return rv;
}
