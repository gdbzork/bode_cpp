#ifndef __BFLAGS_H
#define __BFLAGS_H

#include <ostream>
#include <vector>
#include <map>

namespace bode {

/// \brief Types of command-line flags supported.
enum FlagType {BOOL,INT,DOUBLE,STRING};

/**
 * \brief <span class=stat_bad>red</span> Command-line option parsing class.
 *
 * This class implements a simple command-line parser, with a set of options
 * and positional parameters.  The programmer calls the add method for each
 * option, specifying the type (of enum FlagType) and description.  Then
 * parse is called.  Assuming the parse is successful, the programmer can then
 * retrieve the values of the options, as well as any positional arguments.
 */
class Flags {
  public:
    /// \brief Flags constructor
    Flags(void): isParsed(false),goodParse(false)
    {};
    virtual ~Flags(void)
    {};
    virtual void add(std::string const &name,FlagType ft,std::string deflt,std::string const &ds);
    virtual void add(std::string const &name,FlagType ft,std::string const &ds);
    virtual void parse(int argc, char * const * const argv);

    virtual bool isSet(std::string const &name) const;
    virtual bool getBool(std::string const &name) const;
    virtual int getInt(std::string const &name) const;
    virtual double getDouble(std::string const &name) const;
    virtual std::string const &getStr(std::string const &name) const;
    virtual std::vector<std::string> const &positionalArgs(void) const;
    virtual bool okay(void) const;

    virtual void printHelp(std::ostream &out) const;

  private:
    Flags(Flags const &x);
    Flags &operator=(Flags const &x);
    bool checkBoolValue(std::string &val);
    bool checkIntValue(std::string const val);
    bool checkDoubleValue(std::string const val);

    bool isParsed;
    bool goodParse;
    std::map<std::string,std::string> descr;
    std::map<std::string,FlagType> ftype;
    std::map<std::string,std::string> opts;
    std::map<std::string,bool> isset;
    std::vector<std::string> args;

};

}

#endif
