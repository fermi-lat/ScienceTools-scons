/** \file FitsExtensionUtils.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/

#include <cctype>

#include "FitsExtensionUtils.h"
#include "FitsHeaderData.h"
#include "FitsTabularData.h"
#include "table/IData.h"
#include "table/TableException.h"

namespace table {

  // Construct without opening the file.
  FitsExtensionUtils::FitsExtensionUtils(const std::string & file_name, const std::string & ext_name):
    m_file_name(file_name), m_ext_name(ext_name), m_col_name_lookup(), m_col_num_lookup(), m_num_records(0), m_fp(0) {}

  // Close file automatically while destructing.
  FitsExtensionUtils::~FitsExtensionUtils() { close(); }

  IHeaderData * FitsExtensionUtils::createHeader() { return new FitsHeaderData(this); }

  IData * FitsExtensionUtils::createData() { return new FitsTabularData(this); }

  // Subclasses call this to open the file and position it to the desired extension.
  void FitsExtensionUtils::open() {
    fitsfile * fp = 0;
    int status = 0;

    // Open the fits file.
    fits_open_file(&fp, const_cast<char *>(m_file_name.c_str()), READWRITE, &status);

    if (status) throw TableException();

    // Move to the indicated extension.
    fits_movnam_hdu(fp, ANY_HDU, const_cast<char *>(m_ext_name.c_str()), 0, &status);

    if (status) {
      fits_close_file(fp, &status);
      throw TableException();
    }

    // Success: save the pointer.
    m_fp = fp;

    return m_fp;
  }

  // Close file.
  void FitsExtensionUtils::close() {
    int status = 0;
    if (m_fp) fits_close_file(m_fp, &status);
    m_fp = 0;
  }

  void FitsExtensionUtils::openTable() {
    // Open the actual file and move to the right extension.
    if (0 == m_fp) open();

    int column_status = 0;
    int status = 0;
    long nrows = 0;

    // Read the number of rows present in the table.
    fits_get_num_rows(m_fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (status || nrows < 0) {
      close();
      throw TableException();
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    char * match_all = "*";
    char name[128]; // jp fix this: what is the maximum length of a FITS column name?
    int col_num;
    long repeat;

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != column_status) {
      *name = '\0';
      col_num = 0;
      repeat = 0;
      // Get each column's name.
      fits_get_colname(m_fp, CASEINSEN, match_all, name, &col_num, &column_status);
      if (0 == column_status || COL_NOT_UNIQUE == column_status) {
        for (char * itor = name; *itor; ++itor) *itor = tolower(*itor);

        // Also get its repeat count.
        fits_get_coltype(m_fp, col_num, 0, &repeat, 0, &status);
        if (0 != status) {
          close();
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

  FieldIndex_t FitsExtensionUtils::getFieldIndex(const std::string & field_name) const {
    // Copy field name and make it lowercase.
    std::string tmp = field_name;
    for (std::string::iterator itor = tmp.begin(); itor != tmp.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, ColumnInfo>::const_iterator itor = m_col_name_lookup.find(tmp);
    if (itor == m_col_name_lookup.end()) throw TableException();

    // Get the number of the column.
    return itor->second.m_col_num;
  }

  Index_t FitsExtensionUtils::getFieldNumElements(FieldIndex_t field_index) const {
    // Find field_index in container of columns. Complain if not found.
    std::map<FieldIndex_t, ColumnInfo>::const_iterator itor = m_col_num_lookup.find(field_index);
    if (itor == m_col_num_lookup.end()) throw TableException();
    return itor->second.m_repeat;
  }

}
