#include "bode/histogram.h"

/// \brief Default constructor, creates a histogram with bins from 0-50.
bode::Histogram::Histogram(void) {
  l = 0;
  h = 50;
  bins = new int[h+1];
  for (int i=0;i<h-l+1;i++) {
    bins[i] = 0;
  }
}

/// \brief Constructor to create histogram with user-specified bins.

/// Creates a histogram with bins in a range provided by the user.
/// \param low the lowest-numbered bin
/// \param high the highest-numbered bin (so there are actually high-low+1 bins)
bode::Histogram::Histogram(int low,int high) {
  l = low;
  h = high;
  bins = new int[h-l+1];
  for (int i=0;i<h-l+1;i++) {
    bins[i] = 0;
  }
}

/// \brief Destructor (obviously...)
bode::Histogram::~Histogram(void) {
  delete[] bins;
}

/// \brief Increments the specified bin by 1.
/// \param bin the bin to increment
void bode::Histogram::increment(int bin) {
  bins[bin-l] += 1;
}

/// \brief Adds 'count' to the specified bin.
/// \param bin the bin to increment
/// \param count the amount to add to the bin
void bode::Histogram::add(int bin,int count) {
  bins[bin-l] += count;
}

/// \brief Get the current value of the specified bin.
/// \param bin the bin to query
int bode::Histogram::get(int bin) {
  return bins[bin-l];
}
