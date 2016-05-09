#include <string>
#include <ostream>

#include "bode/sequence.h"

bode::Sequence::Sequence(std::string const &n,std::string const &s) {
  _name = n;
  _seq = s;
  _null = false;
}

bode::Sequence::Sequence(bode::Sequence const &other) {
  _name = other._name;
  _seq = other._seq;
  _null = other._null;
}

bode::Sequence::~Sequence(void) {
}

void bode::Sequence::write(std::ostream &out) {
  out << _name << '\t' << _seq << std::endl;
}

std::string bode::Sequence::format(void) const {
  return _name + '\t' + _seq + '\n';
}
