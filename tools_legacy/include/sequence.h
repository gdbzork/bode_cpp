#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stdio.h>

/* seqstruct -- a DNA sequence object (or any long string, really...).
 */
typedef struct seqstruct {
  unsigned blocksize;  /* capacity of one block (number of characters)    */
  unsigned blocksUsed;      /* how many blocks are in use currently            */
  unsigned blocksAllocated; /* how many blocks are allocated                   */
  unsigned blocksPossible;  /* how many blocks there is room for in 'data'     */
  unsigned length;          /* length of the current sequence                  */
  char **data;         /* pointer to the list of data blocks              */
  unsigned *position;       /* pointer to list of offsets (since blocks aren't */
                       /* necessarily full)                               */
  unsigned namelen;    /* number of bytes allocated for sequence name     */
  char *name;          /* sequence name                                   */
} *sequence;

/****
 * Allocates a new, empty sequence.
 * Returns: ref to sequence, or NULL if it cannot allocate enough space.
 */
sequence seq_alloc();

/****
 * Allocate a new, empty sequence, with space for 'size' characters.
 * Parameters:
 *   size -- number of characters to pre-allocate space for.
 * Returns: ref to sequence, or NULL if it cannot allocate enough space.
 */
sequence seq_allocn(unsigned size);

/****
 * Allocates a new, empty sequence, using blocks of size 'blockSize'.
 * Parameters:
 *   blockSize -- capacity of each block (number of characters)
 * Returns: ref to sequence, or NULL if it cannot allocate enough space.
 *
 * In general, the default block size should be fine.  If you want to allocate
 * many short sequences, a small block size (but longer than typical sequences)
 * is more space efficient.  For very large sequences (e.g. genome-sized), a
 * larger than default block size may be more time efficient.
 */
sequence seq_allocbs(unsigned blockSize);

/****
 * Allocates a new, empty sequence with blocks of 'blockSize' characters, with
 * enough space allocated for 'size' characters.
 * Parameters:
 *   size -- number of characters to pre-allocate space for.
 *   blockSize -- capacity of each block (number of characters)
 * Returns: ref to sequence, or NULL if it cannot allocate enough space.
 *
 *  See note above regarding block size.
 */
sequence seq_allocnbs(unsigned size,unsigned blockSize);

/****
 * Frees a sequence, returning its space to the OS.
 * Parameters:
 *   seq -- the sequence to free.
 */
void seq_free(sequence seq);

/****
 * Clears the contents of a sequence, so the next 'append' will start a new
 * sequence, but doesn't free its storage.
 * Parameters:
 *   seq -- the sequence to clear.
 */
void seq_clear(sequence seq);

/****
 * Ensures that a sequence has space for 'size' characters.
 * Parameters:
 *   seq -- the sequence to check the capacity of.
 *   size -- the number of characters to ensure space is available for.
 * Returns: ref to the same sequence, or NULL if space allocation fails.
 * 
 * Note: it is not in general necessary to call this method, since it
 * is called internally if necessary, when adding data to a sequence.
 * However, if the approximate length of the sequence is known in advance,
 * it may be more efficient to call this function prior to loading data in
 * (and also less error-prone, since the success of this call guarantees
 * availability of the requested amount of space, ensuring the success of
 * future 'append' requests, up to the amount of space requested).
 */
sequence seq_ensureCapacity(sequence seq,unsigned size);

/****
 * Sets the name of a sequence (replacing the old name, if any).
 * Parameters:
 *   seq -- the sequence to set the name of.
 *   name -- the new name (null-terminated string).
 * Returns: ref to the sequence, or NULL if space allocation for name failed.
 *
 * Note: 'blocksize' characters are initially allocated for the name.  No
 * additional space is allocated unless the name is longer than 'blocksize',
 * in which case a region of memory of sufficient size to hold the name is
 * allocated (and the previously allocated region is freed).  If the attempt
 * to allocate additional space fails, the name remains unchanged (and
 * NULL is returned).
 */
sequence seq_setName(sequence seq,char *name);

/****
 * Returns the name of the sequence.
 * Parameters:
 *   seq -- the sequence to get the name of.
 * Returns: ref to the sequence name, or NULL if space allocation failed.
 *
 * Note: The return value points to a copy of the sequence's name.  It is the
 * responsibility of the caller to free() the space appropriately.
 */
char *seq_getName(sequence seq);

/****
 * Appends at most 'length' characters from 'data' to sequence 'seq'.
 * Parameters:
 *   seq -- the sequence to append to.
 *   data -- the data to append to the sequence.
 *   length -- the maximum amount of data from 'data' to append.
 * Returns: ref to the destination sequence.
 *
 * Note: if a null is encountered in 'data' before 'length' characters have
 * been appended, the append will stop at the null.
 */
sequence seq_append(sequence seq,char *data,unsigned length);

/****
 * Ensures a sequence is uppercase.
 */
void seq_toupper(sequence seq);

/****
 * Prints the sequence to the specified file descriptor, in Fasta format,
 * using a default line width (currently 60 characters).
 * Parameters:
 *   fd -- the file to write the sequence to (must be open for writing).
 *   seq -- the sequence to write.
 */
void seq_print(FILE *fd,sequence seq);

/****
 * Prints the sequence to the specified file descriptor, in Fasta format,
 * with data lines at most 'lineWidth' characters (name line may exceed
 * 'lineWidth' characters).
 * Parameters:
 *   fd -- the file to write the sequence to (must be open for writing).
 *   seq -- the sequence to write.
 *   lineWidth -- width of data lines.
 */
void seq_printlw(FILE *fd,sequence s,unsigned lineWidth);

/****
 * Copies a region of a source sequence to a destination sequence.  Overwrites
 * any prior contents of destination sequence.
 * Parameters:
 *   dest -- destination sequence.
 *   src -- source sequence.
 *   start -- starting position in source sequence (1st char is position zero).
 *   length -- how many characters to copy.
 * Returns: destination sequence, or NULL if space could not be allocated.
 *
 * Note: it is not necessary, in general, to ensure that 'dest' has enough
 * room for the data, since 'seq_ensureCapacity(...)' will be called
 * internally.  However, doing so (successfully) will ensure that the copy
 * does not fail (return NULL).
 */
sequence seq_substr(sequence dest,sequence src,unsigned start,unsigned length);

/****
 * Copies the sequence to an ordinary null-terminated string.
 * Parameters:
 *   seq -- the sequence to copy into the string.
 * Returns: ref to the character string, or NULL if space allocation fails.
 *
 * Note 1: caller is responsible for freeing space pointed to by the returned
 * pointer.
 * Note 2: Likely to fail if the sequence is very long, since allocationg
 * a single character array of many megabytes is error-prone.
 */
char *seq_string(sequence seq);

/****
 * Copies the sequence to a user-supplied string.
 * Parameters:
 *   dest -- the place to copy the sequence to.
 *   src -- the sequence to copy into the string.
 *   space -- the amount of space allocated to 'dest', hence also the maximum
              number of characters to copy (including terminating null).
 * Returns: ref to dest.
 *
 * Note: caller is responsible for ensuring that 'dest' points to at least
 * 'length' bytes.
 */
char *seq_nstring(char *dest,sequence src,unsigned space);

/****
 * Dumps the internal state of the sequence to an output stream.  Useful
 * only for debugging.
 * Parameters:
 *   fd -- the file to write the output to.
 *   seq -- the sequence to dump.
 */
void seq_dump(FILE *fd,sequence seq);

/****
 * Checks data structure for sanity/consistency, crashes via assert()
 * if there's a problem.  Useful for debugging.
 * Parameters:
 *   seq -- the sequence data structure to check for sanity.
 */
void seq_assertSanity(sequence seq);

#endif
