/** \file FitsTable.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include <cctype>

#include "FitsTable.h"

namespace table {

  FitsTable::FitsTable(const std::string & file_name, const std::string & table_name): Table(),
    m_file_name(file_name), m_table_name(table_name), m_fp(0), m_num_records(0) {
    int status = 0;
    long nrows = 0;

    // Open the actual file and move to the right table.
    open();

    // Read the number of rows present in the table.
    fits_get_num_rows(m_fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (status || nrows < 0) {
      close();
      throw TableException();
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != status) {
      char * match_all = "*";
      char name[128] = ""; // jp fix this: what is the maximum length of a FITS column name?
      int col_num = 0;

      fits_get_colname(m_fp, CASEINSEN, match_all, name, &col_num, &status);
      if (0 == status || COL_NOT_FOUND == status || COL_NOT_UNIQUE == status) {
        for (char * itor = name; *itor; ++itor) *itor = tolower(*itor);
        m_col_info[name] = col_num;
      }
    }
    // Note: status variable is non-0 here, but that is OK -- just remember that if any code gets added here!
  }

  FitsTable::FitsTable(const FitsTable & table): Table(), m_col_info(table.m_col_info),
    m_file_name(table.m_file_name), m_table_name(table.m_table_name), m_fp(0), m_num_records(table.m_num_records) {
    // Copy construction requires that the new table opens the file itself separately.
    try {
      open();
    } catch(TableException & x) {
      m_col_info.clear();
      m_num_records = 0;
      throw;
    }
  }

  FitsTable::~FitsTable() {
    close();
  }

  Index_t FitsTable::getNumRecords() const { return m_num_records; }

  void FitsTable::read(const std::string & field, Index_t record_index, double & value) const {
    int status = 0;
    std::map<std::string, int>::const_iterator itor = m_col_info.find(field);
    if (itor == m_col_info.end()) throw TableException();

    int col_num = itor->second;

    // Read one value at a time.
    fits_read_col(m_fp, TDOUBLE, col_num, record_index + 1, 1, 1, 0, &value, 0, &status);
  }

  void FitsTable::open() {
    fitsfile * fp = 0;
    int status = 0;

    // Open the fits file.
    fits_open_file(&fp, const_cast<char *>(m_file_name.c_str()), READWRITE, &status);

    if (status) throw TableException();

    // Move to the indicated extension.
    fits_movnam_hdu(fp, BINARY_TBL, const_cast<char *>(m_table_name.c_str()), 0, &status);

    if (status) {
      fits_close_file(fp, &status);
      throw TableException();
    }

    // Success: save the pointer.
    m_fp = fp;
  }

  void FitsTable::close() {
    int status = 0;
    if (m_fp) fits_close_file(m_fp, &status);
    m_fp = 0;
  }

}
