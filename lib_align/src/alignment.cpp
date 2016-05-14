#include <ostream>
#include <iostream>
#include <iomanip>
#include <string.h>
#include <assert.h>
#include "bode/alignmentException.h"
#include "bode/alignment.h"

namespace bode {

Alignment::Alignment(int cnt=2,int ml=1024) {
  slots = cnt;
  maxlen = ml;
  used = 0;
  seqs = new char *[cnt];
  assert(seqs != NULL);
  for (unsigned i=0;i<slots;i++) {
    seqs[i] = new char[maxlen+1];
    assert(seqs[i] != NULL);
  }
  starts = new int[cnt];
  assert(starts != NULL);
  ends = new int[cnt];
  assert(ends != NULL);
  alen = 0;
  sc = NEGINF;
}

Alignment::~Alignment() {
  for (unsigned i=0;i<slots;i++) {
    delete seqs[i];
  }
  delete seqs;
  delete starts;
  delete ends;
}

void Alignment::clear() {
  used = 0;
}

void Alignment::addSeq(char *seq,int start,int end) {
  if (strlen(seq) > maxlen) {
    throw AlignmentLengthException((char *)"string length exceeds max",
                                   maxlen,strlen(seq));
  }
  if (used >= slots) {
    throw AlignmentCountException((char *)"too many strings in alignment",
                                   slots);
  }
  strcpy(seqs[used],seq);
  starts[used] = start;
  ends[used] = end;
  used++;
  if (alen == 0) {
    alen = strlen(seq);
  } else {
    if (strlen(seq) != alen) {
      std::cerr << "warning: incoming seqlen " << strlen(seq) << " != alignlen "
           << alen << std::endl;
    }
  }
}

void Alignment::setScore(int scr) {
  sc = scr;
}

int Alignment::score() {
  return sc;
}

int Alignment::identity() {
  int ident = 0;
  if (used < 1) {
    return alen;
  }
  for (unsigned i=0;i<alen;i++) {
    char c = seqs[0][i];
    int match = 1;
    for (unsigned j=1;j<used;j++) {
      if (seqs[j][i] != c) {
        match = 0;
      }
    }
    ident += match;
  }
  return ident;
}

int Alignment::alignlen() {
  return alen;
}

void Alignment::dump(std::ostream *f,int indent,char *label) {
  *f << std::setw(indent) << "" << "Alignment " << label << std::endl;
  *f << std::setw(indent) << "" << "  maxlen=" << maxlen
                           << " slots=" << slots
                           << " used=" << used
                           << " alen=" << alen
                           << " sc=" << sc
                           << " ident=" << identity() << std::endl;
  for (unsigned i=0;i<used;i++) {
    *f << std::setw(indent) << "" << seqs[i] << " " << starts[i]
                                        << " " << ends[i] << std::endl;
  }
}

}
