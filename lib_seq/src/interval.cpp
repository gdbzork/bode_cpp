#include <string>
#include <sstream>
#include "bode/interval.h"

bode::Interval::Interval(std::string const &chr,int l,int r) {
  _chrom = chr;
  _left = l;
  _right = r;
  _mapped = true;
  _score = 0.0;
  _strand = '.';
}

bode::Interval::Interval(std::string const &chr,int l,int r,double s) {
  _chrom = chr;
  _left = l;
  _right = r;
  _mapped = true;
  _score = s;
  _strand = '.';
}

bode::Interval::Interval(std::string const &chr,int l,int r,double s,char st) {
  _chrom = chr;
  _left = l;
  _right = r;
  _mapped = true;
  _score = s;
  _strand = st;
}

bode::Interval::Interval(Interval const &i) {
  _chrom = i._chrom;
  _left = i._left;
  _right = i._right;
  _mapped = i._mapped;
  _score = i._score;
  _strand = i._strand;
}

bode::Interval &bode::Interval::operator=(Interval const &i) {
  if (this != &i) {
    _chrom = i._chrom;
    _left = i._left;
    _right = i._right;
    _mapped = i._mapped;
    _score = i._score;
    _strand = i._strand;
  }
  return *this;
}

bool bode::operator==(bode::Interval const &l,bode::Interval const &r) {
  return (l._chrom == r._chrom &&
          l._left == r._left &&
          l._right == r._right);
}

bool bode::operator<(Interval const &l,Interval const &r) {
  return (l._chrom < r._chrom ||
          (l._chrom == r._chrom && l._left < r._left) ||
          (l._chrom == r._chrom && l._left == r._left && l._right < r._right));
}

void bode::Interval::update(std::string const &chr,int l,int r) {
  _chrom = chr;
  _left = l;
  _right = r;
  _mapped = true;
}

void bode::Interval::update(std::string const &chr,int l,int r,char st) {
  _chrom = chr;
  _left = l;
  _right = r;
  _mapped = true;
  _strand = st;
}

std::string bode::Interval::format(void) const {
  std::ostringstream out;
  out << _chrom << ":" << _left << "-" << _right;
  return out.str();
}
