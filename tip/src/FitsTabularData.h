/** \file FitsTabularData.h

    \brief Low level abstract interface to Fits tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsTabularData_h
#define table_FitsTabularData_h

#include <map>
#include <string>

#include "fitsio.h"

#include "FitsExtension.h"
#include "table/ITabularData.h"
#include "table/table_types.h"

namespace table {

  /** \class FitsTabularData

      \brief Low level abstract interface to Fits tabular data.
  */
  class FitsTabularData : public ITabularData {
    public:
      /** \brief Create a new FitsTabularData object for the given table.
          \param file_name The name of the FITS file.
          \param table_name The name of the FITS table.
      */
      FitsTabularData(const std::string & file_name, const std::string & table_name);

      /** \brief Copy contructor. Reopens its own copy of the FITS file.
          \param fits_data The source object being copied.
      */
      FitsTabularData(const FitsTabularData & fits_data);

      /** \brief Destructor. Closes the FITS file.
      */
      virtual ~FitsTabularData();

      /** \brief Return the number of records in the current tabular data object.
      */
      virtual Index_t getNumRecords() const;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Get a value from the current tabular data object.
          \param field The name of the field (column) to get.
          \param record_index The index whose value to get.
          \param value The output value.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, bool & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, double & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, float & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, char & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed char & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed short & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed int & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed long & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned char & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned short & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned int & value) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned long & value) const;

      /** \brief Get a keyword from this extension object.
          \param name The name of the keyword to get from the extension object.
          \param value The output value.
      */
      virtual void getKeyword(const std::string & name, double & value) const;

    private:
      // The real FITS-specific stuff is hidden in the utility class FitsExtension, accessed
      // through the m_extension member.
      FitsExtension m_extension;
      std::map<std::string, int> m_col_info;
      std::string m_file_name;
      std::string m_table_name;
      Index_t m_num_records;
  };

}

#endif
