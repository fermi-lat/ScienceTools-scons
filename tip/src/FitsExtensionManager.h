/** \file FitsExtensionUtils.h

    \brief Utilities to help manage FITS specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsExtensionUtils_h
#define table_FitsExtensionUtils_h

#include <cassert>
#include <map>
#include <sstream>
#include <string>

#include "fitsio.h"

#include "FitsPrimProps.h"
#include "table/IExtensionData.h"
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
  class FitsExtensionUtils : public IExtensionData {
    public:
      /** \brief Create an object to provide low-level access to the given FITS extension.
          \param file_name The name of the FITS file.
          \param ext_name The name of the FITS extension.
      */
      FitsExtensionUtils(const std::string & file_name, const std::string & ext_name);

      /** \brief Destructor. Closes file if it is open.
      */
      virtual ~FitsExtensionUtils();

      /** \brief Create a header object which refers to this file. Caller is responsible for deleting
          the header object.
      */
      virtual IHeaderData * getHeaderData();

      /** \brief Create a data object (table or image) which refers to this file. Caller is responsible
          for deleting the header object.
      */
      virtual ITabularData * getTabularData();

      // General support for FITS files:
      /** \brief Open the FITS file and return Cfitsio's fitsfile pointer.
      */
      void open();

      /** \brief Close the FITS file.
      */
      void close();

      // Header/keyword-specific support:
      /** \brief Templated function which can get keywords from a FITS table, converted to any data type.
          \param name The name of the keyword.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getKeywordGeneric(const std::string & name, T & value) const;

      // Table-specific support:
      /** Struct holding information about the FITS columns.
      */
      struct ColumnInfo {
          std::string m_name;
          long m_repeat;
          int m_col_num;
      };

      /** \brief Open the FITS table. Exceptions will be thrown if the extension does not exist, or if
          the extension is not a table.
      */
      void openTable();

      // Non-virtual helper functions for ITabularData interface:
      
      /** \brief Return the number of records in the current tabular data object (the number of rows
          in the FITS file).
      */
      Index_t getNumRecords() const { return m_num_records; }

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Return the number of elements in the given field (the number of items in a vector column.
      */
      Index_t getFieldNumElements(FieldIndex_t field_index) const;

      /** \brief Templated function which can get any kind of data from a FITS table. This
          method throws an exception if the extension is not a table.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      template <typename T>
      void getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end, T * dest) const;

      /** \brief Templated function which can set any kind of data in a FITS table. This
          method throws an exception if the extension is not a table.
          \param field_index The index of the field (column) to set.
          \param record_index The record index (row number) whose value to set.
          \param src_begin Index of the first element within the Cell to set.
          \param dest_begin Pointer to the first element in the output sequence.
          \param dest_end Pointer to one past the last element in the output sequence.
      */
      template <typename T>
      void setCellGeneric(int col_num, Index_t record_index, Index_t src_begin, T * dest_begin, T * dest_end);

    private:
      std::string formatWhat(const std::string & msg) const;

      std::string m_file_name;
      std::string m_ext_name;
      std::map<std::string, ColumnInfo> m_col_name_lookup;
      std::map<int, ColumnInfo> m_col_num_lookup;
      Index_t m_num_records;
      fitsfile * m_fp;
      IHeaderData * m_header;
      IData * m_data;
  };

  // Getting keywords.
  template <typename T>
  inline void FitsExtensionUtils::getKeywordGeneric(const std::string & name, T & value) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &value, 0, &status);
    if (status) throw TableException(formatWhat(std::string("Cannot read keyword ") + name));
  }

  // Getting keywords as bool is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsExtensionUtils::getKeywordGeneric<bool>(const std::string & name, bool & value) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (status) throw TableException(formatWhat(std::string("Cannot read keyword ") + name));
    value = tmp;
  }

  // Getting keywords as strings is a special case because Cfitsio gets them as char *.
  template <>
  inline void FitsExtensionUtils::getKeywordGeneric<std::string>(const std::string & name, std::string & value) const {
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (status) throw TableException(formatWhat(std::string("Cannot read keyword ") + name));
    value = tmp;
  }

  // Getting columns.
  template <typename T>
  inline void FitsExtensionUtils::getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end,
    T * dest_begin) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fits_read_col(m_fp, data_type_code, col_num, record_index + 1, src_begin + 1, src_end - src_begin, 0,
      dest_begin, 0, &status);
    if (status) {
      std::ostringstream s;
      s << "Cannot read record number " << record_index << " from column number " << col_num;
      throw TableException(formatWhat(s.str()));
    }
  }

  // Getting column values as bools is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsExtensionUtils::getCellGeneric<bool>(int col_num, Index_t record_index, Index_t src_begin,
    Index_t src_end, bool * dest) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    char tmp[1];
    for (Index_t ii = src_begin; ii != src_end; ++ii) {
      fits_read_col(m_fp, data_type_code, col_num, record_index + 1, ii, 1, 0, tmp, 0, &status);
      if (status) {
        std::ostringstream s;
        s << "Cannot read record number " << record_index << " from column number " << col_num;
        throw TableException(formatWhat(s.str()));
      }
      *dest++ = *tmp;
    }
  }

  // Getting column values as strings is a special case because Cfitsio gets them as char *.
  // This is tricky and painful to get right because of Cfitsio's way of indicating the width
  // of columns. Since there is no immediate need to manage strings, this is simply not supported
  // for now.
  template <>
  inline void FitsExtensionUtils::getCellGeneric<std::string>(int col_num, Index_t record_index, Index_t src_begin,
    Index_t src_end, std::string * dest) const {
    assert(0);
  }

  // Setting columns.
  template <typename T>
  inline void FitsExtensionUtils::setCellGeneric(int col_num, Index_t record_index, Index_t src_begin,
    T * dest_begin, T * dest_end) {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fits_write_col(m_fp, data_type_code, col_num, record_index + 1, src_begin + 1, dest_end - dest_begin,
      dest_begin, &status);
    if (status) {
      std::ostringstream s;
      s << "Cannot write record number " << record_index << " from column number " << col_num;
      throw TableException(formatWhat(s.str()));
    }
  }

  // Setting column values as bools is a special case because Cfitsio treats them as ints.
  template <>
  inline void FitsExtensionUtils::setCellGeneric<bool>(int col_num, Index_t record_index, Index_t src_begin,
    bool * dest_begin, bool * dest_end) {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    char tmp[1];
    for (; dest_begin != dest_end; ++dest_begin, ++src_begin) {
      *tmp = *dest_begin;
      fits_write_col(m_fp, data_type_code, col_num, record_index + 1, src_begin, 1, tmp, &status);
      if (status) {
        std::ostringstream s;
        s << "Cannot write record number " << record_index << " from column number " << col_num;
        throw TableException(formatWhat(s.str()));
      }
    }
  }

  // Setting column values as strings is not supported. See note above getCellGeneric.
  template <>
  inline void FitsExtensionUtils::setCellGeneric<std::string>(int col_num, Index_t record_index, Index_t src_begin,
    std::string * dest_begin, std::string * dest_end) {
    assert(0);
  }

}

#endif
