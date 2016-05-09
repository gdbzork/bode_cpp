#include <ostream>
#include <string.h>
#include "bode/sequence.h"
#include "bode/fasta.h"

bode::Fasta::Fasta(std::string const &n,std::string const &d,std::string const &s):Sequence(n,s) {
  _descr = d;
}

bode::Fasta::Fasta(void):Sequence() {
  _descr = "";
}

bode::Fasta::Fasta(Fasta const &other):Sequence(other) {
  _descr = other._descr;
}

void bode::Fasta::write(std::ostream &out) {
  out << ">" << _name;
  if (_descr != "") {
    out << " " << _descr;
  }
  out << std::endl << _seq << std::endl;
}

std::string bode::Fasta::format(void) const {
  std::string result;
  result = ">" + _name;
  if (_descr != "") {
    result += " " + _descr;
  }
  result += "\n" + _seq + "\n";
  return result;
}
