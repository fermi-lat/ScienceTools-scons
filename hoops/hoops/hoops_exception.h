/******************************************************************************
 *   File name: hoops_exception.h                                             *
 *                                                                            *
 * Description: Exception class for use by hoops.                             *
 *                                                                            *
 *    Language: C++                                                           *
 *                                                                            *
 *      Author: James Peachey, for HEASARC/GSFC/NASA                          *
 *                                                                            *
 *  Change log: see CVS Change log at the end of the file.                    *
 ******************************************************************************/
#ifndef HOOPS_EXCEPTION_H
#define HOOPS_EXCEPTION_H

////////////////////////////////////////////////////////////////////////////////
// C++ header files.
////////////////////////////////////////////////////////////////////////////////
#include <exception>
#include <string>
////////////////////////////////////////////////////////////////////////////////

namespace hoops {

  //////////////////////////////////////////////////////////////////////////////
  // Constants.
  //////////////////////////////////////////////////////////////////////////////
  // Enumerated codes to describe each type of Hexception.
  enum HexceptionCode_e {
    P_OK = 0,              // No exception, normal execution.
    P_ILLEGAL = 1,         // No rules exist to perform the operation.
    P_OVERFLOW = 2,        // Source value > maximum destination value.
    P_UNDERFLOW = 3,       // Source value < minimum destination value.
    P_BADSIZE = 4,         // Destination may be smaller than the source.
    P_PRECISION = 5,       // Conversion between integral and floating types.
    P_SIGNEDNESS = 6,      // Conversion between signed and unsigned int. types.
    P_STR_OVERFLOW = 7,    // Attempt to convert a string which contained
                           // non-space characters after the number.
    P_STR_INVALID = 8,     // Attempt to convert a string which did not
                           // contain a number.
    P_STR_NULL = 9,        // Attempt to convert from a null string.
    P_UNEXPECTED = 10      // (Not thrown) An error occurred which does
                           // not fit into one of the other categories.
  };
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type declarations/definitions.
  //////////////////////////////////////////////////////////////////////////////
  class Hexception: public std::exception {
    public:
      virtual ~Hexception() throw() {}

      Hexception(const int & code, const std::string & msg = std::string(),
        const std::string & filename = std::string(), int line = 0):
        mMsg(msg), mCode(code) {}
      int Code() const { return mCode; }
      const std::string & Msg() const { return mMsg; }

    private:
      std::string mMsg;
      int mCode;
  };
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Type declarations/definitions.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Global variable forward declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  // Function declarations.
  //////////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////////

}
#endif

/******************************************************************************
 * $Log$
 * Revision 1.1  2003/04/11 19:20:38  peachey
 * New component HOOPS, an object oriented parameter interface. Low
 * level access currently uses PIL, but this can be changed.
 *
 ******************************************************************************/
