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

      virtual Index_t getFieldNumElements(FieldIndex_t field_index) const;

      /** \brief Get one or more values from the current tabular data object.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        bool * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        double * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        float * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        char * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed char * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed short * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed int * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed long * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned char * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned short * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned int * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned long * dest_begin) const;

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
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      template <typename T>
      void getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end, T * dest) const;

      FitsExtension m_extension;
      std::map<std::string, ColumnInfo> m_col_name_lookup;
      std::map<int, ColumnInfo> m_col_num_lookup;
      Index_t m_num_records;
  };

  // Getting columns.
  template <typename T>
  inline void FitsTabularData::getCellGeneric(int col_num, Index_t record_index, Index_t src_begin, Index_t src_end,
    T * dest_begin) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fitsfile * fp = m_extension.getFitsFp();
    fits_read_col(fp, data_type_code, col_num, record_index + 1, src_begin + 1, src_end - src_begin, 0,
      dest_begin, 0, &status);
    if (status) throw TableException();
  }

  // Getting column values as bools is a special case because Cfitsio gets them as ints.
  template <>
  inline void FitsTabularData::getCellGeneric<bool>(int col_num, Index_t record_index, Index_t src_begin,
    Index_t src_end, bool * dest) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = 0;
    fitsfile * fp = m_extension.getFitsFp();
    for (Index_t ii = src_begin; ii != src_end; ++ii) {
      fits_read_col(fp, data_type_code, col_num, record_index + 1, ii, 1, 0, &tmp, 0, &status);
      if (status) throw TableException();
      *dest++ = tmp;
    }
  }

  // Getting column values as strings is a special case because Cfitsio gets them as char *.
  template <>
  inline void FitsTabularData::getCellGeneric<std::string>(int col_num, Index_t record_index, Index_t src_begin,
    Index_t src_end, std::string * dest) const {
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[s_max_scalar_len];
    fitsfile * fp = m_extension.getFitsFp();
    for (Index_t ii = src_begin; ii != src_end; ++ii) {
      fits_read_col(fp, data_type_code, col_num, record_index + 1, ii, 1, 0, tmp, 0, &status);
      if (status) throw TableException();
      *dest++ = tmp;
    }
  }

}

#endif
