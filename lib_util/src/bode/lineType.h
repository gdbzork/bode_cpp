#ifndef LINETYPE_H
#define LINETYPE_H

#include <string>
#include <istream>

namespace bode {

enum TextType {UNIX,DOS,MAC,MIXED,UNKNOWN,FAIL};

class TextTypeChecker {
  public:
    static TextType sget(std::string fn,unsigned limit=10000);
    static TextType fget(std::istream &fd,unsigned limit=10000);
    static std::string text(TextType tt);
  private:
    static TextType get(char *buffer,unsigned limit=10000);
};

}

#endif
