/** \file RootExtensionData.cxx

    \brief Root-specific low level abstract interface to all extension types.
    \author James Peachey, HEASARC
*/

#include "RootExtensionData.h"
#include "RootExtensionUtils.h"
//#include "RootTabularData.h"
#include "table/IData.h"
#include "table/IHeaderData.h"
#include "table/ITabularData.h"

namespace table {

  // Construct without opening the file.
  RootExtensionData::RootExtensionData(const std::string & file_name, const std::string & ext_name):
    m_root_utils(0), m_header(0), m_data(0) {
    try {
      m_root_utils = new RootExtensionUtils(file_name, ext_name);
      m_header = m_root_utils->createHeader();
      m_data = m_root_utils->createData();
    } catch(...) {
      delete m_data; m_data = 0;
      delete m_header; m_header = 0;
      delete m_root_utils; m_root_utils = 0;
      throw;
    }
  }

  // Close file automatically while destructing.
  RootExtensionData::~RootExtensionData() { delete m_data; delete m_header; delete m_root_utils; }

  IHeaderData * RootExtensionData::getHeaderData() { return m_header; }

//  ITabularData * RootExtensionData::getTabularData() { return dynamic_cast<RootTabularData *>(m_data); }
  ITabularData * RootExtensionData::getTabularData() { return dynamic_cast<ITabularData *>(m_data); }

}
