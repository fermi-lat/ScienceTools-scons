/** \file FitsExtensionManager.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/

#include <algorithm>
#include <cctype>
#include <sstream>

#include "FitsExtensionManager.h"
#include "tip/TipException.h"

namespace tip {

  FitsExtensionManager::FitsExtensionManager(const std::string & file_name, const std::string & ext_name,
    const std::string & filter, bool read_only): m_file_name(file_name), m_ext_name(ext_name),
    m_filter(filter), m_col_name_lookup(), m_col_num_lookup(), m_fields(), m_num_records(0), m_fp(0),
    m_is_table(false), m_read_only(read_only) { open(); }

  // Close file automatically while destructing.
  FitsExtensionManager::~FitsExtensionManager() { close(); }

  // Subclasses call this to open the file and position it to the desired extension.
  void FitsExtensionManager::open() {
    if (0 == m_fp) {
      fitsfile * fp = 0;
      int status = 0;

      // Construct the full name of the file from file name [extension] [filter] (if any):
      std::ostringstream s;
      s << m_file_name;
      if (!m_ext_name.empty()) s << "[" << m_ext_name << "]";
      if (!m_filter.empty()) s << "[" << m_filter << "]";
      std::string file_name = s.str();

      // Try to open the fits file read-write, unless read-only mode was explicitly set before open
      // was called.
      if (!m_read_only)
        fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READWRITE, &status);

      // If opening read-write didn't work, or if read-only mode was explicitly set before open
      // was called...
      if (0 != status || m_read_only) {
        // Attempt to open the file read-only:
        status = 0;
        fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READONLY, &status);
        m_read_only = true;
      }

      if (0 != status) {
        // TODO 9. 4/2/2004: Bug in cfitsio 2.48: Check for it and warn about it. The bug causes
        // the parser not to move to the correct extension.
        // DONE 9: 4/21/2004: A warning is issued for this specific version. Nothing more can be done, really.
        float cfitsio_version = 0.;
        fits_get_version(&cfitsio_version);
        // This is surreal. A FLOATING POINT VERSION NUMBER! Checking for == doesn't work -- I tried it.
        if (2.4795 < cfitsio_version && 2.4805 > cfitsio_version)
          throw TipException(std::string("WARNING: there is a known bug in Cfitsio 2.48's extended "
            "syntax parser!\nCould not open FITS file ") + file_name);
        throw TipException(std::string("Could not open FITS file \"") + file_name + '"');
      }

      // Success: save the pointer.
      m_fp = fp;

      // Check whether the file pointer is pointing at a table:
      int hdu_type = 0;
      fits_get_hdu_type(m_fp, &hdu_type, &status);
      if (0 != status) {
        close(status);
        throw TipException(formatWhat("Could not determine the type of the HDU"));
      }
      if (ASCII_TBL == hdu_type || BINARY_TBL == hdu_type) {
        m_is_table = true;

        // If this is a table, perform other table-specific initializations:
        openTable();
      }
    }
  }

  // Close file.
  void FitsExtensionManager::close(int status) {
    if (0 != m_fp) fits_close_file(m_fp, &status);
    m_fields.clear();
    m_col_num_lookup.clear();
    m_col_name_lookup.clear();
    m_fp = 0;
  }

  Index_t FitsExtensionManager::getNumRecords() const {
    if (!m_is_table) throw TipException(formatWhat("getNumRecords called, but object is not a table"));
    return m_num_records;
  }

  // Resize the FITS table, adding or deleting rows as necessary.
  void FitsExtensionManager::setNumRecords(Index_t num_records) {
    if (!m_is_table) throw TipException(formatWhat("setNumRecords called, but object is not a table"));
    if (m_read_only) throw TipException(formatWhat("setNumRecords called, but object is not writable"));
    int status = 0;
    if (m_num_records < num_records) {
      fits_insert_rows(m_fp, m_num_records, num_records - m_num_records, &status);
      if (0 != status) throw TipException("Could not insert rows in FITS table");
      m_num_records = num_records;
    } else if (m_num_records > num_records) {
      fits_delete_rows(m_fp, num_records + 1, m_num_records - num_records, &status);
      if (0 != status) throw TipException("Could not delete rows from FITS table");
      m_num_records = num_records;
    }
  }

  const IExtensionData::FieldCont & FitsExtensionManager::getValidFields() const { return m_fields; }

  FieldIndex_t FitsExtensionManager::getFieldIndex(const std::string & field_name) const {
    if (!m_is_table) throw TipException(formatWhat("getFieldIndex called, but object is not a table"));

    // Copy field name and make it lowercase.
    std::string lc_name = field_name;
    for (std::string::iterator itor = lc_name.begin(); itor != lc_name.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, ColumnInfo>::const_iterator itor = m_col_name_lookup.find(lc_name);
    if (itor == m_col_name_lookup.end())
      throw TipException(formatWhat(std::string("Could not get field index for field ") + lc_name));

    // Get the number of the column.
    return itor->second.m_col_num;
  }

  Index_t FitsExtensionManager::getFieldNumElements(FieldIndex_t field_index, Index_t record_index) const {
    if (!m_is_table) throw TipException(formatWhat("getFieldNumElements called, but object is not a table"));

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

  // Append field to a table extension.
  void FitsExtensionManager::appendField(const std::string & field_name, const std::string & format) {
    // Confirm this is a table:
    if (!m_is_table) throw TipException(formatWhat("Cannot append field if object is not a table"));

    // Make a lowercase copy of field name for comparison purposes:
    std::string lc_name = field_name;
    for (std::string::iterator itor = lc_name.begin(); itor != lc_name.end(); ++itor) *itor = tolower(*itor);

    // Do not append a new column with the same name as an existing column:
    if (m_fields.end() != std::find(m_fields.begin(), m_fields.end(), lc_name))
      throw TipException(formatWhat(std::string("Cannot add field ") + field_name + " because field " +
        m_col_name_lookup[lc_name].m_name + " already exists"));

    int status = 0;
    int col_num = m_fields.size() + 1;

    // Call cfitsio to insert the field (column). Note: respect original case of client:
    fits_insert_col(m_fp, col_num, const_cast<char *>(field_name.c_str()), const_cast<char *>(format.c_str()), &status);
    if (0 != status) {
      std::ostringstream os;
      os << "Could not insert field " << field_name << " with form " << format;
      throw TipException(formatWhat(os.str()));
    }

    // Get all pertinent info about the new column:
    getColumnInfo(field_name, col_num);
  }

  void FitsExtensionManager::openTable() {
    int status = 0;
    int column_status = 0;
    long nrows = 0;

    // Read the number of rows present in the table.
    fits_get_num_rows(m_fp, &nrows, &status);

    // Check for success and if not, do not continue.
    if (0 != status) {
      close(status);
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
      fits_get_colname(m_fp, CASESEN, match_all, name, &col_num, &column_status);
      if (0 == column_status || COL_NOT_UNIQUE == column_status) {
        try {
          // Get all other pertinent info about the column:
          getColumnInfo(name, col_num);
        } catch(...) {
          close(status);
          throw;
        }
      }
    }
  }

  void FitsExtensionManager::getColumnInfo(const std::string & col_name, Index_t col_num) {
    int type_code = 0;
    long repeat = 0;
    int status = 0;

    // Get column type and repeat count:
    fits_get_coltype(m_fp, col_num, &type_code, &repeat, 0, &status);
    if (0 != status) {
      std::ostringstream s;
      s << "Could not get type information for column number " << col_num;
      throw TipException(formatWhat(s.str()));
    }

    // Make a lowercase copy of field name for comparison purposes:
    std::string lc_name = col_name;
    for (std::string::iterator itor = lc_name.begin(); itor != lc_name.end(); ++itor) *itor = tolower(*itor);

    // Populate an info structure with all the particulars:
    ColumnInfo info;
    info.m_name = col_name;
    info.m_col_num = col_num;
    info.m_repeat = repeat;
    info.m_type_code = type_code;

    // Save column information indexed on the lowercased name:
    m_col_name_lookup[lc_name] = info;

    // Save column information indexed on the column number:
    m_col_num_lookup[col_num] = info;

    // Save lower cased name of field in sequential container of field names:
    m_fields.push_back(lc_name);
  }

  std::string FitsExtensionManager::formatWhat(const std::string & msg) const {
    std::ostringstream msg_str;
    msg_str << msg;
    if (!m_ext_name.empty()) msg_str << " in extension \"" << m_ext_name << '"';
    msg_str << " in file \"" << m_file_name << '"';
    return msg_str.str();
  }

}
