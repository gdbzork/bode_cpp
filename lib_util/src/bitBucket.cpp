#include "bode/bodeException.h"
#include "bode/bitBucket.h"

namespace bode {

BitBucket::BitBucket(const int n) {
  size = n;
  bits = new unsigned char[n];
  for (int i=0;i<n;i++) {
    bits[i] = 0;
  }
}

BitBucket::~BitBucket(void) {
  delete[] bits;
}

void BitBucket::set(const int i) {
  if (i < 0 || i >= size) {
    throw bode::BodeError("Range error in BitBucket");
  }
  bits[i] = 1;
}

void BitBucket::reset(const int i) {
  if (i < 0 || i >= size) {
    throw bode::BodeError("Range error in BitBucket");
  }
  bits[i] = 0;
}

bool BitBucket::isSet(const int i) {
  if (i < 0 || i >= size) {
    throw bode::BodeError("Range error in BitBucket");
  }
  return bits[i] == 1;
}

void BitBucket::clear(void) {
  for (int i=0; i<size; i++) {
    bits[i] = 0;
  }
}

bool BitBucket::operator[](const int i) {
  if (i < 0 || i >= size) {
    throw bode::BodeError("Range error in BitBucket");
  }
  return bits[i] == 1;
}

}
