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

      virtual Index_t getFieldNumElements(FieldIndex_t field_index) const = 0;

      /** \brief Get a value from the current tabular data object.
          \param field_index The index of the field (column) to get.
          \param record_index The index whose value to get.
          \param value The output value.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, bool & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, double & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, float & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, char & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed char & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed short & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed int & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, signed long & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned char & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned short & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned int & value) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, unsigned long & value) const = 0;

      /** \brief Get one or more values from the current tabular data object.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        bool * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        double * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        float * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        char * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed char * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed short * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed int * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed long * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned char * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned short * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned int * dest_begin) const = 0;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned long * dest_begin) const = 0;

  };

}

#endif
