/** \file TabularData.h

    \brief Templated implementation of the ITabularData interface which forwards all actions to
    a referent object of the type given by the template parameter. The allows the virtual interface
    to take advantage of templated code in the referent object.

    \author James Peachey, HEASARC
*/
#ifndef tip_TabularData_h
#define tip_TabularData_h

#include <string>

#include "tip/ITabularData.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class TabularData

      \brief Templated generic implementation of the ITabularData interface which forwards all its
      operations to the referent object of the parametrized type.
  */
  template <typename Implementor>
  class TabularData : public ITabularData {
    public:
      /** \brief Create a TabularData object which refers to the given utility object.
          \param implementor Pointer to the object which does the real work.
      */
      TabularData(Implementor & implementor): ITabularData(), m_implementor(implementor) {
        m_implementor.open();
      }

      virtual ~TabularData() {}

      /** \brief Return the number of records in the current tabular data object.
      */
      virtual Index_t getNumRecords() const { return m_implementor.getNumRecords(); }

      /* \brief Change the number of records in the current table, adding or deleting
         rows as needed.
         \param num_records The new value for the number of records in the table.
      */
      virtual void setNumRecords(Index_t num_records) { m_implementor.setNumRecords(num_records); }

      /** \brief Get an index associated with the given field (column) name.
          \param field_name The name of the field.
      */
      virtual FieldIndex_t getFieldIndex(const std::string & field_name) const
        { return m_implementor.getFieldIndex(field_name); }

      /** \brief Return the number of elements in the given field (the number of items in a vector column.
      */
      virtual Index_t getFieldNumElements(FieldIndex_t field_index, Index_t record_index = 0) const
        { return m_implementor.getFieldNumElements(field_index, record_index); }


      /** \brief Get one or more values from the current tabular data object.
          \param field_index The index of the field (column) to get.
          \param record_index The record index (row number) whose value to get.
          \param src_begin Index of the first element within the Cell.
          \param src_end Index of one element past the last element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
      */
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        bool * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        double * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        float * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        char * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed char * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed short * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed int * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        signed long * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned char * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned short * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned int * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }
      virtual void getCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin, Index_t src_end,
        unsigned long * dest_begin) const
        { m_implementor.getCellGeneric(field_index, record_index, src_begin, src_end, dest_begin); }

      /** \brief Set one or more values in the current tabular data object.
          \param field_index The index of the field (column) to set.
          \param record_index The record index (row number) whose value to set.
          \param src_begin Index of the first element within the Cell.
          \param dest_begin Pointer to the first element in the output sequence.
          \param dest_end Pointer to the first element in the output sequence.
      */
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        bool * dest_begin, bool * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        double * dest_begin, double * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        float * dest_begin, float * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        char * dest_begin, char * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed char * dest_begin, signed char * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed short * dest_begin, signed short * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed int * dest_begin, signed int * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        signed long * dest_begin, signed long * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned char * dest_begin, unsigned char * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned short * dest_begin, unsigned short * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned int * dest_begin, unsigned int * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }
      virtual void setCell(FieldIndex_t field_index, Index_t record_index, Index_t src_begin,
        unsigned long * dest_begin, unsigned long * dest_end)
        { m_implementor.setCellGeneric(field_index, record_index, src_begin, dest_begin, dest_end); }

    private:
      Implementor & m_implementor;

  };

}

#endif
