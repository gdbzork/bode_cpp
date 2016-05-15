#include <stdio.h>

#include <bam/bam.h>
#include <bam/sam.h>

int main(int argc,char **argv) {
  samfile_t *fdIn;
  bam1_t *inSeq;
  int i;
  
  fdIn = samopen(argv[1],"rb",NULL);
  inSeq = bam_init1();

  while (samread(fdIn,inSeq) > 0) {
/*
    outSeq->core.tid = inSeq->core.tid;
    outSeq->core.pos = inSeq->core.pos;
    outSeq->core.bin = inSeq->core.bin;
    outSeq->core.qual = inSeq->core.qual;
    outSeq->core.flag = inSeq->core.flag;
    outSeq->core.mtid = inSeq->core.mtid;
    outSeq->core.mpos = inSeq->core.mpos;
    outSeq->core.isize = inSeq->core.isize;
*/

    printf("l_qname: %d\n",inSeq->core.l_qname);
    printf("n_cigar: %d\n",inSeq->core.n_cigar);
    printf("l_qseq: %d\n",inSeq->core.l_qseq);
    printf("l_aux: %d\n",inSeq->l_aux);
    printf("data_len: %d\n",inSeq->data_len);
    printf("m_data: %d\n",inSeq->m_data);
    printf("data:");
    for (i=0;i<inSeq->data_len;i++) {
      printf(" %02x",inSeq->data[i]);
    }
    printf("\n");
    
  }
  samclose(fdIn);
  return 0;
}
