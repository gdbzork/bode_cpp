#include <iostream>
#include <fstream>
#include <string.h>

#include "bode/writer.h"
#include "bode/sequence.h"
#include "bode/fastaReader.h"
#include "bode/fasta.h"

bode::FastaReader::FastaReader(std::string const &f) {
  _filename = f;
  _fd = new std::ifstream(_filename.c_str(),std::ifstream::in);
  _fa = new bode::Fasta();
}


bode::FastaReader::~FastaReader(void) {
  close();
}

void bode::FastaReader::close(void) {
  _fd->close();
}

bool bode::FastaReader::eof(void) {
  return _fd->eof();
}

void *bode::FastaReader::header(void) {
  return NULL;
}

bode::Writer *bode::FastaReader::getWriter(void) {
  return new bode::Writer();
}

char *bode::FastaReader::splitHead(char *s) {
  char *ind, *rc = NULL;
  ind = strpbrk(s," \t\n\r");
  if (ind != NULL) {
    rc = stripseq(ind);
    *ind = '\0';
  }
  return rc;
}

char *bode::FastaReader::stripseq(char *str) {
  int slen,start,end;

  slen = strlen(str);
  start = 0;
  while (start < slen && isspace(str[start])) {
    start++;
  }
  end = slen-1;
  while (end > start && isspace(str[end])) {
    end--;
  }
  str[end+1] = '\0';
  if (start > 0) {
    memmove(str,str+start,end-start+2);
  }
  return str;
}


bode::Sequence *bode::FastaReader::nextS(void) {
  char buffer[_maxlen];
  char *r,*d;

  _fd->getline(buffer,_maxlen);
  if (_fd->eof()) {
    return NULL;
  }
  if (buffer[0] == '>') {
    buffer[0] = ' ';
  }
  r = stripseq(buffer);
  d = splitHead(r);
  _fa->setName(std::string(r));
  if (d != NULL) {
    _fa->setDescr(std::string(d));
  } else {
    _fa->setDescr("");
  }
  _fd->getline(buffer,_maxlen);
  r = stripseq(buffer);
  _fa->setSeq(buffer);
  return _fa;
}
