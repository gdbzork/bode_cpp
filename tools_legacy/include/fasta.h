/*
 * fasta.h
 *
 * A simple, fast reader for short fasta sequences.  Typical usage is to
 * allocate one object (or however many you need at the same time), and
 * use that to read and process successive Fasta sequences.
 * 
 * Assumptions:
 *  a) No sequence or header will be longer than "maxseqlen".
 *  b) Each sequence will consist of exactly two lines: the header line and
 *     one line of sequence data.
 *
 * Warning: This module does essentially no error checking.  It is
 * designed for speed, not safety.
 */

#ifndef FASTA_H
#define FASTA_H

typedef struct fasta {
  char *id;
  char *data;
  int maxlen;
} *fastap;

/* fa_alloc -- Allocates a Fasta structure which can contain up to
 *             'maxseqlen' characters of sequence data.
 */
fastap fa_alloc(int maxseqlen);

/* fa_next -- Reads the next Fasta sequence from 'fd' into 'seq'.
 *            Returns 0 on end-of-file (or error),
 *                    1 if the read was successful.
 */
int fa_next(fastap seq,FILE *fd);

/* fa_fasta -- Writes 'seq', in Fasta format, to 'fd', which must be open
 *             for writing.
 */
void fa_fasta(fastap seq,FILE *fd);

/* fa_fasta_trunc -- Writes up to 'len' characters of 'seq', in Fasta format, 
 *                   to 'fd', which must be open for writing.  Destructively
 *                   truncates the sequence.
 */
void fa_fasta_trunc(fastap seq,FILE *fd,int len);

/* fa_free -- Deallocates a Fasta structure.
 */
void fa_free(fastap seq);

/* fa_mask -- overwrite part of a sequence with a given letter.
 */
void fa_mask(fastap seq,unsigned start,unsigned length,char mask);

#endif /* FASTA_H */
