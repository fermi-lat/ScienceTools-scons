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

      /** \brief Get a value from the current tabular data object.
          \param field The name of the field (column) to get.
          \param record_index The index whose value to get.
          \param value The output value.
      */
      virtual void getCell(const std::string & field, Index_t record_index, double & value) const = 0;
      virtual void getCell(const std::string & field, Index_t record_index, signed long & value) const = 0;

      virtual FieldIndex_t getFieldIndex(const std::string & field) const = 0;
  };

}

#endif
