/** \file FitsExtensionManager.h

    \brief Utilities to help manage FITS specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef tip_FitsExtensionManager_h
#define tip_FitsExtensionManager_h

#include <cstring>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "fitsio.h"

#include "FitsPrimProps.h"
#include "tip/IColumn.h"
#include "tip/IExtensionData.h"
#include "tip/TipException.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class FitsExtensionManager

      \brief Low level interface to FITS format extensions. This is not part of the API.

      This class is a standalone utility class which encapsulates Cfitsio (fitsfile *) access. It also
      acts as a factory for creating FITS-specific header and data objects, which refer back to the
      FitsExtensionManager object which created them.
  */
  class FitsExtensionManager {
    public:
      /** \brief Create an object to provide low-level access to the given FITS extension.
          \param file_name The name of the FITS file.
          \param ext_name The name of the FITS extension.
      */
      FitsExtensionManager(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "", bool read_only = true);

      /** \brief Destructor. Closes file if it is open.
      */
      virtual ~FitsExtensionManager();

      // General support for FITS files:
      /** \brief Open the FITS file and return Cfitsio's fitsfile pointer.
      */
      void open();

      /** \brief Close the FITS file.
          \param status The status to use when closing the file. Note this is not a reference!
      */
      void close(int status = 0);

      // Header/keyword-specific support:
      /** \brief Templated function which can get keywords from a FITS table, converted to any data type.
          \param name The name of the keyword.
          \param value The variable in which the read value is placed.
      */
      template <typename T>
      void getKeywordGeneric(const std::string & name, T & value) const;

      /** \brief Templated function which can set keywords in a FITS table.
          \param name The name of the keyword.
          \param value The value to be written.
      */
      template <typename T>
      void setKeywordGeneric(const std::string & name, const T & value);

      /** \brief Get the full key record as a single string, in the form name = value / comment
          \param name The name of the key record.
          \param record The output record string.
      */
      void getKeyRecord(const std::string & name, std::string & record) const;

      /** \brief Set the full key record as a single string, in the form name = value / comment
          \param name The name of the key record.
          \param record The new record string.
      */
      void setKeyRecord(const std::string & name, const std::string & record);

      // Table-specific support:
      // Non-virtual helper functions for data object interface:

      /** \brief Return a flag indicating whether the given data object is a table.
      */
      bool isTable() const { return m_is_table; }

      /** \brief Return the number of records in the current tabular data object (the number of rows
          in the FITS file).
      */
      Index_t getNumRecords() const;

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      void setNumRecords(Index_t num_records);

      /** \brief Return a container of all field names valid for this table:
      */
      const IExtensionData::FieldCont & getValidFields() const;

      IColumn * getColumn(FieldIndex_t field_index);

      const IColumn * getColumn(FieldIndex_t field_index) const;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Copy a cell from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_field The field identifier in the source data object.
          \param src_record The record identifier in the source data object.
          \param dest_field The field identifier in this object (the destination data object).
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyCell(const IExtensionData * src_ext, FieldIndex_t src_field, Index_t src_record, FieldIndex_t dest_field,
        Index_t dest_record);

      /** \brief Copy a record from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_record The record identifier in the source data object.
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyRecord(const IExtensionData * src_ext, Index_t src_record, Index_t dest_record);

      /** \brief Append a field to the table.
          \param field_name The name of the field to append.
          \param format The format of the field to append, e.g. 1D for scalar double, 8J for vector long, etc.
           See Cfitsio documentation for details.
      */
      virtual void appendField(const std::string & field_name, const std::string & format);

      /** \brief Select rows in current table which match the given filtering criteria.
                 Note that this actualy changes the underlying table.
          \param filter The string containing the filtering expression.
      */
      virtual void filterRows(const std::string & filter);

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const;

      /** \brief Set the dimensionality of an image.
          \param dims Array holding the sizes in each dimension.
      */
      virtual void setImageDimensions(const std::vector<PixOrd_t> & dims);

      /** \brief Get a specific pixel from an image extension.
          \param x Vector of coordinates of the pixel.
          \param pixel The pixel value.
      */
      virtual void getPixel(const std::vector<PixOrd_t> & x, double & pixel) const;

      /** \brief Set a specific pixel in an image extension.
          \param x Vector of coordinates of the pixel.
          \param pixel The pixel value.
      */
      virtual void setPixel(const std::vector<PixOrd_t> & x, const double & pixel);

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array in which to store the image.
      */
      virtual void getImage(std::vector<float> & image) const;

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array which stores the image.
      */
      virtual void setImage(const std::vector<float> & image);

      fitsfile * getFp() const { return m_fp; }

      bool readOnly() const { return m_read_only; }

    protected:
      /** \brief Open the FITS table. Exceptions will be thrown if the extension does not exist, or if
          the extension is not a table. Normally this is called by open()
      */
      void openTable();

      /** \brief Get all pertinent information about a column, and store it:
          \param col_name The name of the column.
          \param col_num The number of the column in the FITS file.
      */
      void getColumnInfo(const std::string & col_name, Index_t col_num);

      /** \brief Open a FITS image. Exceptions will be thrown if the extension does not exist, or if
          the extension is not an image. Normally this is called by open()
      */
      void openImage();

    private:
      std::string formatWhat(const std::string & msg) const;

      std::string m_file_name;
      std::string m_ext_name;
      std::string m_filter;
      std::map<std::string, FieldIndex_t> m_col_name_lookup;
      IExtensionData::FieldCont m_fields;
      std::vector<PixOrd_t> m_image_dimensions;
      std::vector<IColumn *> m_columns;
      Index_t m_num_records;
      fitsfile * m_fp;
      bool m_is_table;
      bool m_read_only;
  };

  // Getting keywords.
  template <typename T>
  inline void FitsExtensionManager::getKeywordGeneric(const std::string & name, T & value) const {
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &value, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read keyword \"") + name + '"'));
  }

  // Getting keywords as bool is a special case because Cfitsio treats them as ints.
  template <>
  inline void FitsExtensionManager::getKeywordGeneric<bool>(const std::string & name, bool & value) const {
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = 0;
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read keyword \"") + name + '"'));
    value = tmp;
  }

  // Getting keywords as strings is a special case because Cfitsio treats them as char *.
  template <>
  inline void FitsExtensionManager::getKeywordGeneric<std::string>(const std::string & name, std::string & value) const {
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    fits_read_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read keyword \"") + name + '"'));
    value = tmp;
  }

  // Setting keywords.
  template <typename T>
  inline void FitsExtensionManager::setKeywordGeneric(const std::string & name, const T & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<T>::dataTypeCode();
    int status = 0;
    T tmp = value;
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  // Setting keywords as bool is a special case because Cfitsio treats them as ints.
  template <>
  inline void FitsExtensionManager::setKeywordGeneric<bool>(const std::string & name, const bool & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<bool>::dataTypeCode();
    int status = 0;
    int tmp = value;
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), &tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  // Setting keywords as strings is a special case because Cfitsio treats them as char *.
  template <>
  inline void FitsExtensionManager::setKeywordGeneric<std::string>(const std::string & name, const std::string & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<std::string>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    strncpy(tmp, value.c_str(), FLEN_KEYWORD - 1);
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  // Setting keywords as strings is a special case because Cfitsio treats them as char *.
  template <>
  inline void FitsExtensionManager::setKeywordGeneric<const char *>(const std::string & name, const char * const & value) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + "\"; object is not writable"));
    static int data_type_code = FitsPrimProps<const char *>::dataTypeCode();
    int status = 0;
    char tmp[FLEN_KEYWORD];
    strncpy(tmp, value, FLEN_KEYWORD - 1);
    fits_update_key(m_fp, data_type_code, const_cast<char *>(name.c_str()), tmp, 0, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write keyword \"") + name + '"'));
  }

  inline void FitsExtensionManager::getKeyRecord(const std::string & name, std::string & record) const {
    int status = 0;
    char tmp[FLEN_CARD];
    fits_read_card(m_fp, const_cast<char *>(name.c_str()), tmp, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot read key record \"") + name + '"'));
    record = tmp;
  }

  inline void FitsExtensionManager::setKeyRecord(const std::string & name, const std::string & record) {
    if (m_read_only)
      throw TipException(formatWhat(std::string("Cannot write key record\"") + name + "\"; object is not writable"));
    int status = 0;
    char tmp[FLEN_CARD];
    strncpy(tmp, record.c_str(), FLEN_CARD - 1);
    fits_update_card(m_fp, const_cast<char *>(name.c_str()), tmp, &status);
    if (0 != status) throw TipException(formatWhat(std::string("Cannot write key record\"") + name + '"'));
  }

  // Copying cells.
  inline void FitsExtensionManager::copyCell(const IExtensionData * src_ext, FieldIndex_t src_field, Index_t src_record,
    FieldIndex_t dest_field, Index_t dest_record) {
    getColumn(dest_field)->copy(src_ext->getColumn(src_field), src_record, dest_record);
  }

  // Copying records.
  inline void FitsExtensionManager::copyRecord(const IExtensionData * src_ext, Index_t src_record, Index_t dest_record) {
    for (IExtensionData::FieldCont::iterator itor = m_fields.begin(); itor != m_fields.end(); ++itor) {
      copyCell(src_ext, src_ext->getFieldIndex(*itor), src_record, getFieldIndex(*itor), dest_record);
    }
  }
}

#endif
