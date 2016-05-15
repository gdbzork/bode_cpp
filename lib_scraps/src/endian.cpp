#include <iostream>

enum ByteOrder {BIGENDIAN,LITTLEENDIAN,UNKNOWN_ENDIAN};

ByteOrder getByteOrder(void) {
  union {
    long l;
    char c[4];
  } test;
  test.l = 1;
  if( test.c[3] && !test.c[2] && !test.c[1] && !test.c[0] )
    return BIGENDIAN;
  if( !test.c[3] && !test.c[2] && !test.c[1] && test.c[0] )
    return LITTLEENDIAN;
  return UNKNOWN_ENDIAN;
}

int main() {
  ByteOrder end;

  end = getByteOrder();
  switch (end) {
    case BIGENDIAN:
      std::cout << "big-endian" << std::endl;
      break;
    case LITTLEENDIAN:
      std::cout << "little-endian" << std::endl;
      break;
    case UNKNOWN_ENDIAN:
      std::cout << "unknown endian" << std::endl;
      break;
  }
  return 0;
}
