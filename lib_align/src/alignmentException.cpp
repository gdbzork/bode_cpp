#include <string.h>
#include "bode/alignmentException.h"

namespace bode {

AlignmentException::AlignmentException(char *msg) {
  message = strdup(msg);
}

AlignmentException::~AlignmentException() {
  delete message;
}

AlignmentLengthException::AlignmentLengthException(char *msg,int c,int n):AlignmentException(msg) {
  currentLen = c;
  newLen = n;
}

AlignmentCountException::AlignmentCountException(char *msg,int m):AlignmentException(msg) {
  maxCount = m;
}

}
