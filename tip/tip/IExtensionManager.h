/** \file IExtensionData.h

    \brief Low level abstract interface to information common to all extensions.

    \author James Peachey, HEASARC
*/
#ifndef table_IExtensionData_h
#define table_IExtensionData_h

#include <string>

namespace table {

  /** \class IExtensionData

      \brief Low level abstract interface to information common to all extensions.
  */
  class IExtensionData {
    public:
      virtual ~IExtensionData() {}

      /** \brief Read a keyword from this extension.
          \param name The name of the keyword to read.
          \param value The output value.
      */
      virtual void readKeyword(const std::string & name, double & value) const = 0;
  };

}

#endif
