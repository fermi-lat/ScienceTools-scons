/** \file FitsExtensionData.h

    \brief Low level abstract interface to Fits tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsExtensionData_h
#define table_FitsExtensionData_h

#include <string>

#include "fitsio.h"

#include "table/IExtensionData.h"

namespace table {

  /** \class FitsExtensionData

      \brief Low level abstract interface to Fits tabular data.
  */
  class FitsExtensionData : public IExtensionData {
    public:
      FitsExtensionData(const std::string & file_name, const std::string & ext_name);

      virtual ~FitsExtensionData();

      /** \brief Get a keyword from this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value.
      */
      virtual void getKeyword(const std::string & name, double & value) const;

      fitsfile * open();

      void close();

      fitsfile * getFitsFp() const { return m_fp; }

    private:
      std::string m_file_name;
      std::string m_ext_name;
      fitsfile * m_fp;
  };

}

#endif
