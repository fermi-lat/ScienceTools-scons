/** \file IFileSvc.cxx

    \brief Factory for handling table objects.

    \author James Peachey, HEASARC
*/

#include "FitsFileSvc.h"
#include "table/IFileSvc.h"
#include "table/TableException.h"


namespace table {

  // Static initializations.
  IFileSvc::FileSvcCont_t IFileSvc::m_file_services;
  static FitsFileSvc s_fits_file_factory("fits");

  IFileSvc & IFileSvc::getSvc() {
    // Look for the master file service (presently hard-wired to be fits).
    FileSvcCont_t::iterator it = m_file_services.find("fits");

    // If found, return it. Otherwise complain.
    if (m_file_services.end() != it) return *it->second;
    else throw TableException();
  }

  IFileSvc::~IFileSvc() {
    for (FileSvcCont_t::iterator it = m_file_services.begin(); it != m_file_services.end(); ++it)
      if (this == it->second) m_file_services.erase(it);
  }

  IFileSvc::IFileSvc(const std::string & name): m_name(name) { m_file_services[name] = this; }

}
