#ifndef UTIL_H
#define UTIL_H

/* stripWhiteSpace -- strip white space from the beginning and end of a
 *                    string, returning the passed-in pointer.  Assumes
 *                    the string is null-terminated.
 */
char *stripWhiteSpace(char *str);

/* reverseComplementInPlace -- reverse-complements a null-terminated
 *                             nucleotide string, respecting IUPAC ambiguity
 *                             codes.
 */
void reverseComplementInPlace(char *str);

/* phredQStr2Int -- convert a null-terminated string of Phred quality
 *                  values to integer scores.
 */
void phredQStr2Int(int *dest,char *src);

/* phredScore2Prob -- convert integer Phred scores to float probabilities.
 *                    Assumes the src and dest vectors contain 'count'
 *                    items.
 */
void phredScore2Prob(float *dest,int *src,int count);

int isFastaFile(char *filename);
int isFastqFile(char *filename);
int isRegularFile(char *filename);
int fileExists(char *filename);
int split(char *str,char **dest,int max);
int splits(char *str,char **dest,int max);

#endif
