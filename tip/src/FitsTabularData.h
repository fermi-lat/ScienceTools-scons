/** \file FitsTabularData.h

    \brief Low level abstract interface to Fits tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsTabularData_h
#define table_FitsTabularData_h

#include <map>
#include <string>

#include "fitsio.h"

#include "FitsExtension.h"
#include "FitsPrimProps.h"
#include "table/ITabularData.h"
#include "table/table_types.h"

namespace table {

  /** \class FitsTabularData

      \brief Low level abstract interface to Fits tabular data.
  */
  class FitsTabularData : public ITabularData {
    public:
      /** \brief Create a new FitsTabularData object for the given table.
          \param file_name The name of the FITS file.
          \param table_name The name of the FITS table.
      */
      FitsTabularData(const std::string & file_name, const std::string & table_name);

      /** \brief Copy contructor. Reopens its own copy of the FITS file.
          \param fits_data The source object being copied.
      */
      FitsTabularData(const FitsTabularData & fits_data);

      /** \brief Destructor. Closes the FITS file.
      */
      virtual ~FitsTabularData();

      /** \brief Return the number of records in the current tabular data object.
      */
      virtual Index_t getNumRecords() const;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Get a value from the current tabular data object.
          \param field The name of the field (column) to get.
          \param record_index The index whose value to get.
          \param value The output value.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, bool & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, double & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, float & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, char & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed char & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed short & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed int & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed long & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned char & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned short & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned int & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned long & value) const;

      virtual void getCell(FieldIndex_t field_index, Index_t record_index, std::vector<double> & value) const;

      /** \brief Get a keyword from this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value.
      */
      virtual void getKeyword(const std::string & name, double & value) const;
      virtual void getKeyword(const std::string & name, std::string & value) const;

    private:
      struct ColumnInfo {
          std::string m_name;
          long m_repeat;
          int m_col_num;
      };

      static const FieldIndex_t s_max_scalar_len = 64;

      /** \brief Templated function which can get any kind of data from a FITS table. This
          method throws an exception if the extension is not a table.
          \param col_num The number of the column.
          \param record_index The record (row) number.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getCellGeneric(int col_num, Index_t record_index, T & value) const;

      /** \brief Templated function which can get any kind of vector data from a FITS table. This
          method throws an exception if the extension is not a table.
          \param col_num The number of the column.
          \param record_index The record (row) number.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getVectorCellGeneric(int col_num, Index_t record_index, T & value) const;

      FitsExtension m_extension;
      std::map<std::string, ColumnInfo> m_col_name_lookup;
      std::map<int, ColumnInfo> m_col_num_lookup;
      std::string m_file_name;
      std::string m_table_name;
      Index_t m_num_records;
  };

  // Getting columns.
  template <typename T>
  inline void FitsTabularData::getCellGeneric(int col_num, Index_t record_index, T & value) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fitsfile * fp = m_extension.getFitsFp();
    fits_read_col(fp, data_type_code, col_num, record_index + 1, 1, 1, 0, &value, 0, &status);
    if (status) throw TableException();
  }

  // Getting column values as bools is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsTabularData::getCellGeneric<bool>(int col_num, Index_t record_index, bool & value) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = 0;
    fitsfile * fp = m_extension.getFitsFp();
    fits_read_col(fp, data_type_code, col_num, record_index + 1, 1, 1, 0, &tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

  // Getting column values as strings is a special case because Cfitsio gets them as char *.
  template <>
  inline void FitsTabularData::getCellGeneric<std::string>(int col_num, Index_t record_index, std::string & value) const {
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[s_max_scalar_len];
    fitsfile * fp = m_extension.getFitsFp();
    fits_read_col(fp, data_type_code, col_num, record_index + 1, 1, 1, 0, tmp, 0, &status);
    if (status) throw TableException();
    value = tmp;
  }

  // Getting vector-valued columns.
  // T must be a vector type.
  template <typename T>
  inline void FitsTabularData::getVectorCellGeneric(int col_num, Index_t record_index, T & vec) const {
    static int data_type_code = FitsPrimProps<typename T::value_type>::dataTypeCode();
    int status = 0;
    long repeat = m_col_num_lookup.find(col_num)->second.m_repeat;
    fitsfile * fp = m_extension.getFitsFp();
    vec.reserve(repeat);
    // Note the following depends on vectors being contiguous in memory. The address of the first vector
    // item is passed directly to fitsio. The C++ standard does not guarantee this, however, so this should
    // be rewritten to use a native primitive array somehow.
    fits_read_col(fp, data_type_code, col_num, record_index + 1, 1, repeat, 0, &*vec.begin(), 0, &status);
    if (status) throw TableException();
  }

}

#endif
