#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <map>

namespace bode {

enum ParamType {BOOL,INT,DOUBLE,STRING};

struct param {
  std::string key;
  std::string default_val;
  ParamType ptype;
};

class Configuration {
  public:
    Configuration(void) {};
    Configuration(std::string defaults[][2]);
    Configuration(struct param defaults[]);
    void load(std::string const &filenames);
    std::string get(std::string const &key);
    long getInt(std::string const &key);
    double getDouble(std::string const &key);
    bool getBool(std::string const &key);
    void set(std::string const &key,std::string const &val);
    void add(std::map<std::string,std::string>);
    std::vector<std::string> *keys(void);
    bool isSet(std::string const &key);
    static bool isInt(std::string const &s);
    static bool isDouble(std::string const &s);
    static bool isBool(std::string const &s);
  private:
    enum SOURCE { DEFAULT, CONFIGFILE, COMMANDLINE };
    void fload(std::string fn);
    void lparse(char *s);
    std::map<std::string,std::string> dict;
    std::map<std::string,ParamType> ptype;
    std::map<std::string,SOURCE> source;
};

}

#endif
