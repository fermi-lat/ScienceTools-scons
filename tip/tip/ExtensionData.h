/** \file ExtensionData.h

    \brief 

    \author James Peachey, HEASARC
*/
#ifndef tip_ExtensionData_h
#define tip_ExtensionData_h

#include <string>

#include "tip/IExtensionData.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class ExtensionData

  */
  template <typename ExtensionManager>
  class ExtensionData : public IExtensionData {
    public:
      ExtensionData(const std::string & file_name, const std::string & extension_name, const std::string & filter = "",
        bool read_only = true):
        m_extension_manager(file_name, extension_name, filter, read_only) {}

      virtual ~ExtensionData() {}

      /** \brief Get a keyword from this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void getKeyword(const std::string & name, bool & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, double & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, float & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, char & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed char & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed short & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed int & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, signed long & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned char & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned short & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned int & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, unsigned long & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }
      virtual void getKeyword(const std::string & name, std::string & value) const
        { m_extension_manager.getKeywordGeneric(name, value); }

      /** \brief Set a keyword in this header object.
          \param name The name of the keyword to get from the header object.
          \param value The output value of the keyword, converted to the given type.
      */
      virtual void setKeyword(const std::string & name, const bool & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const double & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const float & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const char & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed char & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed short & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed int & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const signed long & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned char & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned short & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned int & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const unsigned long & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const std::string & value)
        { m_extension_manager.setKeywordGeneric(name, value); }
      virtual void setKeyword(const std::string & name, const char * const & value)
        { m_extension_manager.setKeywordGeneric(name, value); }

      /** \brief Return a flag indicating whether the given data object is a table.
      */
      virtual bool isTable() const { return m_extension_manager.isTable(); }

      /** \brief Return the number of records in the current extension object.
      */
      virtual Index_t getNumRecords() const { return m_extension_manager.getNumRecords(); }

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      virtual void setNumRecords(Index_t num_records) { m_extension_manager.setNumRecords(num_records); }

      /** \brief Return a container of all field names valid for this table. Note that these names
          will automatically be converted to lower case, and all lookup of field names is case
          insensitive.
      */
      virtual const IExtensionData::FieldCont & getValidFields() const { return m_extension_manager.getValidFields(); }

      virtual IColumn * getColumn(FieldIndex_t field_index) { return m_extension_manager.getColumn(field_index); }

      virtual const IColumn * getColumn(FieldIndex_t field_index) const { return m_extension_manager.getColumn(field_index); }

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const
        { return m_extension_manager.getFieldIndex(field_name); }

      /** \brief Copy a cell from a source extension data object to a cell in this object.
          \param src_ext The source extension data object.
          \param src_field The field identifier in the source data object.
          \param src_record The record identifier in the source data object.
          \param dest_field The field identifier in this object (the destination data object).
          \param dest_record The record identifier in this object (the destination data object).
      */
      virtual void copyCell(const IExtensionData * src_ext, FieldIndex_t src_field, Index_t src_record, FieldIndex_t dest_field,
        Index_t dest_record) { m_extension_manager.copyCell(src_ext, src_field, src_record, dest_field, dest_record); }


      /** \brief Append a field to the table. This will fail if a field of the same name (case insensitive) already exists.
          \param field_name The name of the field to append.
          \param format The format of the field to append, e.g. 1D for scalar double, 8J for vector long, etc.
           See Cfitsio documentation for details.
      */
      virtual void appendField(const std::string & field_name, const std::string & format)
        { m_extension_manager.appendField(field_name, format); }

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const
        { return m_extension_manager.getImageDimensions(); }

      /** \brief Set the dimensionality of an image.
          \param dims Array holding the sizes in each dimension.
      */
      virtual void setImageDimensions(const std::vector<PixOrd_t> & dims)
        { return m_extension_manager.setImageDimensions(dims); }

      /** \brief Get a specific pixel from an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      virtual void getPixel(PixOrd_t x, PixOrd_t y, double & pixel) const
        { m_extension_manager.getPixel(x, y, pixel); }

      /** \brief Set a specific pixel in an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      virtual void setPixel(PixOrd_t x, PixOrd_t y, const double & pixel)
        { m_extension_manager.setPixel(x, y, pixel); }

    private:
      ExtensionManager m_extension_manager;
  };

}

#endif
