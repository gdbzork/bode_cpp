#include <string>
#include <vector>

#include "bode/intervalReader.h"
#include "bode/writer.h"
#include "bode/locationFilter.h"
#include "bode/flags.h"

int main(int argc,char **argv) {
  int total,kept;
  bode::Flags *flagset;
  std::vector<std::string> args;
  bool invert;

  flagset = new bode::Flags();
  flagset->add("invert",bode::BOOL,"exclude reads in regions of interest");
  flagset->parse(argc,argv);
  args = flagset->positionalArgs();

  invert = flagset->getBool("invert");
  std::string regionFN(args[1]);
  std::string inFN(args[2]);
  std::string outFN(args[3]);
  
  bode::IntervalReader *regions = bode::IntervalReader::open(regionFN);
  bode::LocationFilter *f = new bode::LocationFilter(regions,invert);
  regions->close();

  bode::IntervalReader *input = bode::IntervalReader::open(inFN);
  bode::Writer *output = input->getWriter();
  output->open(outFN,input->header());
  f->filter(input,output,kept,total);
  input->close();
  output->close();
  fprintf(stderr,"kept %d / %d\n",kept,total);

  delete regions;
  delete f;
  delete input;
  delete output;
  
  return 0;
}
