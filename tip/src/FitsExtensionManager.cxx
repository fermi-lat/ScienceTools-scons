/** \file FitsExtensionManager.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/

#include <algorithm>
#include <cctype>
#include <sstream>

#include "FitsColumn.h"
#include "FitsExtensionManager.h"
#include "tip/TipException.h"

namespace tip {

  FitsExtensionManager::FitsExtensionManager(const std::string & file_name, const std::string & ext_name,
    const std::string & filter, bool read_only): m_file_name(file_name), m_ext_name(ext_name),
    m_filter(filter), m_col_name_lookup(), m_fields(), m_image_dimensions(), m_columns(),
    m_num_records(0), m_fp(0), m_is_table(false), m_read_only(read_only) { open(); }

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
            "syntax parser!\nCould not open FITS extension ") + file_name);
        throw TipException(std::string("Could not open FITS extension \"") + file_name + '"');
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
      } else {
        // Perform image-specific initializations:
        openImage();
      }
    }
  }

  // Close file.
  void FitsExtensionManager::close(int status) {
    if (0 != m_fp) fits_close_file(m_fp, &status);
    for (std::vector<IColumn *>::reverse_iterator itor = m_columns.rbegin(); itor != m_columns.rend(); ++itor) delete *itor;
    m_image_dimensions.clear();
    m_fields.clear();
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
      if (0 != status) throw TipException(formatWhat("setNumRecords could not insert rows in FITS table"));
      m_num_records = num_records;
    } else if (m_num_records > num_records) {
      fits_delete_rows(m_fp, num_records + 1, m_num_records - num_records, &status);
      if (0 != status) throw TipException(formatWhat("setNumRecords could not delete rows from FITS table"));
      m_num_records = num_records;
    }
  }

  const IExtensionData::FieldCont & FitsExtensionManager::getValidFields() const { return m_fields; }

  IColumn * FitsExtensionManager::getColumn(FieldIndex_t field_index) {
    if (!m_is_table) throw TipException(formatWhat("getColumn called, but object is not a table"));
    if (0 > field_index || m_columns.size() <= std::vector<IColumn*>::size_type(field_index))
      throw TipException(formatWhat("FitsExtensionManager::getColumn called with invalid index"));
    return m_columns[field_index];
  }

  const IColumn * FitsExtensionManager::getColumn(FieldIndex_t field_index) const {
    if (!m_is_table) throw TipException(formatWhat("getColumn const called, but object is not a table"));
    if (0 > field_index || m_columns.size() <= std::vector<IColumn*>::size_type(field_index))
      throw TipException(formatWhat("FitsExtensionManager::getColumn const called with invalid index"));
    return m_columns[field_index];
  }

  FieldIndex_t FitsExtensionManager::getFieldIndex(const std::string & field_name) const {
    if (!m_is_table) throw TipException(formatWhat("getFieldIndex called, but object is not a table"));

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
  void FitsExtensionManager::appendField(const std::string & field_name, const std::string & format) {
    // Confirm this is a table:
    if (!m_is_table) throw TipException(formatWhat("Cannot append field if object is not a table"));

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
    fits_insert_col(m_fp, col_num, const_cast<char *>(field_name.c_str()), const_cast<char *>(format.c_str()), &status);
    if (0 != status) {
      std::ostringstream os;
      os << "Could not insert field " << field_name << " with form " << format;
      throw TipException(formatWhat(os.str()));
    }

    // Get all pertinent info about the new column:
    getColumnInfo(field_name, col_num);
  }

  const std::vector<PixOrd_t> & FitsExtensionManager::getImageDimensions() const {
    if (m_is_table) throw TipException(formatWhat("getImageDimensions called but object is not an image"));
    return m_image_dimensions;
  }

  void FitsExtensionManager::setImageDimensions(const std::vector<PixOrd_t> & dims) {
    if (m_is_table) throw TipException(formatWhat("setImageDimensions called but object is not an image"));

    // Make C primitive copy of array to pass to Cfitsio.
    std::vector<PixOrd_t>::size_type naxis = dims.size();
    long * naxes = new long[naxis];

    // Copy dimensions to C array.
    for (std::vector<PixOrd_t>::size_type ii = 0; ii < naxis; ++ii)
      naxes[ii] = dims[ii];

    int status = 0;
    int bitpix = 0;

    // Get current image type, which will not be changed by the resize operation.
    fits_get_img_type(m_fp, &bitpix, &status);
    if (0 != status) throw TipException(formatWhat("setImageDimensions cannot determine image type"));

    // Resize the image.
    fits_resize_img(m_fp, bitpix, naxis, naxes, &status);
    if (0 != status) throw TipException(formatWhat("setImageDimensions cannot change image dimensions"));

    delete [] naxes;
  }

  void FitsExtensionManager::getPixel(const std::vector<PixOrd_t> & x, double & pixel) const {
    if (m_is_table) throw TipException(formatWhat("getPixel called, but object is not an image"));
    int status = 0;
    std::vector<PixOrd_t> coord = x;
    for (std::vector<PixOrd_t>::iterator itor = coord.begin(); itor != coord.end(); ++itor) ++(*itor);
    double array[2] = { 0., 0. };

    // Read the given pixel:
    fits_read_pix(m_fp, TDOUBLE, &*coord.begin(), 1, 0, array, 0, &status);
    if (0 != status) throw TipException(formatWhat("getPixel could not read pixel as a double"));

    // Copy the value just read:
    pixel = *array;
  }

  void FitsExtensionManager::setPixel(const std::vector<PixOrd_t> & x, const double & pixel) {
    if (m_is_table) throw TipException(formatWhat("setPixel called, but object is not an image"));
    if (m_read_only) throw TipException(formatWhat("setPixel called for read-only image"));
    int status = 0;
    std::vector<PixOrd_t> coord = x;
    for (std::vector<PixOrd_t>::iterator itor = coord.begin(); itor != coord.end(); ++itor) ++(*itor);
    // Copy pixel into temporary array:
    double array[2] = { pixel, 0. };

    // Write the copy to the output file:
    fits_write_pix(m_fp, TDOUBLE, &*coord.begin(), 1, array, &status);
    if (0 != status) throw TipException(formatWhat("setPixel could not write a double to a pixel"));
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

    // Iterate over columns, putting the name of each in the column container.
    while (COL_NOT_FOUND != column_status) {
      *name = '\0';
      int col_num = 0;
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
    int status = 0;

    // Get column type for this column number.
    fits_get_coltype(m_fp, col_num, &type_code, 0, 0, &status);
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

  void FitsExtensionManager::openImage() {
    int naxis = 0;
    int status = 0;

    // Get number of axes:
    fits_get_img_dim(m_fp, &naxis, &status);
    if (0 != status) throw TipException(formatWhat("Cannot get number of dimensions of image"));

    // Get naxes:
    long * naxes = new long[naxis];
    fits_get_img_size(m_fp, naxis, naxes, &status);
    if (0 != status) {
      delete [] naxes;
      throw TipException(formatWhat("Cannot get dimensions of each degree of freedom of image"));
    }

    // If we got here, we obtained all information successfully, so store it in member:
    for (int ii = 0; ii < naxis; ++ii) m_image_dimensions.push_back(naxes[ii]);
    delete [] naxes;
  }

  std::string FitsExtensionManager::formatWhat(const std::string & msg) const {
    std::ostringstream msg_str;
    msg_str << msg;
    if (!m_ext_name.empty()) msg_str << " in extension \"" << m_ext_name << '"';
    msg_str << " in file \"" << m_file_name << '"';
    return msg_str.str();
  }

}
