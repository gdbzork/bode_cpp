#include <stdio.h>
#include <htslib/sam.h>
#include "bode/bamWriter.h"
#include "bode/bam.h"

bode::BamWriter::BamWriter(std::string const &filename,void *header) {
  _hdr = (bam_hdr_t *) header;
  _fd = sam_open(filename.c_str(),"wb");
  sam_hdr_write(_fd,_hdr);
  _open = true;
}

bode::BamWriter::BamWriter(void) {
  _open = false;
  _fd = NULL;
}

bode::BamWriter::~BamWriter(void) {
  close();
}

void bode::BamWriter::close(void) {
  if (_fd != NULL) {
    sam_close(_fd);
    _fd = NULL;
  }
}

void bode::BamWriter::write(Interval const &i) {
  const Bam b = dynamic_cast<const Bam&>(i);
  sam_write1(_fd,_hdr,b.raw());
}

bode::Writer *bode::BamWriter::open(std::string const &filename,void *header) {
  _hdr = (bam_hdr_t *)header;

  if (_fd != NULL) {
    sam_close(_fd);
  }
  _fd = sam_open(filename.c_str(),"wb");
  sam_hdr_write(_fd,_hdr);
  _open = true;
  return this;
}
