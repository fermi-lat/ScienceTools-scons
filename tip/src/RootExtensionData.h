/** \file RootExtensionData.h

    \brief Root-specific extension abstractions.

    \author James Peachey, HEASARC
*/
#ifndef table_RootExtensionData_h
#define table_RootExtensionData_h

#include <string>

#include "table/IExtensionData.h"

namespace table {

  class RootExtensionUtils;
  class IData;
  class IHeaderData;
  class ITabularData;

  /** \class RootExtensionData

      \brief Root-specific implmentation of IExtensionData interface.
  */
  class RootExtensionData : public IExtensionData {
    public:
      /** \brief Construct a RootExtensionData object from the given Root file name and extension.
          \param file_name The Root file name.
          \param ext_name The Root file extension.
      */
      RootExtensionData(const std::string & file_name, const std::string & ext_name);

      /** Destruct object, flushing and closing all file information.
      */
      virtual ~RootExtensionData();

      /** \brief Return the associated IDataHeader object which encapsulates access to header information.
      */
      virtual IHeaderData * getHeaderData();

      /** \brief Return the associated ITabularData object which encapsulates access to the table information.
          If the current object's Root file + extension does not refer to a Root table, a NULL pointer will be
          returned.
      */
      virtual ITabularData * getTabularData();

    private:
      // All direct Root calls are encapsulated in the RootExtensionUtils helper class.
      RootExtensionUtils * m_root_utils;
      IHeaderData * m_header;
      IData * m_data;
  };

}

#endif
