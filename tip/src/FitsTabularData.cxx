/** \file FitsTabularData.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include <cctype>

#include "table/TableException.h"
#include "FitsTabularData.h"

namespace table {

  FitsTabularData::FitsTabularData(const std::string & file_name, const std::string & table_name): ITabularData(),
    m_extension(file_name, table_name), m_col_info(), m_file_name(file_name), m_table_name(table_name),
    m_num_records(0) {
    int status = 0;
    long nrows = 0;

    // Open the actual file and move to the right table.
    fitsfile * fp = m_extension.open();

    // Read the number of rows present in the table.
    fits_get_num_rows(fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (status || nrows < 0) {
      m_extension.close();
      throw TableException();
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != status) {
      char * match_all = "*";
      char name[128] = ""; // jp fix this: what is the maximum length of a FITS column name?
      int col_num = 0;

      fits_get_colname(fp, CASEINSEN, match_all, name, &col_num, &status);
      if (0 == status || COL_NOT_FOUND == status || COL_NOT_UNIQUE == status) {
        for (char * itor = name; *itor; ++itor) *itor = tolower(*itor);
        m_col_info[name] = col_num;
      }
    }
    // Note: status variable is non-0 here, but that is OK -- just remember that if any code gets added here!
  }

  FitsTabularData::FitsTabularData(const FitsTabularData & table): ITabularData(), 
    m_extension(table.m_file_name, table.m_table_name), m_col_info(table.m_col_info), m_file_name(table.m_file_name),
    m_table_name(table.m_table_name), m_num_records(table.m_num_records) {
    // Copy construction requires that the new table opens the file itself separately.
    try {
      m_extension.open();
    } catch(TableException & x) {
      m_col_info.clear();
      m_num_records = 0;
      throw;
    }
  }

  FitsTabularData::~FitsTabularData() {
  }

  Index_t FitsTabularData::getNumRecords() const { return m_num_records; }

  FieldIndex_t FitsTabularData::getFieldIndex(const std::string & field_name) const {
    // Copy field name and make it lowercase.
    std::string tmp = field_name;
    for (std::string::iterator itor = tmp.begin(); itor != tmp.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, int>::const_iterator itor = m_col_info.find(tmp);
    if (itor == m_col_info.end()) throw TableException();

    // Get the number of the column.
    return itor->second;
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, bool & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, double & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, float & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, char & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, signed char & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, signed short & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, signed int & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, signed long & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, unsigned char & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, unsigned short & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, unsigned int & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getCell(FieldIndex_t field_index, Index_t record_index, unsigned long & value) const {
    m_extension.getCellGeneric(field_index, record_index, value);
  }

  void FitsTabularData::getKeyword(const std::string & name, double & value) const {
    m_extension.getKeywordGeneric<double>(name, value);
  }

}
