/** \file FitsFileSvc.h

    \brief Factory for handling Fits objects.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsFileSvc_h
#define table_FitsFileSvc_h

#include "table/IFileSvc.h"

namespace table {

  /** \class FitsFileSvc

      \brief Singleton factory for creating, editing and reading tables and images from files.
  */
  class FitsFileSvc : public IFileSvc {
    public:
      FitsFileSvc(const std::string & name);

      /** \brief Open an existing Fits table with modification access.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name);
  };

}

#endif
