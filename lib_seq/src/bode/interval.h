#ifndef __INTERVAL_H
#define __INTERVAL_H

#include <string>

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Stores a genomic interval.
 */
class Interval {
  public:
    Interval(std::string const &chr,int l,int r);
    Interval(std::string const &chr,int l,int r,double s);
    Interval(std::string const &chr,int l,int r,double s,char st);
    Interval(void)                                         { _mapped = false; };
    Interval(Interval const &i);
    virtual ~Interval(void) {};
    Interval &operator=(Interval const &i);
    friend bool operator==(Interval const &l,Interval const &r);
    friend bool operator<(Interval const &l,Interval const &r);

    std::string const& chrom(void) const                   { return _chrom; };
    int left(void) const                                   { return _left; }; 
    int right(void) const                                  { return _right; };
    bool isMapped(void) const                              { return _mapped; };
    double score(void) const                               { return _score; };
    char strand(void) const                                { return _strand; };

    virtual void update(std::string const &chr,int l,int r);
    virtual void update(std::string const &chr,int l,int r,char st);
    virtual void setScore(double s)                        { _score = s; };
    virtual std::string format(void) const;
    virtual void setUnmapped(void)                         { _mapped = false; };

  protected:
    int _left;
    int _right;
    std::string _chrom;
    bool _mapped;
    double _score;
    char _strand;

};

bool operator==(Interval const &l,Interval const &r);
bool operator<(Interval const &l,Interval const &r);

}

#endif
