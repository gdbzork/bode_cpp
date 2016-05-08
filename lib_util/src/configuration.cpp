#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <string.h>

#include "bode/util.h"
#include "bode/configuration.h"
#include "bode/bodeException.h"

int const MAXLINE = 1024;

bode::Configuration::Configuration(std::string defaults[][2]) {
  int i=0;
  while (defaults[i][0] != "") {
    dict[defaults[i][0]] = defaults[i][1];
    ptype[defaults[i][0]] = bode::STRING;
    source[defaults[i][0]] = bode::Configuration::DEFAULT;
    i++;
  }
}

bode::Configuration::Configuration(bode::param defaults[]) {
  int i=0;
  while (defaults[i].key != "") {
    /* insert type checking here... */
    dict[defaults[i].key] = defaults[i].default_val;
    ptype[defaults[i].key] = defaults[i].ptype;
    source[defaults[i].key] = bode::Configuration::DEFAULT;
    i++;
  }
}

bool bode::Configuration::isInt(std::string const &s) {
  long x;
  bool okay;
  char *endptr;
  x = strtol(s.c_str(),&endptr,10);
  okay = *endptr == '\0' && s.c_str()[0] != '\0';
  return okay;
}

bool bode::Configuration::isDouble(std::string const &s) {
  char *endptr;
  double x;
  bool okay;
  x = strtod(s.c_str(),&endptr);
  okay = *endptr == '\0' && s.c_str()[0] != '\0';
  return okay;
}

bool bode::Configuration::isBool(std::string const &s) {
  bool okay;
  std::string bval;
  bval = s;
  toLower(bval);
  okay = bval == "true" || bval == "t" || bval == "yes" || bval == "y" ||
         bval == "false" || bval == "f" || bval == "no" || bval != "n";
  return okay;
}

void bode::Configuration::lparse(char *s) {
  char *colon;

  bode::stripWhiteSpace(s);
  if (s[0] == '\0' || s[0] == '#') {
    return;
  }
  colon = strchr(s,':');
  if (colon == NULL) { /* no separator? */
    dict[std::string(s)] = "true";
  } else {
    *colon = '\0';
    bode::stripWhiteSpace(s);
    bode::stripWhiteSpace(colon+1);
    dict[std::string(s)] = std::string(colon+1);
  }
}

void bode::Configuration::fload(std::string fn) {
  std::ifstream *fd;
  char buffer[MAXLINE];
  fd = new std::ifstream(fn.c_str(),std::ifstream::in);
  if (fd->is_open()) {
    fd->getline(buffer,MAXLINE);
    while (fd->gcount() > 0) {
      lparse(buffer);
      fd->getline(buffer,MAXLINE);
    }
    fd->close();
  } else {
    /* fprintf(stderr,"skipped %s, failed open\n",fn.c_str()); */
  }
  delete fd;
}

void bode::Configuration::load(std::string const &filenames) {
  int ncount;
  std::vector<std::string> *names = bode::splitV(filenames,':');
  ncount = names->size();
  for (int i=0;i<ncount;i++) {
    fload(names->at(i));
  }
  delete names;
}

std::string bode::Configuration::get(std::string const &key) {
  return dict[key];
}

long bode::Configuration::getInt(std::string const &key) {
  std::string fval;
  char *endptr;
  long x;
  fval = get(key);
  x = strtol(fval.c_str(),&endptr,10);
  if (*endptr != '\0') {
    throw bode::ParameterException("illegal integer value '"+fval+"'");
  }
  return x;
}

double bode::Configuration::getDouble(std::string const &key) {
  std::string fval;
  char *endptr;
  double x;
  fval = get(key);
  x = strtod(fval.c_str(),&endptr);
  if (*endptr != '\0') {
    throw bode::ParameterException("illegal double value '"+fval+"'");
  }
  return x;
}

bool bode::Configuration::getBool(std::string const &key) {
  std::string fval;
  bool x;
  fval = get(key);
  toLower(fval);
  x = (fval == "true" || fval == "t" || fval == "yes" || fval == "y");
  if (!x && fval != "false" && fval != "f" && fval != "no" && fval != "n") {
    throw bode::ParameterException("illegal boolean value '"+fval+"'");
  }
  return x;
}

void bode::Configuration::set(std::string const &key,std::string const &value) {
  dict[key] = value;
}

/*void bode::Configuration::add(bode::Flags &f) {
  
} */

void bode::Configuration::add(std::map<std::string,std::string>) {
}

std::vector<std::string> *bode::Configuration::keys(void) {
  std::vector<std::string> *s;
  std::map<std::string,std::string>::iterator it = dict.begin();
  s = new std::vector<std::string>();
  while (it != dict.end()) {
    s->push_back(it->first);
    s++;
  }
  return s;
}

bool bode::Configuration::isSet(std::string const &key) {
  return dict.find(key) != dict.end();
}
