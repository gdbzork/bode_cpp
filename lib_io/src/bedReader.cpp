#include <string>
#include <fstream>
#include <zlib.h>
#include <iostream>
#include <stdlib.h>

#include "bode/interval.h"
#include "bode/bed.h"
#include "bode/bedReader.h"

void bode::BedReader::trimLF(char *str) {
  int slen = strlen(str);
  if (slen > 0 && str[slen-1] == '\n') {
    str[slen-1] = '\0';
  }
}

int bode::BedReader::splits(char *str,char **dest,int max) {
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

bode::BedReader::BedReader(std::string const &filename) {
  char *res;
/*  _fd = new std::ifstream(filename.c_str()); */
  _fd = gzopen(filename.c_str(),"r");
  _buffer = new char[maxLine];

/*  _fd->getline(_buffer,maxLine); */
  res = gzgets(_fd,_buffer,maxLine);
  if (strncmp(_buffer,"track",5) == 0) {
    res = gzgets(_fd,_buffer,maxLine);
/*    _fd->getline(_buffer,maxLine); */
  }
  if (res == NULL) {
    _eof = true;
  }
  _bseq = new Bed();
}

bode::BedReader::~BedReader(void) {
  delete[] _buffer;
  delete _bseq;
  close();
}

bode::BedReader *bode::BedReader::open(std::string const &filename) {
  BedReader *br = new BedReader(filename);
  return br;
}

void bode::BedReader::close(void) {
/*  if (_fd != NULL && _fd->is_open()) { */
  if (_fd != NULL) {
    gzclose(_fd);
/*    delete _fd; */
    _fd = NULL;
  }
}

bode::Interval *bode::BedReader::nextI(void) {
  char *fields[12];
  int count;
  bode::Interval *rv = NULL;

  if (_buffer[0] == '\0') {
    _eof = true;
    _bseq->setUnmapped();
    return rv;
  }
  count = splits(_buffer,fields,12);
  if (count < 6) {
    _bseq->update(fields[0],atoi(fields[1]),atoi(fields[2]));
    rv = _bseq;
  } else {
    if (count > 4) {
      _bseq->update(fields[0],atoi(fields[1]),atoi(fields[2]),fields[3],atof(fields[4]),fields[5][0]);
    } else {
      _bseq->update(fields[0],atoi(fields[1]),atoi(fields[2]),fields[3],atof(fields[4]),'.');
    }
    rv = _bseq;
  }
/*  if (_fd->eof()) { */
  if (gzeof(_fd)) {
    _buffer[0] = '\0';
  } else {
/*    _fd->getline(_buffer,maxLine); */
    char *rc;
    rc = gzgets(_fd,_buffer,maxLine);
    if (rc == Z_NULL) {
      _eof = true;
    }
  }
/*  fprintf(stderr,"%s %s %s\n",fields[0],fields[1],fields[2]); */
  return rv;
}
