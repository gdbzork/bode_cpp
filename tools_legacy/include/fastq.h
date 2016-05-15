/*
 * fastq.h
 *
 * A simple, fast reader for fastq sequences.  Typical usage is to
 * allocate one object (or however many you need at the same time), and
 * use that to read and process successive Fastq sequences.
 * 
 * Assumptions:
 *  a) No sequence or header will be longer than "maxseqlen".
 *  b) Each sequence will consist of exactly four lines: the header line
 *     one line of sequence data, the second header, and the quality line.
 *     The second header should either be a single "+", or exactly match
 *     the first header, except for the "@" and "+". The sequence and quality
 *     lines should, of course, be the same length.
 *
 * Warning: This module does essentially no error checking.  It is
 * designed for speed, not safety.
 */

#ifndef FASTQ_H
#define FASTQ_H

typedef struct fastq {
  char *id;
  char *data;
  char *qual;
  int maxlen;
} *fastqp;

/* fq_alloc -- Allocates a Fastq structure which can contain up to
 *             'maxseqlen' characters of sequence data.
 */
fastqp fq_alloc(int maxseqlen);

/* fq_next -- Reads the next Fastq sequence from 'fd' into 'seq'.
 *            Returns 0 on end-of-file (or error),
 *                    1 if the read was successful.
 */
int fq_next(fastqp seq,FILE *fd);

/* fq_fastq -- Writes 'seq', in Fastq format, to 'fd', which must be open
 *             for writing.
 */
void fq_fastq(fastqp seq,FILE *fd);

/* fq_fastq_trunc -- Writes up to 'len' characters of 'seq', in Fastq format, 
 *                   to 'fd', which must be open for writing.  Destructively
 *                   truncates the sequence.
 */
void fq_fastq_trunc(fastqp seq,FILE *fd,int len);

/* fq_fasta -- Writes 'seq', in Fasta format, to 'fd', which must be open
 *             for writing.
 */
void fq_fasta(fastqp seq,FILE *fd);

/* fq_fasta_trunc -- Writes up to 'len' characters of 'seq', in Fasta format, 
 *                   to 'fd', which must be open for writing.  Destructively
 *                   truncates the sequence.
 */
void fq_fasta_trunc(fastqp seq,FILE *fd,int len);

/* fq_free -- Deallocates a fastq structure.
 */
void fq_free(fastqp seq);

/* fq_setQualityToZero -- set quality to zero for some region of a sequence.
 *                        If 'length' is zero, it defaults to the end of the
 *                        sequence.  If 'isSolexa' is non-zero, Solexa-type
 *                        quality values are assumed.  Otherwise, Phred-type.
 *                        (Solexa: offset=64,   Phred: offset=33)
 */
void fq_setQualityToZero(fastqp seq,int start,int length,int isSolexa);

/* fq_mask -- overwrite part of sequence with a given letter.
 */
void fq_mask(fastqp seq,unsigned start,unsigned length,char mask);

#endif /* FASTQ_H */
