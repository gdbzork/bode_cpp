#ifndef __ALIGNMENTEXCEPTION_H
#define __ALIGNMENTEXCEPTION_H

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Exception for errors in alignment.
 */
class AlignmentException {
  public:
    AlignmentException(char *);
    ~AlignmentException();
  protected:
    char *message;
};

/**
 * \brief <span class=stat_bad>red</span> Exception for errors in alignment length.
 */
class AlignmentLengthException: public AlignmentException {
  public:
    AlignmentLengthException(char *,int,int);
  private:
    int currentLen;
    int newLen;
};

/**
 * \brief <span class=stat_bad>red</span> Exception for errors in alignment counting.
 */
class AlignmentCountException: public AlignmentException {
  public:
    AlignmentCountException(char *,int);
  private:
    int maxCount;
};

}

#endif
