#include <string>
#include <cstdlib>
#include <htslib/sam.h>

#include "bode/bam.h"
#include "bode/interval.h"
#include "bode/bamReader.h"

bode::BamReader::BamReader(std::string const &filename) {
  _fd = sam_open(filename.c_str(),"rb");
  _hdr = sam_hdr_read(_fd);
  _seq = bam_init1();
  _bseq = new Bam();
  _bseq->setHeader(_hdr);
  _eof = false;
}

bode::BamReader::BamReader(void) {
  _fd = NULL;
  _seq = NULL;
  _bseq = NULL;
  _eof = true;
}

bode::BamReader::~BamReader(void) {
  close();
  if (_seq != NULL) {
    if (_seq->data != NULL) {
      free(_seq->data);
      _seq->data = NULL;
    }
    free(_seq);
    _seq = NULL;
  }
  delete _bseq;
}

void bode::BamReader::close(void) {
  if (_fd != NULL) {
    sam_close(_fd);
    _fd = NULL;
  }
}

bode::Interval *bode::BamReader::nextI(void) {
  int samrv;
  Interval *rv = NULL;

  samrv = sam_read1(_fd,_hdr,_seq);
  if (samrv > 0) {
    _bseq->update(_seq);
    rv = _bseq;
  } else {
    _bseq->setUnmapped();
    _eof = true;
  }
  return rv;
}

bode::Sequence *bode::BamReader::nextS(void) {
  int samrv;
  Sequence *rv = NULL;

  samrv = sam_read1(_fd,_hdr,_seq);
  if (samrv > 0) {
    _bseq->update(_seq);
    rv = _bseq;
  } else {
    _bseq->setUnmapped();
    _eof = true;
  }
  return rv;
}

bode::BamReader *bode::BamReader::open(std::string const &filename) {
  BamReader *br = new BamReader(filename);
  return br;
}
