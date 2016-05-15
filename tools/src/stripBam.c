#include <stdio.h>

#include <bam/bam.h>
#include <bam/sam.h>

int main(int argc,char **argv) {
  samfile_t *fdIn,*fdOut;
  bam1_t *inSeq,*outSeq;
  
  fdIn = samopen(argv[1],"rb",NULL);
  fdOut = samopen(argv[2],"wb",fdIn->header);

  inSeq = bam_init1();
  outSeq = bam_init1();

  outSeq->core.l_qname = 2;
  outSeq->core.n_cigar = 1;
  outSeq->core.l_qseq = 0;
  outSeq->l_aux = 0;
  outSeq->data_len = 6;
  outSeq->m_data = 8;
  outSeq->data = (uint8_t *) calloc(8,sizeof(uint8_t));
  outSeq->data[0] = 0x2a;
  outSeq->data[1] = 0;
  outSeq->data[2] = 0x40;
  outSeq->data[3] = 0x02;
  outSeq->data[4] = 0;
  outSeq->data[5] = 0;
  
  while (samread(fdIn,inSeq) > 0) {
    outSeq->core.tid = inSeq->core.tid;
    outSeq->core.pos = inSeq->core.pos;
    outSeq->core.bin = inSeq->core.bin;
    outSeq->core.qual = inSeq->core.qual;
    outSeq->core.flag = inSeq->core.flag;
    outSeq->core.mtid = inSeq->core.mtid;
    outSeq->core.mpos = inSeq->core.mpos;
    outSeq->core.isize = inSeq->core.isize;
    samwrite(fdOut,outSeq);
  }
  samclose(fdIn);
  samclose(fdOut);
  return 0;
}
