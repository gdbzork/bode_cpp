#include "bode/bodeException.h"

// Note that inline methods are documented here to keep the header file clean.

/// \fn BodeException::BodeException(std::string const &msg)
/// \brief Constructor which takes a message string for the recipient.

/// \fn BodeException::BodeException(void)
/// \brief Default constructor, just sets the message to an empty string.

/// \fn std::string const BodeException::msg(void)
/// \brief Returns the message associated with this exception.

namespace bode {

/**
 * \brief Write a BodeException to an output stream.
 */
std::ostream& operator<<(std::ostream&s,const bode::BodeError &be) {
  s << be.lbl() << ": " << be.msg();
  return s;
}

}
