/** \file FitsTabularData.h

    \brief FITS-specific implmentation of the ITabularData interface.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsTabularData_h
#define table_FitsTabularData_h

#include <string>

#include "table/ITabularData.h"
#include "table/table_types.h"

namespace table {

  class FitsExtensionUtils;

  /** \class FitsTabularData

      \brief FITS-specific implmentation of the ITabularData interface.
  */
  class FitsTabularData : public ITabularData {
    public:
      /** \brief Create a FitsTabularData object which refers to the given utility object.
          \param fits_utils Pointer to the FITS utility object.
      */
      FitsTabularData(FitsExtensionUtils * fits_utils);

      virtual ~FitsTabularData() {}

      /** \brief Return the number of records in the current tabular data object (the number of rows
          in the FITS file).
      */
      virtual Index_t getNumRecords() const;

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const;

      /** \brief Return the number of elements in the given field (the number of items in a vector column.
      */
      virtual Index_t getFieldNumElements(FieldIndex_t field_index) const;

      /** \brief Get one or more values from the current tabular data object.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        bool * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        double * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        float * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        char * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed char * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed short * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed int * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed long * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned char * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned short * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned int * dest_begin) const;
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned long * dest_begin) const;

      /** \brief Set one or more values in the current tabular data object.
          \param field_index The index of the field (column) to set.
          \param record_index The record index (row number) whose value to set.
          \param src_begin Index of the first element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
          \param dest_end Pointer to the first element in the output sequence.
      */
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        bool * dest_begin, bool * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        double * dest_begin, double * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        float * dest_begin, float * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        char * dest_begin, char * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed char * dest_begin, signed char * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed short * dest_begin, signed short * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed int * dest_begin, signed int * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed long * dest_begin, signed long * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned char * dest_begin, unsigned char * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned short * dest_begin, unsigned short * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned int * dest_begin, unsigned int * dest_end);
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned long * dest_begin, unsigned long * dest_end);

    private:
      FitsExtensionUtils * m_fits_utils;

  };

}

#endif
