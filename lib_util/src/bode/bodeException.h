#ifndef __BODEEXCEPTION_H
#define __BODEEXCEPTION_H

#include <string>
#include <exception>
#include <ostream>

namespace bode {

/**
 * \brief Base class for errors in the BODE library.
 *
 * The basic model of error and exception handling in BODE is that BodeError
 * and most subclasses indicate software errors in the program, i.e. bugs,
 * while BodeException and its subclasses indicate exceptional coditions in
 * the environment: user error, illegal data format, system errors, etc.
 *
 * Broadly speaking, BodeExceptions should be caught, while BodeErrors should
 * be allowed to propagate to the user level, i.e. crash (unless this is used
 * in some sort of high-availability system, or something...).
 *
 * At some point we may want 3 levels: BodeBug (software error), BodeError
 * (unrecoverable environment error), BodeException (recoverable environment
 * error).  But let's not get too fancy just yet.
 */
class BodeError {
  public:
    BodeError(std::string const &lbl,std::string const &msg):
      label(lbl),
      message(msg)
    {};
    BodeError(std::string const &msg): label("BodeError"), message(msg)
    {};
    BodeError(void): label("BodeError"), message("Generic Message")
    {};
    virtual ~BodeError(void)
    {};
    std::string const &msg(void) const
    { return message; };
    std::string const &lbl(void) const
    { return label; };
  private:
    std::string label;
    std::string message;
};

std::ostream& operator<<(std::ostream&s,const BodeError &be);

/**
 * \brief Base class for exceptions in the BODE library.
 */
class BodeException: public BodeError {
  public:
    BodeException(std::string const &lbl,std::string const &msg):
      BodeError(lbl,msg)
    {};
    BodeException(std::string const &msg): BodeError("BodeException",msg)
    {};
    BodeException(void): BodeError("BodeException","Generic Message")
    {};
};

/**
 * \brief Thrown if an error occurs in a Flags object.
 */
class FlagError: public BodeError {
  public:
    FlagError(std::string const &lbl,std::string const &msg): BodeError(lbl,msg)
    {};
    FlagError(std::string const &msg): BodeError("FlagError",msg)
    {};
    FlagError(void): BodeError("FlagError","Generic Message")
    {};
};

/**
 * \brief Thrown if an exceptional condition occurs in a Flags object.
 */
class FlagException: public BodeException {
  public:
    FlagException(std::string const &lbl,std::string const &msg):
      BodeException(lbl,msg)
    {};
    FlagException(std::string const &msg): BodeException("FlagException",msg)
    {};
    FlagException(void): BodeException("FlagException","Generic Message")
    {};
};

/**
 * \brief Thrown if calling code invokes the "fatal" log method.
 */
class LogError: public BodeError {
  public:
    LogError(std::string const &lbl,std::string const &msg): BodeError(lbl,msg)
    {};
    LogError(std::string const &msg): BodeError("LogError",msg)
    {};
    LogError(void): BodeError("LogError","Generic Message")
    {};
};

/**
 * \brief Thrown if a parameter does not match its listed type.
 */
class ParameterException: public BodeException {
  public:
    ParameterException(std::string const &lbl,std::string const &msg):
      BodeException(lbl,msg)
    {};
    ParameterException(std::string const &msg): BodeException("ParameterException",msg)
    {};
    ParameterException(void): BodeException("ParameterException","Generic Message")
    {};
};

}

#endif
