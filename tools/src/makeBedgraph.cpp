#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <map>
#include <zlib.h>

#include <bode/chromosome.h>
#include <bode/intervalReader.h>
#include <bode/interval.h>

/* #define DEBUG 1 */

enum FileFormat {UNKFMT,BEDFMT,BAMFMT};
enum Strand {BOTTOMSTRAND,BOTHSTRAND,TOPSTRAND};

#define LINELEN 1024
#define THRESHOLD 1
#define MAPPING_QUAL_THRESHOLD 0
#define FLOATINGPOINT false
#define NORMALIZE 0

static char usage[] = "usage: makeBedgraph [-q nn] [-t nn] [-N nn] [-f] [-3|-5] [-k karyotypeFile] <infile> <outfile>\n";
static char opts[] = "t:d:n:q:N:f35k:";

/***************************************************************/

class Read {
  public:
    Read(void): left(0),right(0) {};
    Read(unsigned l,unsigned r): left(l),right(r) {};

    bool operator<(const Read& other) const {
      return (left < other.left) || (left == other.left && right < other.right);
    }
    inline unsigned l(void) { return left; };
    inline unsigned r(void) { return right; };
  private:
    unsigned left;
    unsigned right;
};

int readOptions(int argc,char **argv,int *threshold,
                                     char *name,
                                     char *description,
                                     int *mappingQualThreshold,
                                     int *normalize,
                                     bool *floatingpoint,
                                     enum Strand *strand,
                                     char *karyoFile) {
  char c;
  *threshold = THRESHOLD;
  *mappingQualThreshold = MAPPING_QUAL_THRESHOLD;
  name[0] = '\0';
  description[0] = '\0';
  *floatingpoint = FLOATINGPOINT;
  *strand = BOTHSTRAND;
  karyoFile[0] = '\0';
  *normalize = NORMALIZE;
  while ((c = getopt(argc,argv,opts)) != -1) {
    switch (c) {
      case 't': *threshold = atoi(optarg); break;
      case 'n': strcpy(name,optarg); break;
      case 'd': strcpy(description,optarg); break;
      case 'q': *mappingQualThreshold = atoi(optarg); break;
      case 'N': *normalize = atoi(optarg); break;
      case 'f': *floatingpoint = true; break;
      case '3': *strand = BOTTOMSTRAND; break;
      case '5': *strand = TOPSTRAND; break;
      case 'k': strcpy(karyoFile,optarg); break;
      case '?': fprintf(stderr,usage); exit(0);
      default:  fprintf(stderr,usage); exit(0);
    }
  }
  return optind;
}

enum FileFormat guessFileType(char *fn) {
  int fnlen;
  char *suffix;
  enum FileFormat ftype;

  ftype = UNKFMT;
  fnlen = strlen(fn);
  suffix = strrchr(fn,'.');
  if (suffix != NULL) {
    if (!strncmp(suffix,".bam",4)) {
      ftype = BAMFMT;
    } else if (!strncmp(suffix,".bed",4)) {
      ftype = BEDFMT;
    } else if (!strncmp(suffix-4,".bed.gz",7)) {
      ftype = BEDFMT;
    }
  }
  return ftype;
}

void processRead(bode::Interval *read,std::map<bode::Chromosome,std::vector<Read> > &chromosomes,int mapQualThr,enum Strand strand) {
  std::string chrom = read->chrom();
  std::vector<Read> &pl = chromosomes[bode::Chromosome(chrom)];
  Read r(read->left(),read->right());

  if (strand != BOTHSTRAND
      && (   (strand == BOTTOMSTRAND && read->strand() == '+')
          || (strand == TOPSTRAND && read->strand() == '-')
         )
     )
  {
    return;
  }
  if (read->score() < mapQualThr) {
    return;
  }
  pl.push_back(r);
}

void generateBedgraph(std::ofstream &fd,
                      std::vector<Read> &pl,
                      int threshold,
                      bode::Chromosome &chrom,
                      int normalize,
                      double normScale,
                      bool floatingpoint,
                      enum Strand strand,
                      std::map<bode::Chromosome,unsigned> &chromSizes) {
  int height,pheight;
  unsigned left,tmpright;
  unsigned i,end;
  double tmpheight;
  double round_offset = 0.5;
  unsigned bufferleft[65536];
  unsigned bufferright[65536];
  int bufferval[65536];
  int bufferindex,z;
  int buffermax;
  bool keeping;
  unsigned chromSize;
  std::vector<Read>::iterator low;
  std::vector<Read>::iterator high;
  std::vector<Read>::iterator j;

  try {
    chromSize = chromSizes.at(chrom);
#ifdef DEBUG
    std::cerr <<"DEBUG: chrom "<<chrom<<" size="<<chromSize<<std::endl;
#endif
  } catch (...) {
    chromSize = 2000000000;
  }

  end = pl.back().r(); /* highest possible value */
  i = pl.front().l(); /* lowest possible value */
  low = pl.begin();
  high = pl.begin();
  height = 0;
  pheight = 0;
  keeping = false;
  bufferindex = 0;
  buffermax = 0;
  left = 0;
  while (i <= end) {
    pheight = height;
    while (high->l() <= i && high < pl.end()) {
      height++;
      high++;
    }
    while (low->r() <= i && low < pl.end()) {
      if (low->r() == i) {
        height--;
      }
	  low++;
    }
    for (j=low;j<high;j++) {
      if (j->r() == i) {
        height--;
      }
    }
    if (height < 0) {
      std::cerr <<"height implausible: "<<height<<" (at "<<i<<")"<<std::endl;
      exit(-1);
    }
    if (pheight != height) {
      if (pheight > threshold || (keeping && pheight > 0)) {
        if (bufferindex > 0) {
          if (bufferindex > buffermax) {
            buffermax = bufferindex;
          }
          for (z=0;z<bufferindex;z++) {
            if (normalize) {
              tmpheight = (double)(bufferval[z]) * normScale;
            } else {
              tmpheight = bufferval[z];
            }
            if (strand == BOTTOMSTRAND) {
              tmpheight = -tmpheight;
              round_offset = -0.5;
            }
            if (bufferleft[z] < chromSize) {
              if (bufferright[z] > chromSize) {
                tmpright = chromSize;
                std::cerr <<"WARNING: truncating "
                          <<chrom<<":"<<bufferleft[z]<<"-"<<bufferright[z]
                          <<" to "<<chrom<<":"<<bufferleft[z]<<"-"<<tmpright
                          <<", chrom end overhang"<<std::endl;
              } else {
                tmpright = bufferright[z];
              }
              if (floatingpoint) {
                fd <<chrom<<"\t"<<bufferleft[z]<<"\t"<<tmpright<<"\t"<<std::setprecision(6)<<std::fixed<<tmpheight<<std::endl;
              } else {
                fd <<chrom<<"\t"<<bufferleft[z]<<"\t"<<tmpright<<"\t"<<(int)(tmpheight+round_offset)<<std::endl;
              }
            } else {
              std::cerr <<"WARNING: omitting "
                        <<chrom<<":"<<bufferleft[z]<<"-"<<bufferright[z]
                        <<", chrom end overhang"<<std::endl;
            }
          }
          bufferindex = 0;
        }
        keeping = true;
        if (normalize) {
          tmpheight = (double)pheight * normScale;
        } else {
          tmpheight = pheight;
        }
        if (strand == BOTTOMSTRAND) {
          tmpheight = -tmpheight;
          round_offset = -0.5;
        }
        if (left < chromSize) {
          if (i > chromSize) {
            tmpright = chromSize;
            std::cerr <<"WARNING: truncating "<<chrom<<":"<<left<<"-"<<i
                      <<" to "<<chrom<<":"<<left<<"-"<<tmpright
                      <<", chrom end overhang"<<std::endl;
          } else {
            tmpright = i;
          }
          if (floatingpoint) {
            fd <<chrom<<"\t"<<left<<"\t"<<tmpright<<"\t"<<std::setprecision(6)<<std::fixed<<tmpheight<<std::endl;
          } else {
            fd <<chrom<<"\t"<<left<<"\t"<<tmpright<<"\t"<<(int)(tmpheight+round_offset)<<std::endl;
          }
        } else {
          std::cerr <<"WARNING: omitting "<<chrom<<":"<<left<<"-"<<i
                        <<", chrom end overhang"<<std::endl;
        }
      } else if (pheight == 0) {
        keeping = false;
        bufferindex = 0;
      } else {
        bufferleft[bufferindex] = left;
        bufferright[bufferindex] = i;
        bufferval[bufferindex] = pheight;
        bufferindex++;
        if (bufferindex >= 65536) {
          std::cerr <<"buffer overflow: i="<<bufferindex<<std::endl;
          exit(-99);
        }
      }
      left = i;
    }
    i++;
  }
}

void tidyString(char *s) {
  int fnlen,i;
  char *dotpos;
  char tmpname[LINELEN];
  char *basepos;
  strcpy(tmpname,s);
  basepos = basename(tmpname);
  strcpy(s,basepos);
  fnlen = strlen(s);
  if (!strcmp(s+fnlen-3,".gz")) {
    s[fnlen-3] = '\0';
    fnlen = fnlen - 3;
  }
  dotpos = strrchr(s,'.');
  if (dotpos != NULL) {
    *dotpos = '\0';
    fnlen = strlen(s);
  }
  for (i=0;i<fnlen;i++) {
    if (s[i] == '_') {
      s[i] = ' ';
    }
  }
}

void loadChromSizes(char *fn,std::map<bode::Chromosome,unsigned> &sizes) {
  std::ifstream fd;
  char line[LINELEN];
  int lcount;

  fd.open(fn);
  lcount = 0;
  while (!fd.eof()  && !fd.fail()) {
    fd.getline(line,LINELEN);
    if (!fd.eof()) {
      std::stringstream s(line);
      std::string chrom;
      int n;
      s >> chrom >> n;
      sizes[bode::Chromosome(chrom)] = n;
      lcount++;
#ifdef DEBUG
      std::cerr << "DEBUG: chrom " << chrom << ": " << n << std::endl;
#endif
    }
  }
  if (!fd.eof() && fd.fail()) {
    std::cerr << "Error reading karyotype file " << fn << std::endl;
    exit(-1);
  }
#ifdef DEBUG
  std::cerr << "DEBUG: read " << lcount << " lines from " << fn << std::endl;
#endif
  fd.close();
}

/***************************************************************/

int main(int argc,char **argv) {
  std::map<bode::Chromosome,std::vector<Read> > chroms;
  std::map<bode::Chromosome,unsigned> chromSizes;
  int optpos;
  int threshold;
  char name[LINELEN];
  char description[LINELEN];
  int mappingQualThreshold;
  int normalize = 0;
  bool floatingpoint = 0;
  char chromSizeFile[LINELEN];
  enum Strand strand;
  char *inFN,*outFN;
  enum FileFormat filetype;
  bode::IntervalReader *inFD;
  std::ofstream outFD;
  bode::Interval *read;

  int total;
  int readcount = 0;
  double normScale = 1.0;
  std::string strand_tag;
  std::string colour_tag;

  optpos = readOptions(argc,argv,&threshold,name,description,
                       &mappingQualThreshold,&normalize,
                       &floatingpoint,&strand,chromSizeFile);
  inFN = argv[optpos];
  outFN = argv[optpos+1];
  if (strlen(name) == 0) {
    strcpy(name,inFN);
    tidyString(name);
  }
  if (strlen(description) == 0) {
    strcpy(description,inFN);
    tidyString(description);
  }

  switch (strand) {
    case BOTTOMSTRAND: strand_tag = "_3p"; colour_tag = "0,0,255"; break;
    case TOPSTRAND: strand_tag = "_5p"; colour_tag = "255,0,0"; break;
    default: strand_tag = ""; colour_tag = "255,0,0";
  }
    
  if (chromSizeFile[0] != '\0') {
    loadChromSizes(chromSizeFile,chromSizes);
  }

  filetype = guessFileType(inFN);
  if (filetype == UNKFMT) {
    std::cerr << "Unable to determine file type of '" << inFN << "'" << std::endl;
    exit(1);
  }

  inFD = bode::IntervalReader::open(inFN);

  read = inFD->nextI();
  while (read != NULL) {
    readcount++;
    processRead(read,chroms,mappingQualThreshold,strand);
    read = inFD->nextI();
  }
  inFD->close();

  total = 0;
  if (normalize) {
    normScale = ((double)normalize) / ((double)readcount);
  }

  outFD.open(outFN,std::ofstream::out);
  outFD << "track type=bedGraph name=\""
        << name << strand_tag
        << "\" description=\"" << description
        << "\" visibility=2 color=" << colour_tag
        << " windowingFunction=maximum alwaysZero=ON" << std::endl;

  
  for (std::map<bode::Chromosome,std::vector<Read> >::iterator it = chroms.begin(); it != chroms.end(); it++) {
    bode::Chromosome chrom(it->first);

    std::vector<Read> &pl = it->second;
    if (pl.size() == 0) {
      continue;
    }
    total += pl.size();
    std::sort(pl.begin(),pl.end());
    
    generateBedgraph(outFD,pl,threshold,chrom,normalize,normScale,
                     floatingpoint,strand,chromSizes);
  }
  outFD.close();
  std::cout << outFN << std::endl;
  return 0;

}
