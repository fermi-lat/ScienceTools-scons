/** \file RootHeaderData.h

    \brief Root-specific implementation of the IHeaderData interface.

    \author James Peachey, HEASARC
*/
#ifndef table_RootHeaderData_h
#define table_RootHeaderData_h

#include <string>

#include "table/IHeaderData.h"

namespace table {

  class RootExtensionUtils;

  /** \class RootHeaderData

      \brief Root-specific implementation of the IHeaderData interface.
  */
  class RootHeaderData : public IHeaderData {
    public:
      /** \brief Create a RootHeaderData object which refers to the given utility object.
          \param root_utils Pointer to the Root utility object.
      */
      RootHeaderData(RootExtensionUtils * root_utils);

      virtual ~RootHeaderData() {}

      /** \brief Get a keyword from this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, double & value) const;
      virtual void getKeyword(const std::string & name, std::string & value) const;

    private:
      RootExtensionUtils * m_root_utils;
  };

}

#endif
