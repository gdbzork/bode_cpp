#include <string>

#include "bode/interval.h"
#include "bode/intervalReader.h"
#include "bode/bamReader.h"
#include "bode/bedReader.h"
#include "bode/cffReader.h"

bode::IntervalReader::~IntervalReader(void) {
  /* this function exists solely to prevent a weird compiler error. */
}

bode::IntervalReader *bode::IntervalReader::open(std::string const &filename) {
  IntervalReader *r;
  int flen;

  // Ugly.  Must fix.
  // See http://stackoverflow.com/questions/582331/c-is-there-a-way-to-instantiate-objects-from-a-string-holding-their-class-name
  // for a fix.

  flen = filename.length();
  if (filename.compare(flen-4,4,".bam") == 0) {
    r = new BamReader(filename);
  } else if (filename.compare(flen-4,4,".cff") == 0) {
    r = new CffReader(filename);
  } else if (filename.compare(flen-4,4,".bed") == 0) {
    r = new BedReader(filename);
  } else if (filename.compare(flen-7,7,".bed.gz") == 0) {
    r = new BedReader(filename);
  }
  return r;
}
