/** \file ITabularData.h

    \brief Low level abstract interface to tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_ITabularData_h
#define table_ITabularData_h

#include <string>

#include "table/IExtensionData.h"
#include "table/table_types.h"

namespace table {

  /** \class ITabularData

      \brief Low level abstract interface to tabular data.
  */
  class ITabularData : public IExtensionData {
    public:
      virtual ~ITabularData() {}

      /** \brief Return the number of records in the current tabular data object.
      */
      virtual Index_t getNumRecords() const = 0;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const = 0;

      /** \brief Get a value from the current tabular data object.
          \param field_index The index of the field (column) to get.
          \param record_index The index whose value to get.
          \param value The output value.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, double & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed long & value) const = 0;
  };

}

#endif
