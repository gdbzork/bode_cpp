#ifndef __BED_H
#define __BED_H

#include <string>

#include "interval.h"

namespace bode {

/**
 * \brief <span class=stat_bad>red</span> Stores BED reads.
 */
class Bed: public Interval {
  public:
    Bed(std::string const &chr,int l,int r);
    Bed(std::string const &chr,int l,int r,std::string n,double score,char s);
    Bed(void);
    Bed(Bed const &b);

    std::string const& name(void) const { return _name; };
    char strand(void) const             { return _strand; };

    virtual void update(std::string const &chr,int l,int r);
    virtual void update(std::string const &chr,int l,int r,std::string const &n,double score,char s);
    virtual std::string format(void) const;

  protected:
    std::string _name;
};

}

#endif
