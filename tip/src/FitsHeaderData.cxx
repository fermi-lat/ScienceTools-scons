/** \file FitsHeaderData.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include "FitsExtensionUtils.h"
#include "FitsHeaderData.h"
#include "table/TableException.h"

namespace table {

  FitsHeaderData::FitsHeaderData(FitsExtensionUtils * fits_utils): m_fits_utils(fits_utils) {
    if (!m_fits_utils) throw TableException("FitsHeaderData::FitsHeaderData(FitsExtensionUtils *): "
      "Cannot create FitsHeaderData with NULL FitsExtensionUtils pointer");
    m_fits_utils->open();
  }

  void FitsHeaderData::getKeyword(const std::string & name, double & value) const {
    m_fits_utils->getKeywordGeneric<double>(name, value);
  }

  void FitsHeaderData::getKeyword(const std::string & name, std::string & value) const {
    m_fits_utils->getKeywordGeneric<std::string>(name, value);
  }

}
