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
      virtual void getKeyword(const std::string & name, bool & value) const = 0;
      virtual void getKeyword(const std::string & name, double & value) const = 0;
      virtual void getKeyword(const std::string & name, float & value) const = 0;
      virtual void getKeyword(const std::string & name, char & value) const = 0;
      virtual void getKeyword(const std::string & name, signed char & value) const = 0;
      virtual void getKeyword(const std::string & name, signed short & value) const = 0;
      virtual void getKeyword(const std::string & name, signed int & value) const = 0;
      virtual void getKeyword(const std::string & name, signed long & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned char & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned short & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned int & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned long & value) const = 0;
      virtual void getKeyword(const std::string & name, std::string & value) const = 0;

      /** \brief Set a keyword in this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void setKeyword(const std::string & name, const bool & value) = 0;
      virtual void setKeyword(const std::string & name, const double & value) = 0;
      virtual void setKeyword(const std::string & name, const float & value) = 0;
      virtual void setKeyword(const std::string & name, const char & value) = 0;
      virtual void setKeyword(const std::string & name, const signed char & value) = 0;
      virtual void setKeyword(const std::string & name, const signed short & value) = 0;
      virtual void setKeyword(const std::string & name, const signed int & value) = 0;
      virtual void setKeyword(const std::string & name, const signed long & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned char & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned short & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned int & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned long & value) = 0;
      virtual void setKeyword(const std::string & name, const std::string & value) = 0;
      virtual void setKeyword(const std::string & name, const char * const & value) = 0;
  };

}

#endif
