/** \file TableIterator.h

    \brief Iterators for traversing ITabularData objects. Note: these are helper classes, probably not
    of interest to table clients.

    \author James Peachey, HEASARC
*/
#ifndef table_TableIterator_h
#define table_TableIterator_h

#include "table/TableException.h"
#include "table/TableRecord.h"
#include "table/table_types.h"

namespace table {

  class ITabularData;

  /** \class TableIterator

      \brief STL-style iterator which points to a position (record index) within the ITabularData.
  */
  class TableIterator {
    public:
      /** \brief Create an TableIterator object which is not immediately associated with a table. Assignment
          to this TableIterator can form such an association.
      */
      TableIterator(): m_record() {}

      /** \brief Create an TableIterator which does refer to the given table and index.
          \param tab_data Pointer to the referent ITabularData object.
          \param record_index The index indicating the position of this iterator within the table.
      */
      TableIterator(ITabularData * tab_data, Index_t record_index): m_record(*tab_data, record_index) {}

      /** \brief Standard assignment, which makes this object refer to the same record in the same
          table as the source, but does not copy the source iterator's TableRecord.
          \param itor The source iterator object.
      */
      TableIterator & operator =(const TableIterator & itor);

      /** \brief Go on to the next table position.
      */
      TableIterator & operator ++() { m_record.nextRecord(); return *this; }

      /** \brief Compare iterator positions. They will agree iff they point to the same record index
          of the same table.
          \param itor The iterator being compared.
      */
      bool operator !=(const TableIterator & itor) const
        { return (m_record.getTabularData() != itor.m_record.getTabularData() ||
          m_record.getIndex() != itor.m_record.getIndex()); }

      /** \brief Get the TableRecord object this TableIterator contains.
      */
      TableRecord & operator *() { return m_record; }

      /** \brief Get the const TableRecord object this TableIterator contains.
      */
      const TableRecord & operator *() const { return m_record; }

      /** \brief Get a pointer to the TableRecord object this TableIterator contains.
      */
      TableRecord * operator ->() { return &m_record; }

      /** \brief Get a pointer to the const TableRecord this TableIterator contains.
      */
      const TableRecord * operator ->() const { return &m_record; }

    private:
      TableRecord m_record;
      ITabularData * m_tab_data;
  };

  // TableIterator
  inline TableIterator & TableIterator::operator =(const TableIterator & itor) {
    if (this != &itor) {
      m_record = itor.m_record;
      m_tab_data = itor.m_tab_data;
    }
    return *this;
  }

}

#endif
