/** \file FitsTable.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <sstream>

#include "FitsColumn.h"
#include "FitsTable.h"
#include "tip/TipException.h"

namespace tip {

  FitsTable::FitsTable(const std::string & file_name, const std::string & ext_name,
    const std::string & filter, bool read_only): m_header(file_name, ext_name, filter, read_only),
    m_file_name(file_name), m_filter(filter), m_col_name_lookup(), m_fields(), m_columns(),
    m_num_records(0) { openTable(); }

  // Close file automatically while destructing.
  FitsTable::~FitsTable() { close(); }

  // Close file.
  void FitsTable::close(int status) {
    for (std::vector<IColumn *>::reverse_iterator itor = m_columns.rbegin(); itor != m_columns.rend(); ++itor) delete *itor;
    m_fields.clear();
    m_col_name_lookup.clear();
    m_header.close(status);
  }

  Header & FitsTable::getHeader() { return m_header; }

  const Header & FitsTable::getHeader() const { return m_header; }

  const std::string & FitsTable::getName() const { return m_header.getName(); }

  void FitsTable::setName(const std::string & name) { m_header.setName(name); }

  Index_t FitsTable::getNumRecords() const {
    return m_num_records;
  }

  // Resize the FITS table, adding or deleting rows as necessary.
  void FitsTable::setNumRecords(Index_t num_records) {
    if (m_header.readOnly()) throw TipException(formatWhat("setNumRecords called, but object is not writable"));
    int status = 0;
    if (m_num_records < num_records) {
      fits_insert_rows(m_header.getFp(), m_num_records, num_records - m_num_records, &status);
      if (0 != status) throw TipException(formatWhat("setNumRecords could not insert rows in FITS table"));
      m_num_records = num_records;
    } else if (m_num_records > num_records) {
      fits_delete_rows(m_header.getFp(), num_records + 1, m_num_records - num_records, &status);
      if (0 != status) throw TipException(formatWhat("setNumRecords could not delete rows from FITS table"));
      m_num_records = num_records;
    }
  }

  const Table::FieldCont & FitsTable::getValidFields() const { return m_fields; }

  IColumn * FitsTable::getColumn(FieldIndex_t field_index) {
    if (0 > field_index || m_columns.size() <= std::vector<IColumn*>::size_type(field_index))
      throw TipException(formatWhat("FitsTable::getColumn called with invalid index"));
    return m_columns[field_index];
  }

  const IColumn * FitsTable::getColumn(FieldIndex_t field_index) const {
    if (0 > field_index || m_columns.size() <= std::vector<IColumn*>::size_type(field_index))
      throw TipException(formatWhat("FitsTable::getColumn const called with invalid index"));
    return m_columns[field_index];
  }

  FieldIndex_t FitsTable::getFieldIndex(const std::string & field_name) const {
    // Copy field name and make it lowercase.
    std::string lc_name = field_name;
    for (std::string::iterator itor = lc_name.begin(); itor != lc_name.end(); ++itor) *itor = tolower(*itor);

    // Find (lowercased) field_name in container of columns. Complain if not found.
    std::map<std::string, FieldIndex_t>::const_iterator itor = m_col_name_lookup.find(lc_name);
    if (itor == m_col_name_lookup.end())
      throw TipException(formatWhat(std::string("Could not get field index for field ") + lc_name));

    // Get the number of the column.
    return itor->second;
  }

  // Append field to a table extension.
  void FitsTable::appendField(const std::string & field_name, const std::string & format) {
    // Make a lowercase copy of field name for comparison purposes:
    std::string lc_name = field_name;
    for (std::string::iterator itor = lc_name.begin(); itor != lc_name.end(); ++itor) *itor = tolower(*itor);

    // Do not append a new column with the same name as an existing column:
    if (m_fields.end() != std::find(m_fields.begin(), m_fields.end(), lc_name))
      throw TipException(formatWhat(std::string("Cannot add field ") + field_name + " because field " +
        getColumn(m_col_name_lookup[lc_name])->getId() + " already exists"));

    int status = 0;
    int col_num = m_fields.size() + 1;

    // Call cfitsio to insert the field (column). Note: respect original case of client:
    fits_insert_col(m_header.getFp(), col_num, const_cast<char *>(field_name.c_str()), const_cast<char *>(format.c_str()), &status);
    if (0 != status) {
      std::ostringstream os;
      os << "Could not insert field " << field_name << " with form " << format;
      throw TipException(formatWhat(os.str()));
    }

    // Get all pertinent info about the new column:
    getColumnInfo(field_name, col_num);
  }

  void FitsTable::filterRows(const std::string & filter) {
    // A blank filter is treated as a no-op.
    if (std::string::npos == filter.find_first_not_of(" \t\n")) return;

    int status = 0;
    fits_select_rows(m_header.getFp(), m_header.getFp(), const_cast<char *>(filter.c_str()), &status);
    if (0 != status) throw TipException(formatWhat("filterRows had an error applying the filtering expression " + filter));

    // Read the number of rows present in the table.
    long nrows = 0;
    fits_get_num_rows(m_header.getFp(), &nrows, &status);

    // Check for success and if not, do not continue.
    if (0 != status) {
      close(status);
      throw TipException(formatWhat("Cannot get number of rows"));
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

  }

  void FitsTable::openTable() {
    // Check whether the file pointer is pointing at a table:
    if (!m_header.isTable()) {
      close();
      throw TipException(formatWhat("HDU is not a table"));
    }

    int status = 0;
    int column_status = 0;
    long nrows = 0;

    // Read the number of rows present in the table.
    fits_get_num_rows(m_header.getFp(), &nrows, &status);

    // Check for success and if not, do not continue.
    if (0 != status) {
      close(status);
      throw TipException(formatWhat("Cannot get number of rows"));
    }

    // Save the number of rows.
    m_num_records = (Index_t) nrows;

    char * match_all = "*";
    char name[128]; // jp fix this: what is the maximum length of a FITS column name?

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != column_status) {
      *name = '\0';
      int col_num = 0;
      // Get each column's name.
      fits_get_colname(m_header.getFp(), CASESEN, match_all, name, &col_num, &column_status);
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

  void FitsTable::getColumnInfo(const std::string & col_name, Index_t col_num) {
    int type_code = 0;
    int status = 0;

    // Get column type for this column number.
    fits_get_coltype(m_header.getFp(), col_num, &type_code, 0, 0, &status);
    if (0 != status) {
      std::ostringstream s;
      s << "Could not get type information for column number " << col_num;
      throw TipException(formatWhat(s.str()));
    }

    // Handle variable-length column specifiers.
    if (0 > type_code) type_code *= -1;

    // Create column abstraction for this column.
    switch (type_code) {
      case TLOGICAL:
        m_columns.push_back(new FitsColumn<bool>(this, col_name, col_num));
        break;
      case TDOUBLE:
        m_columns.push_back(new FitsColumn<double>(this, col_name, col_num));
        break;
      case TFLOAT:
        m_columns.push_back(new FitsColumn<float>(this, col_name, col_num));
        break;
      case TBYTE:
        m_columns.push_back(new FitsColumn<char>(this, col_name, col_num));
        break;
      case TSHORT:
        m_columns.push_back(new FitsColumn<signed short>(this, col_name, col_num));
        break;
      case TINT:
        m_columns.push_back(new FitsColumn<signed int>(this, col_name, col_num));
        break;
      case TLONG:
        m_columns.push_back(new FitsColumn<signed long>(this, col_name, col_num));
        break;
      case TUSHORT:
        m_columns.push_back(new FitsColumn<unsigned short>(this, col_name, col_num));
        break;
      case TUINT:
        m_columns.push_back(new FitsColumn<unsigned int>(this, col_name, col_num));
        break;
      case TULONG:
        m_columns.push_back(new FitsColumn<unsigned long>(this, col_name, col_num));
        break;
      case TSTRING:
        m_columns.push_back(new FitsColumn<std::string>(this, col_name, col_num));
        break;
      default: {
          std::ostringstream os;
          os << "Unsupported column type " << type_code;
          throw TipException(formatWhat(os.str()));
          break;
        }
    }

    // Make a lowercase copy of field name for comparison and lookup purposes:
    std::string lc_name = col_name;
    for (std::string::iterator itor = lc_name.begin(); itor != lc_name.end(); ++itor) *itor = tolower(*itor);

    // Save column number indexed on lowercased column name:
    m_col_name_lookup[lc_name] = m_columns.size() - 1;

    // Save lower cased name of field in sequential container of field names:
    m_fields.push_back(lc_name);

  }

  std::string FitsTable::formatWhat(const std::string & msg) const {
    std::ostringstream msg_str;
    msg_str << msg;
    const std::string & ext_name(getName());
    if (!ext_name.empty()) msg_str << " in extension \"" << ext_name << '"';
    msg_str << " in file \"" << m_file_name << '"';
    return msg_str.str();
  }

}
