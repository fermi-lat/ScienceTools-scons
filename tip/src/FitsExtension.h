/** \file FitsExtension.h

    \brief utilities to help manage FITS specific table access. These classes are not part of the API.

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

      \brief Low level interface to FITS format extensions. This is not part of the API.

      Despite similarities to the IExtensionData interface and to the Extension class, this class
      properly is a standalone utility class.
  */
  class FitsExtension {
    public:
      /** \brief Create an object to provide low-level access to the given FITS extension.
          \param file_name The name of the FITS file.
          \param ext_name The name of the FITS extension.
      */
      FitsExtension(const std::string & file_name, const std::string & ext_name);

      /** \brief Destructor. Closes file if it is open.
      */
      ~FitsExtension();

      /** \brief Templated function which can get any kind of data from a FITS table. This
          method throws an exception if the extension is not a table.
          \param col_num The number of the column.
          \param record_index The record (row) number.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getCellGeneric(int col_num, Index_t record_index, T & value) const;

      /** \brief Templated function which can get keywords from a FITS table, converted to any data type.
          \param name The name of the keyword.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getKeywordGeneric(const std::string & name, T & value) const;

      /** \brief Open the FITS file and return Cfitsio's fitsfile pointer.
      */
      fitsfile * open();

      /** \brief Close the FITS file.
      */
      void close();

      /** \brief Return Cfitsio's fitsfile pointer.
      */
      fitsfile * getFitsFp() const { return m_fp; }

    private:
      static const FieldIndex_t s_max_scalar_len = 64;
      std::string m_file_name;
      std::string m_ext_name;
      fitsfile * m_fp;
  };

  // Getting columns.
  template <typename T>
  inline void FitsExtension::getCellGeneric(int col_num, Index_t record_index, T & value) const {
    int status = 0;
    fits_read_col(m_fp, FitsPrimProps<T>::dataTypeCode(), col_num, record_index + 1, 1, 1, 0, &value, 0, &status);
    if (status) throw TableException();
  }

  // Getting column values as bools is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsExtension::getCellGeneric<bool>(int col_num, Index_t record_index, bool & value) const {
    int status = 0;
    int tmp = 0;
    fits_read_col(m_fp, FitsPrimProps<bool>::dataTypeCode(), col_num, record_index + 1, 1, 1, 0, &tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

  // Getting column values as strings is a special case because Cfitsio gets them as char *.
  template <>
  inline void FitsExtension::getCellGeneric<std::string>(int col_num, Index_t record_index, std::string & value) const {
    int status = 0;
    char tmp[s_max_scalar_len];
    fits_read_col(m_fp, FitsPrimProps<std::string>::dataTypeCode(), col_num, record_index + 1, 1, 1, 0, tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

  // Getting keywords.
  template <typename T>
  inline void FitsExtension::getKeywordGeneric(const std::string & name, T & value) const {
    int status = 0;
    fits_read_key(m_fp, FitsPrimProps<T>::dataTypeCode(), const_cast<char *>(name.c_str()), &value, 0, &status);
    if (status) throw TableException();
  }

  // Getting keywords as bool is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsExtension::getKeywordGeneric<bool>(const std::string & name, bool & value) const {
    int status = 0;
    int tmp = 0;
    fits_read_key(m_fp, FitsPrimProps<bool>::dataTypeCode(), const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

  // Getting keywords as strings is a special case because Cfitsio gets them as char *.
  template <>
  inline void FitsExtension::getKeywordGeneric<std::string>(const std::string & name, std::string & value) const {
    int status = 0;
    char tmp[FLEN_KEYWORD];
    fits_read_key(m_fp, FitsPrimProps<std::string>::dataTypeCode(), const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

}

#endif
