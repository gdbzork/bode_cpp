#include <fstream>

#include "bode/intervalReader.h"
#include "bode/bedReader.h"
#include "bode/writer.h"
#include "bode/readCounter.h"

int main(int argc,char **argv) {
  std::string readsFN(argv[1]);
  std::string inFN(argv[2]);
  std::string outFN(argv[3]);

  bode::IntervalReader *reads = bode::IntervalReader::open(readsFN);
  bode::BedReader *in = new bode::BedReader(inFN);
  std::ofstream *out = new std::ofstream(outFN.c_str(),std::ofstream::out);
  
  bode::ReadCounter *r = new bode::ReadCounter(reads);
  r->countReadsOnIntervals(in,out);
  in->close();
  out->close();

  return 0;
}
