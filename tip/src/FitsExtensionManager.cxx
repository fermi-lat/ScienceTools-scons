/** \file FitsExtensionManager.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/

#include <cctype>

#include "FitsExtensionManager.h"
#include "tip/HeaderData.h"
#include "tip/IData.h"
#include "tip/TabularData.h"
#include "tip/TipException.h"

namespace tip {

  // Construct without opening the file.
  FitsExtensionManager::FitsExtensionManager(const std::string & file_name, const std::string & ext_name):
    m_file_name(file_name), m_ext_name(ext_name), m_col_name_lookup(), m_col_num_lookup(), m_num_records(0), m_fp(0),
    m_header(0), m_data(0) { open(); }

  // Close file automatically while destructing.
  FitsExtensionManager::~FitsExtensionManager() { delete m_data; delete m_header; close(); }

  IHeaderData * FitsExtensionManager::getHeaderData() {
    if (!m_header) m_header = new HeaderData<FitsExtensionManager>(*this);
    return m_header;
  }

  ITabularData * FitsExtensionManager::getTabularData() {
    ITabularData * retval = 0;
    if (!m_data) {
      retval = new TabularData<FitsExtensionManager>(*this);
      m_data = retval;
    }
    return retval;
  }

  // Subclasses call this to open the file and position it to the desired extension.
  void FitsExtensionManager::open() {
    if (!m_fp) {
      fitsfile * fp = 0;
      int status = 0;

      // Open the fits file.
      fits_open_file(&fp, const_cast<char *>(m_file_name.c_str()), READWRITE, &status);

      if (status) throw TipException(std::string("Could not open FITS file ") + m_file_name);

      // Move to the indicated extension.
      fits_movnam_hdu(fp, ANY_HDU, const_cast<char *>(m_ext_name.c_str()), 0, &status);

      if (status) {
        fits_close_file(fp, &status);
        throw TipException(std::string("Could not find extension ") + m_ext_name + " in file " + m_file_name);
      }

      // Success: save the pointer.
      m_fp = fp;
    }
  }

  // Close file.
  void FitsExtensionManager::close() {
    int status = 0;
    if (m_fp) fits_close_file(m_fp, &status);
    m_fp = 0;
  }

  void FitsExtensionManager::openTable() {
    // Open the actual file and move to the right extension.
    if (0 == m_fp) open();

    int column_status = 0;
    int status = 0;
    long nrows = 0;

    // Read the number of rows present in the table.
    fits_get_num_rows(m_fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (status || nrows <= 0) {
      close();
      throw TipException(formatWhat("Cannot get number of rows"));
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    char * match_all = "*";
    char name[128]; // jp fix this: what is the maximum length of a FITS column name?
    int col_num;
    int type_code;
    long repeat;

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != column_status) {
      *name = '\0';
      col_num = 0;
      type_code = 0;
      repeat = 0;
      // Get each column's name.
      fits_get_colname(m_fp, CASEINSEN, match_all, name, &col_num, &column_status);
      if (0 == column_status || COL_NOT_UNIQUE == column_status) {
        for (char * itor = name; *itor; ++itor) *itor = tolower(*itor);

        // Also get its type and repeat count.
        fits_get_coltype(m_fp, col_num, &type_code, &repeat, 0, &status);
        if (0 != status) {
          close();
          std::ostringstream s;
          s << "Could not get type information for column number " << col_num;
          throw TipException(formatWhat(s.str()));
        }

        // Save values iff successful getting all the information.
        m_col_name_lookup[name].m_name = name;
        m_col_name_lookup[name].m_col_num = col_num;
        m_col_name_lookup[name].m_repeat = repeat;
        m_col_name_lookup[name].m_type_code = type_code;
        m_col_num_lookup[col_num].m_name = name;
        m_col_num_lookup[col_num].m_col_num = col_num;
        m_col_num_lookup[col_num].m_repeat = repeat;
        m_col_num_lookup[col_num].m_type_code = type_code;
      }
    }
  }

  FieldIndex_t FitsExtensionManager::getFieldIndex(const std::string & field_name) const {
    // Copy field name and make it lowercase.
    std::string tmp = field_name;
    for (std::string::iterator itor = tmp.begin(); itor != tmp.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, ColumnInfo>::const_iterator itor = m_col_name_lookup.find(tmp);
    if (itor == m_col_name_lookup.end())
      throw TipException(formatWhat(std::string("Could not get field index for field ") + field_name));

    // Get the number of the column.
    return itor->second.m_col_num;
  }

  Index_t FitsExtensionManager::getFieldNumElements(FieldIndex_t field_index, Index_t record_index) const {
    // Find field_index in container of columns. Complain if not found.
    std::map<FieldIndex_t, ColumnInfo>::const_iterator itor = m_col_num_lookup.find(field_index);
    if (itor == m_col_num_lookup.end()) {
      std::ostringstream s;
      s << "Could not get number of elements in field number " << field_index;
      throw TipException(formatWhat(s.str()));
    }
    const ColumnInfo & info = itor->second;
    long repeat = 0;
    if (0 <= info.m_type_code) {
      // Constant length column.
      repeat = info.m_repeat;
    } else {
      // Variable length column.
      int status = 0;
      fits_read_descript(m_fp, field_index, record_index + 1, &repeat, 0, &status);
      if (0 != status) {
        std::ostringstream s;
        s << "Could not get number of elements from variable vector column " << info.m_name << ", row " << record_index;
        throw TipException(formatWhat(s.str()));
      }
    }
    return repeat;
  }

  std::string FitsExtensionManager::formatWhat(const std::string & msg) const {
    std::string retval = msg;
    if (!m_ext_name.empty()) retval += std::string(" in extension ") + m_ext_name;
    retval += " in file " + m_file_name;
    return retval;
  }

}
