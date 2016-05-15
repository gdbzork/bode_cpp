#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <stdio.h>

typedef void *histogram;

histogram hist_new();
void hist_increment(histogram hist,int key);
int hist_contains(histogram hist,int key);
int hist_get(histogram hist,int key);
void hist_write(histogram hist);
void hist_traverse(histogram hist,void(*visit)(int i,int c,void *data),void *data);

#endif
