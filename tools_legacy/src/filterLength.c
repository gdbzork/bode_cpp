/*
 * Filter Fasta sequences by length.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "sequence.h"
#include "fastareader.h"

int lower = 60;
int upper = 10000;

static char opts[] = "l:u:";
int getOptions(int argc,char **argv) {
  char c;
  while ((c = getopt(argc,argv,opts)) != -1) {
    switch (c) {
      case 'l': lower = atoi(optarg);
                break;
      case 'u': upper = atoi(optarg);
                break;
    }
  }
  return optind;
}

int main(int argc,char **argv) {
  fastareader input;
  sequence seq;
  int index;
  FILE *output;

  index = getOptions(argc,argv);
  seq = seq_alloc();
  input = fr_open(argv[index]);
  output = fopen(argv[index+1],"w");
  while ((seq = fr_nextseq(input,seq))) {
    seq_assertSanity(seq);
    if (seq->length >= lower && seq->length <= upper) {
      seq_print(output,seq);
    }
  }
  return 0;
}
