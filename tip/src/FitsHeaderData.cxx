/** \file FitsHeaderData.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include "FitsExtensionUtils.h"
#include "FitsHeaderData.h"

namespace table {

  void FitsHeaderData::getKeyword(const std::string & name, double & value) const {
    m_fits_utils->getKeywordGeneric<double>(name, value);
  }

  void FitsHeaderData::getKeyword(const std::string & name, std::string & value) const {
    m_fits_utils->getKeywordGeneric<std::string>(name, value);
  }

}
