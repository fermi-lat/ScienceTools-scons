/** \file IExtensionData.h

    \brief

    \author James Peachey, HEASARC
*/
#ifndef tip_IExtensionData_h
#define tip_IExtensionData_h

#include <string>
#include <vector>

#include "tip/IColumn.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class IExtensionData

  */
  class IExtensionData {
    public:
      typedef std::vector<std::string> FieldCont;

      virtual ~IExtensionData() {}

      /** \brief Get a keyword from this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, bool & value) const = 0;
      virtual void getKeyword(const std::string & name, double & value) const = 0;
      virtual void getKeyword(const std::string & name, float & value) const = 0;
      virtual void getKeyword(const std::string & name, char & value) const = 0;
      virtual void getKeyword(const std::string & name, signed char & value) const = 0;
      virtual void getKeyword(const std::string & name, signed short & value) const = 0;
      virtual void getKeyword(const std::string & name, signed int & value) const = 0;
      virtual void getKeyword(const std::string & name, signed long & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned char & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned short & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned int & value) const = 0;
      virtual void getKeyword(const std::string & name, unsigned long & value) const = 0;
      virtual void getKeyword(const std::string & name, std::string & value) const = 0;

      /** \brief Set a keyword in this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void setKeyword(const std::string & name, const bool & value) = 0;
      virtual void setKeyword(const std::string & name, const double & value) = 0;
      virtual void setKeyword(const std::string & name, const float & value) = 0;
      virtual void setKeyword(const std::string & name, const char & value) = 0;
      virtual void setKeyword(const std::string & name, const signed char & value) = 0;
      virtual void setKeyword(const std::string & name, const signed short & value) = 0;
      virtual void setKeyword(const std::string & name, const signed int & value) = 0;
      virtual void setKeyword(const std::string & name, const signed long & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned char & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned short & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned int & value) = 0;
      virtual void setKeyword(const std::string & name, const unsigned long & value) = 0;
      virtual void setKeyword(const std::string & name, const std::string & value) = 0;
      virtual void setKeyword(const std::string & name, const char * const & value) = 0;

      /** \brief Return a flag indicating whether the given data object is a table.
      */
      virtual bool isTable() const = 0;

      /** \brief Return the number of records in the current (tabular) extension object.
      */
      virtual Index_t getNumRecords() const = 0;

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      virtual void setNumRecords(Index_t num_records) = 0;

      /** \brief Return a container of all field names valid for this table. Note that these names
          will automatically be converted to lower case, and all lookup of field names is case
          insensitive.
      */
      virtual const FieldCont & getValidFields() const = 0;

      virtual IColumn * getColumn(FieldIndex_t field_index) = 0;

      virtual const IColumn * getColumn(FieldIndex_t field_index) const = 0;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const = 0;

      /** \brief Copy a cell from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_field The field identifier in the source data object.
          \param src_record The record identifier in the source data object.
          \param dest_field The field identifier in this object (the destination data object).
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyCell(const IExtensionData * src_ext, FieldIndex_t src_field, Index_t src_record, FieldIndex_t dest_field,
        Index_t dest_record) = 0;

      /** \brief Copy a record from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_record The record identifier in the source data object.
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyRecord(const IExtensionData * src_ext, Index_t src_record, Index_t dest_record) = 0;

      /** \brief Append a field to the table. This will fail if a field of the same name (case insensitive) already exists.
          \param field_name The name of the field to append.
          \param format The format of the field to append, e.g. 1D for scalar double, 8J for vector long, etc.
           See Cfitsio documentation for details.
      */
      virtual void appendField(const std::string & field_name, const std::string & format) = 0;

      /** \brief Select rows in current table which match the given filtering criteria.
                 Note that this actualy changes the underlying table.
          \param filter The string containing the filtering expression.
      */
      virtual void filterRows(const std::string & filter) = 0;

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const = 0;

      /** \brief Set the dimensionality of an image.
          \param dims Array holding the sizes in each dimension.
      */
      virtual void setImageDimensions(const std::vector<PixOrd_t> & dims) = 0;

      /** \brief Get a specific pixel from an image extension.
          \param x Vector of coordinates of the pixel.
          \param pixel The pixel value.
      */
      virtual void getPixel(const std::vector<PixOrd_t> & x, double & pixel) const = 0;

      /** \brief Set a specific pixel in an image extension.
          \param x Vector of coordinates of the pixel.
          \param pixel The pixel value.
      */
      virtual void setPixel(const std::vector<PixOrd_t> & x, const double & pixel) = 0;

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array in which to store the image.
      */
      virtual void getImage(std::vector<float> & image) const = 0;

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array which stores the image.
      */
      virtual void setImage(const std::vector<float> & image) = 0;
  };

}

#endif
