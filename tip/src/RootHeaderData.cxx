/** \file RootHeaderData.cxx

    \brief Low level implementation for Root format tabular data.

    \author James Peachey, HEASARC
*/

#include "RootExtensionUtils.h"
#include "RootHeaderData.h"
#include "table/TableException.h"

namespace table {

  RootHeaderData::RootHeaderData(RootExtensionUtils * root_utils): m_root_utils(root_utils) {
    if (!m_root_utils) throw TableException("RootHeaderData::RootHeaderData(RootExtensionUtils *): "
      "Cannot create RootHeaderData with NULL RootExtensionUtils pointer");
  }

  void RootHeaderData::getKeyword(const std::string & name, double & value) const {
//    m_root_utils->getKeywordGeneric<double>(name, value);
  }

  void RootHeaderData::getKeyword(const std::string & name, std::string & value) const {
//    m_root_utils->getKeywordGeneric<std::string>(name, value);
  }

}
