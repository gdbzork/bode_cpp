#include <fstream>
#include <string.h>
#include "bode/sequence.h"
#include "bode/sequenceReader.h"
#include "bode/fastq.h"
#include "bode/fastqReader.h"
#include "bode/writer.h"

bode::FastqReader::FastqReader(std::string const &fn,int ml) {
  maxlen = ml;
  buffer = new char[maxlen];
  temp.reserve(maxlen);
  temp[0] = '\0';
  _filename = fn;
  _fd = new std::ifstream(_filename.c_str(),std::ifstream::in);
  if (_fd->rdstate() & std::ifstream::failbit) {
    throw _fd->rdstate();
  }
  _fq = new bode::Fastq();
}

bode::FastqReader::~FastqReader() {
  delete _fd;
  delete buffer;
  delete _fq;
}

bode::Sequence *bode::FastqReader::nextS(void) {
  _fd->getline(buffer,maxlen);
  if (_fd->eof()) {
    return NULL;
  }
  temp.assign(buffer);
  _fq->setName(temp);
  _fd->getline(buffer,maxlen);
  temp.assign(buffer);
  _fq->setSeq(temp);
  _fd->getline(buffer,maxlen);
  _fd->getline(buffer,maxlen);
  temp.assign(buffer);
  _fq->setQual(temp);
  return _fq;
}

void bode::FastqReader::close(void) {
  _fd->close();
}

bool bode::FastqReader::eof(void) {
  return _fd->eof();
}

void *bode::FastqReader::header(void) {
  return NULL;
}

bode::Writer *bode::FastqReader::getWriter(void) {
  bode::Writer *x;
 
  x = new bode::Writer();
  return x;
}

#if 0
void bode::Fastq::zeroQual(unsigned start,unsigned length) {
  unsigned i;
  unsigned quallen;
  char zero = 33;
  quallen = strlen(quality);
  if (start >= quallen) {
    return; /* can't start past the end of the sequence, or before the start */
  }
  if (length == 0 || /* default: from 'start' to end of sequence */
      start + length > quallen) { /* writing past end of sequence */
    length = quallen - start;
  } 
  for (i=start;i<start+length;i++) {
    quality[i] = zero;
  }
}

void bode::Fastq::mask(unsigned start,unsigned length,char symbol) {
  bode::Sequence::mask(start,length,symbol);
  zeroQual(start,length);
}
#endif

