#include <map>
#include <string>
#include "bode/interval.h"
#include "bode/intervalSet.h"
using namespace std;

namespace bode {

bode::IntervalSet::IntervalSet() {
  chroms = new map<string,IntervalTree *,chromComp>();
}

bode::IntervalSet::~IntervalSet() {
  map<string,IntervalTree *,chromComp>::iterator it;
  for (it=chroms->begin();it!=chroms->end();it++) {
    delete it->second;
  }

  delete chroms;
}

void bode::IntervalSet::insert(Interval const *inter) {
  const string chrom = inter->chrom();
  if (chroms->count(chrom) == 0) {
    (*chroms)[chrom] = new IntervalTree();
  }
  (*chroms)[chrom]->insert(inter->left(),inter->right());
}

int bode::IntervalSet::coverage(std::string const &chrom,int point) const {
  int rv;
  if (chroms->count(chrom) == 0) {
    rv = 0;
  } else {
    rv = (*chroms)[chrom]->coverage(point);
  }
  return rv;
}

void bode::IntervalSet::density(std::string const &chrom,int left,int right,int *dest) const {
  if (chroms->count(chrom) != 0) {
    (*chroms)[chrom]->density(left,right,dest);
  }
}

int bode::IntervalSet::overlapping(Interval const *inter) const {
  int rv;
  const string chrom = inter->chrom();
  if (chroms->count(chrom) == 0) {
    rv = 0;
  } else {
    rv = (*chroms)[chrom]->reads(inter->left(),inter->right());
  }
  return rv;
}

int bode::IntervalSet::chromCount() const {
  return chroms->size();
}

int bode::IntervalSet::count() const {
  int reads = 0;
  map<string,IntervalTree *,chromComp>::iterator it;
  for (it=chroms->begin();it!=chroms->end();it++) {
    reads += it->second->getCount();
  }
  return reads;
}

int bode::IntervalSet::realCount() const {
  int reads = 0;
  map<string,IntervalTree *,chromComp>::iterator it;
  for (it=chroms->begin();it!=chroms->end();it++) {
    reads += it->second->realCount();
  }
  return reads;
}

std::map<std::string,bode::IntervalTree *,bode::chromComp>::iterator bode::IntervalSet::chromIter() {
  return chroms->begin();
}

}
