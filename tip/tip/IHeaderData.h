/** \file IHeaderData.h

    \brief Abstract interface to a FITS-like header, viewed as a collection of keywords, accessed by
    the name of the keyword.

    \author James Peachey, HEASARC
*/
#ifndef tip_IHeaderData_h
#define tip_IHeaderData_h

#include <string>

namespace tip {

  /** \class IHeaderData

      \brief Abstract interface to a FITS-like header, viewed as a collection of keywords, accessed by
      the name of the keyword.
  */
  class IHeaderData {
    public:
      virtual ~IHeaderData() {}

      /** \brief Get a keyword from this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, double & value) const = 0;
      virtual void getKeyword(const std::string & name, std::string & value) const = 0;
  };

}

#endif
