/** \file FitsTabularData.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include <cctype>

#include "table/TableException.h"
#include "FitsTabularData.h"

namespace table {

  FitsTabularData::FitsTabularData(FitsExtensionUtils * fits_utils): ITabularData(), m_col_name_lookup(),
    m_col_num_lookup(), m_num_records(0), m_fits_utils(fits_utils) {
    int column_status = 0;
    int status = 0;
    long nrows = 0;

    // Open the actual file and move to the right table.
    fitsfile * fp = m_fits_utils->open();

    // Read the number of rows present in the table.
    fits_get_num_rows(fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (status || nrows < 0) {
      m_fits_utils->close();
      throw TableException();
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != column_status) {
      char * match_all = "*";
      char name[128] = ""; // jp fix this: what is the maximum length of a FITS column name?
      int col_num = 0;
      long repeat = 0;

      // Get each column's name.
      fits_get_colname(fp, CASEINSEN, match_all, name, &col_num, &column_status);
      if (0 == column_status || COL_NOT_UNIQUE == column_status) {
        for (char * itor = name; *itor; ++itor) *itor = tolower(*itor);

        // Also get its repeat count.
        fits_get_coltype(fp, col_num, 0, &repeat, 0, &status);
        if (0 != status) {
          m_fits_utils->close();
          throw TableException();
        }

        // Save values iff successful getting all the information.
        m_col_name_lookup[name].m_name = name;
        m_col_name_lookup[name].m_col_num = col_num;
        m_col_name_lookup[name].m_repeat = repeat;
        m_col_num_lookup[col_num].m_name = name;
        m_col_num_lookup[col_num].m_col_num = col_num;
        m_col_num_lookup[col_num].m_repeat = repeat;
      }
    }
  }

  Index_t FitsTabularData::getNumRecords() const { return m_num_records; }

  FieldIndex_t FitsTabularData::getFieldIndex(const std::string & field_name) const {
    // Copy field name and make it lowercase.
    std::string tmp = field_name;
    for (std::string::iterator itor = tmp.begin(); itor != tmp.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, ColumnInfo>::const_iterator itor = m_col_name_lookup.find(tmp);
    if (itor == m_col_name_lookup.end()) throw TableException();

    // Get the number of the column.
    return itor->second.m_col_num;
  }

  Index_t FitsTabularData::getFieldNumElements(FieldIndex_t field_index) const {
    // Find field_index in container of columns. Complain if not found.
    std::map<FieldIndex_t, ColumnInfo>::const_iterator itor = m_col_num_lookup.find(field_index);
    if (itor == m_col_num_lookup.end()) throw TableException();
    return itor->second.m_repeat;
  }

  // getCell overloads.
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    bool * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    double * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    float * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    char * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed char * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed short * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed int * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    signed long * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned char * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned short * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned int * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }
  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
    unsigned long * dest_begin) const {
    getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin);
  }

  // set methods
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    bool * dest_begin, bool * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    double * dest_begin, double * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    float * dest_begin, float * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    char * dest_begin, char * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed char * dest_begin, signed char * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed short * dest_begin, signed short * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed int * dest_begin, signed int * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    signed long * dest_begin, signed long * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned char * dest_begin, unsigned char * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned short * dest_begin, unsigned short * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned int * dest_begin, unsigned int * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }
  void FitsTabularData::setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
    unsigned long * dest_begin, unsigned long * dest_end) {
    setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end);
  }

}
