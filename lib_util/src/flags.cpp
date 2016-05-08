#include <map>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <regex.h>
#include <cstdlib>
#include <assert.h>
#include <string.h>

#include "bode/bodeException.h"
#include "bode/flags.h"
#include "bode/util.h"

namespace bode {

/**
 * \brief Add a flag to a Flags object
 * 
 * @param name The name of the parameter.  Restricted to [a-z_].
 * @param ft Flag type, an enum from class FlagType.
 * @param ds Description, displayed if help is printed.
 */
void Flags::add(std::string const &name,FlagType ft,std::string deflt,std::string const &ds) {
  std::map<std::string,std::string>::const_iterator known_it;
  size_t ipos;

  known_it = descr.find(name);
  if (known_it != descr.end()) {
    throw FlagError("duplicate flag: '"+name+"'");
  }
  ipos = name.find_first_not_of("abcdefghijklmnopqrstuvwxyz_");
  if (ipos != std::string::npos) {
    throw FlagError("illegal characters in '"+name+"'");
  }
  descr[name] = ds;
  ftype[name] = ft;
  opts[name] = deflt;
  isset[name] = false;
}


void Flags::add(std::string const &name,FlagType ft,std::string const &ds) {
   add(name,ft,"",ds);
}

void Flags::parse(int argc,char * const * const argv) {
  /* given: list of tokens 
   * return: set of key-value pairs,
   *         list of non-option parameters
   */
  int i;
  bool endOfOpts = false;

  isParsed = false;
  goodParse = true;
  for (i=0;i<argc;i++) {
    if (endOfOpts) {
      args.push_back(argv[i]);
    } else if (strcmp(argv[i],"--") == 0) {
      endOfOpts = true;
    } else if (argv[i][0] == '-' && argv[i][1] == '-') {
      std::string k = std::string(argv[i]+2);
      std::map<std::string,std::string>::iterator kit = descr.find(k);
      if (kit == descr.end()) {
        if (k[0] == 'n' && k[1] == 'o') {
          std::string kbase = std::string(argv[i]+4);
          std::map<std::string,bode::FlagType>::iterator fit;
          fit = ftype.find(kbase);
          if (fit == ftype.end()) {
            throw FlagException("unknown flag: '"+k+"'");
          } else {
            if (fit->second == bode::BOOL) {
              opts[kbase] = "false";
              isset[kbase] = true;
            } else {
              throw FlagException("\"no\" prefix only allowed for boolean flags, not '"+kbase+"'");
            }
          }
        } else {
          throw FlagException("unknown flag: '"+k+"'");
        }
      } else {
        if (ftype[k] == bode::BOOL) {
          opts[k] = "true";
        } else {
          opts[k] = argv[++i];
        }
        isset[k] = true;
      }
    } else {
      args.push_back(argv[i]);
    }
  }
  isParsed = true;

  /* now check the results for sanity... */
  std::map<std::string,std::string>::iterator oit;
  for (oit = opts.begin(); oit != opts.end(); oit++) {
    std::map<std::string,bode::FlagType>::iterator it = ftype.find(oit->first);
    bode::FlagType ft = it->second;
    switch (ft) {
      case bode::BOOL:
        if (!checkBoolValue(oit->second)) {
          throw FlagException("illegal bool value '"+oit->second+"' for '"+oit->first+"'");
        }
        break;
      case bode::INT:
        if (!checkIntValue(oit->second)) {
          throw FlagException("illegal int value '"+oit->second+"' for '"+oit->first+"'");
        }
        break;
      case bode::DOUBLE:
        if (!checkDoubleValue(oit->second)) {
          throw FlagException("illegal float value '"+oit->second+"' for '"+oit->first+"'");
        }
        break;
      case bode::STRING:
        break; /* no such thing as an illegal string... */
    }
  }
  return;
}

bool bode::Flags::checkBoolValue(std::string &val) {
  char const *valchr;
  regex_t rt_true,rt_false;
  int mat;
  bool rv = false;
  
  valchr = val.c_str();
  mat = regcomp(&rt_true,"^(true|t|yes|y)$",REG_EXTENDED|REG_NOSUB|REG_ICASE);
  assert(mat==0);
  mat = regcomp(&rt_false,"^(false|f|no|n)$",REG_EXTENDED|REG_NOSUB|REG_ICASE);
  assert(mat==0);
  mat = regexec(&rt_true,valchr,0,NULL,0);
  if (mat == 0) {
    rv = true;
  } else {
    mat = regexec(&rt_false,valchr,0,NULL,0);
    if (mat == 0) {
      rv = true;
    }
  }
  regfree(&rt_true);
  regfree(&rt_false);
  return rv;
}

bool bode::Flags::checkIntValue(std::string const val) {
  char const *valchr;
  regex_t rt_ival;
  int mat;
  valchr = val.c_str();
  mat = regcomp(&rt_ival,"^[-+]?[0-9]+$",REG_NOSUB|REG_EXTENDED);
  assert(mat==0);
  mat = regexec(&rt_ival,valchr,0,NULL,0);
  regfree(&rt_ival);
  return mat == 0;
}

bool bode::Flags::checkDoubleValue(std::string const val) {
  char const *valchr;
  regex_t rt_fval;
  int mat;
  valchr = val.c_str();
  mat = regcomp(&rt_fval,"^[-+]?[0-9]*.?[0-9]+([eE][-+]?[0-9]+)?$",REG_NOSUB|REG_EXTENDED);
  assert(mat==0);
  mat = regexec(&rt_fval,valchr,0,NULL,0);
  regfree(&rt_fval);
  return mat == 0;
}

bool Flags::isSet(std::string const &name) const {
  std::map<std::string,std::string>::const_iterator sit;
  std::map<std::string,bool>::const_iterator bit;
  if (!isParsed || !goodParse) {
    throw FlagError("parsing failed: cannot test isSet "+name);
  }
  sit = descr.find(name);
  if (sit == descr.end()) {
    throw new FlagError("unknown flag: "+name);
  }
  bit = isset.find(name);
  return (bit->second);
}

std::string const &Flags::getStr(std::string const &name) const {
  std::map<std::string,std::string>::const_iterator it;
  if (!isParsed || !goodParse) {
    throw new FlagError("parsing failed: cannot getStr "+name);
  }
  it = descr.find(name);
  if (it == descr.end()) {
    throw new FlagError("unknown flag: "+name);
  }
  it = opts.find(name);
  if (it == opts.end()) {
    throw new FlagException("flag not set: "+name);
  }
  return it->second;
}

bool Flags::getBool(std::string const &name) const {
  std::string fval;

  fval = getStr(name);
  toLower(fval);
  return (fval == "true" || fval == "t" || fval == "yes" || fval == "y");
}

int Flags::getInt(std::string const &name) const {
  std::string fval;
  fval = getStr(name);
  return atoi(fval.c_str());
}

double Flags::getDouble(std::string const &name) const {
  std::string fval;
  fval = getStr(name);
  return atof(fval.c_str());
}

std::vector<std::string> const &Flags::positionalArgs(void) const{
  return args;
}

void Flags::printHelp(std::ostream &out) const {
  std::map<std::string,std::string>::const_iterator it;
  std::map<std::string,std::string>::const_iterator dit;
  std::map<std::string,bode::FlagType>::const_iterator t_it;

  it = opts.begin();
  dit = descr.begin();
  while (it != opts.end()) {
    dit = descr.find(it->first);
    t_it = ftype.find(it->first);
    switch (t_it->second) {
      case bode::BOOL:
        out << "\t--" << it->first << "\t" << dit->second << std::endl;
        break;
      default:
        out << "\t--" << it->first << " [" << it->second << "]\t" << dit->second << std::endl;
    }
    it++;
  }
  out << std::endl;
}

bool Flags::okay(void) const {
  return goodParse;
}

}
