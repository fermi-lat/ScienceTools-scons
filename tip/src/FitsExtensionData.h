/** \file FitsExtensionData.h

    \brief FITS-specific extension abstractions.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsExtensionData_h
#define table_FitsExtensionData_h

#include <string>

#include "table/IExtensionData.h"

namespace table {

  class FitsExtensionUtils;
  class IData;
  class IHeaderData;
  class ITabularData;

  /** \class FitsExtensionData

      \brief FITS-specific implmentation of IExtensionData interface.
  */
  class FitsExtensionData : public IExtensionData {
    public:
      /** \brief Construct a FitsExtensionData object from the given FITS file name and extension.
          \param file_name The FITS file name.
          \param ext_name The FITS file extension.
      */
      FitsExtensionData(const std::string & file_name, const std::string & ext_name);

      /** Destruct object, flushing and closing all file information.
      */
      virtual ~FitsExtensionData();

      /** \brief Return the associated IDataHeader object which encapsulates access to header information.
      */
      virtual IHeaderData * getHeaderData();

      /** \brief Return the associated ITabularData object which encapsulates access to the table information.
          If the current object's FITS file + extension does not refer to a FITS table, a NULL pointer will be
          returned.
      */
      virtual ITabularData * getTabularData();

    private:
      // All direct Cfitsio calls are encapsulated in the FitsExtensionUtils helper class.
      FitsExtensionUtils * m_fits_utils;
      IHeaderData * m_header;
      IData * m_data;
  };

}

#endif
