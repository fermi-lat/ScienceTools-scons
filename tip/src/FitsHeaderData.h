/** \file FitsHeaderData.h

    \brief FITS-specific implementation of the IHeaderData interface.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsHeaderData_h
#define table_FitsHeaderData_h

#include <string>

#include "table/IHeaderData.h"

namespace table {

  class FitsExtensionUtils;

  /** \class FitsHeaderData

      \brief FITS-specific implementation of the IHeaderData interface.
  */
  class FitsHeaderData : public IHeaderData {
    public:
      /** \brief Create a FitsHeaderData object which refers to the given utility object.
          \param fits_utils Pointer to the FITS utility object.
      */
      FitsHeaderData(FitsExtensionUtils * fits_utils): m_fits_utils(fits_utils) {}

      virtual ~FitsHeaderData() {}

      /** \brief Get a keyword from this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, double & value) const;
      virtual void getKeyword(const std::string & name, std::string & value) const;

    private:
      FitsExtensionUtils * m_fits_utils;
  };

}

#endif
