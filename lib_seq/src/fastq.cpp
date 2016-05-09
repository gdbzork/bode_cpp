#include <ostream>
#include <string>

#include "bode/fastq.h"

bode::Fastq::Fastq(std::string const &n,std::string const &s,std::string const &q): Sequence(n,s) {
  _qual = q;
}

bode::Fastq::Fastq(void):Sequence() {
  _qual = "";
}

bode::Fastq::Fastq(bode::Fastq const &other):Sequence(other) {
  _qual = other._qual;
}

bode::Fastq::~Fastq(void) {
}

void bode::Fastq::write(std::ostream &f) {
  f << "@" << _name << std::endl << _seq << std::endl
    << "+" << std::endl << _qual << std::endl;
}

void bode::Fastq::writeFasta(std::ostream &f) {
  f << ">" << _name << std::endl << _seq << std::endl;
}

std::string bode::Fastq::format(void) const {
  return "@" + _name + "\n" + _seq + "\n+\n" + _qual + "\n";
}

std::string bode::Fastq::formatFasta(void) {
  return ">" + _name + "\n" + _seq + "\n";
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
