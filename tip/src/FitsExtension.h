/** \file FitsExtension.h

    \brief Low level interface to Fits format extensions. This is not part of the API and should not
    be of interest to table clients.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsExtension_h
#define table_FitsExtension_h

#include <string>

#include "fitsio.h"

#include "FitsPrimProps.h"
#include "table/TableException.h"
#include "table/table_types.h"

namespace table {

  /** \class FitsExtension

      \brief Low level interface to Fits format extensions. This is not part of the API and should not
      be of interest to table clients. Note that despite similarities to the IExtensionData interface,
      this class need not and should not derive from IExtensionData.
  */
  class FitsExtension {
    public:
      FitsExtension(const std::string & file_name, const std::string & ext_name);

      virtual ~FitsExtension();

      template <typename T>
      void getCellGeneric(int col_num, Index_t record_index, T & value) const;

      template <typename T>
      void getKeywordGeneric(const std::string & name, T & value) const;

      fitsfile * open();
      void close();
      fitsfile * getFitsFp() const { return m_fp; }

    private:
      std::string m_file_name;
      std::string m_ext_name;
      fitsfile * m_fp;
  };

  template <typename T>
  inline void FitsExtension::getCellGeneric(int col_num, Index_t record_index, T & value) const {
    int status = 0;
    fits_read_col(m_fp, FitsPrimProps<T>::dataTypeCode(), col_num, record_index + 1, 1, 1, 0, &value, 0, &status);
    if (status) throw TableException();
  }

  template <typename T>
  inline void FitsExtension::getKeywordGeneric(const std::string & name, T & value) const {
    int status = 0;
    fits_read_key(m_fp, FitsPrimProps<T>::dataTypeCode(), const_cast<char *>(name.c_str()), &value, 0, &status);
    if (status) throw TableException();
  }

  template <>
  inline void FitsExtension::getKeywordGeneric<bool>(const std::string & name, bool & value) const {
    int status = 0;
    int tmp = 0;
    fits_read_key(m_fp, FitsPrimProps<bool>::dataTypeCode(), const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

}

#endif
