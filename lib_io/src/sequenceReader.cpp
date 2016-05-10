#include <string>

#include "bode/sequence.h"
#include "bode/sequenceReader.h"
#include "bode/bamReader.h"
#include "bode/fastaReader.h"
#include "bode/fastqReader.h"

bode::SequenceReader::~SequenceReader(void) {
  /* this function exists solely to prevent a weird compiler error. */
}

bode::SequenceReader *bode::SequenceReader::open(std::string const &filename) {
  SequenceReader *r;
  int flen;

  // Ugly.  Must fix.
  // See http://stackoverflow.com/questions/582331/c-is-there-a-way-to-instantiate-objects-from-a-string-holding-their-class-name
  // for a fix.

  flen = filename.length();
  if (filename.compare(flen-4,4,".bam") == 0) {
    r = new BamReader(filename);
  } else if (filename.compare(flen-4,4,".fa") == 0) {
    r = new FastaReader(filename);
  } else if (filename.compare(flen-4,4,".fq") == 0) {
    r = new FastqReader(filename,1024);
  }
  return r;
}
