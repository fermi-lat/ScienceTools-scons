/** \file IFileSvc.cxx

    \brief Factory for handling table objects.

    \author James Peachey, HEASARC
*/

#include "FileSvc.h"
#include "table/IFileSvc.h"
#include "table/TableException.h"


namespace table {

  // Static initializations.
  // Singleton registry (container) of IFileSvc objects.
  IFileSvc::FileSvcCont_t IFileSvc::m_file_services;

  // Create the factory.
  static FileSvc s_file_factory;

  IFileSvc & IFileSvc::getSvc() {
    // Look for the master file service, which currently can handle either FITS or Root format.
    FileSvcCont_t::iterator it = m_file_services.find("default");

    // If found, return it. Otherwise complain.
    if (m_file_services.end() != it) return *it->second;
    else throw TableException("Cannot find a file service object");
  }

  // Destructor for a file service removes it from the registry (container) m_file_services.
  IFileSvc::~IFileSvc() {
    for (FileSvcCont_t::iterator it = m_file_services.begin(); it != m_file_services.end(); ++it)
      if (this == it->second) m_file_services.erase(it);
  }

  // Protected constructor which adds the current object to the registry of IFileSvc objects.
  IFileSvc::IFileSvc(const std::string & format_name): m_format_name(format_name)
    { m_file_services[format_name] = this; }

}
