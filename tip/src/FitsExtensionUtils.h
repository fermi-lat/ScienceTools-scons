/** \file FitsExtensionUtils.h

    \brief Utilities to help manage FITS specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsExtensionUtils_h
#define table_FitsExtensionUtils_h

#include <string>

#include "fitsio.h"

#include "FitsPrimProps.h"
#include "table/TableException.h"
#include "table/table_types.h"

namespace table {

  class IData;
  class IHeaderData;

  /** \class FitsExtensionUtils

      \brief Low level interface to FITS format extensions. This is not part of the API.

      This class is a standalone utility class which encapsulates Cfitsio (fitsfile *) access. It also
      acts as a factory for creating FITS-specific header and data objects, which refer back to the
      FitsExtensionUtils object which created them.
  */
  class FitsExtensionUtils {
    public:
      /** \brief Create an object to provide low-level access to the given FITS extension.
          \param file_name The name of the FITS file.
          \param ext_name The name of the FITS extension.
      */
      FitsExtensionUtils(const std::string & file_name, const std::string & ext_name);

      /** \brief Destructor. Closes file if it is open.
      */
      ~FitsExtensionUtils();

      /** \brief Create a header object which refers to this file. Caller is responsible for deleting
          the header object.
      */
      IHeaderData * createHeader();

      /** \brief Create a data object (table or image) which refers to this file. Caller is responsible
          for deleting the header object.
      */
      IData * createData();

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
      std::string m_file_name;
      std::string m_ext_name;
      fitsfile * m_fp;
  };

  // Getting keywords.
  template <typename T>
  inline void FitsExtensionUtils::getKeywordGeneric(const std::string & name, T & value) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &value, 0, &status);
    if (status) throw TableException();
  }

  // Getting keywords as bool is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsExtensionUtils::getKeywordGeneric<bool>(const std::string & name, bool & value) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

  // Getting keywords as strings is a special case because Cfitsio gets them as char *.
  template <>
  inline void FitsExtensionUtils::getKeywordGeneric<std::string>(const std::string & name, std::string & value) const {
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

}

#endif
