/** \file TableRecord.h

    \brief Helper classes to allow convenient and idiomatic access to ITabularData objects.

    \author James Peachey, HEASARC
*/
#ifndef table_TableRecord_h
#define table_TableRecord_h

#include <cassert>
#include <map>
#include <string>

#include "table/ITabularData.h"
#include "table/TableException.h"
#include "table/table_types.h"

namespace table {

  class TableRecord;

  /** \class TableCell

      \brief Encapsulation of a single table cell, which may contain (in principle) any type of object.

      The read() method will need to be overloaded for every type the TableCell may in practice contain.
  */
  class TableCell {
    public:
      /** \brief Construct a TableCell object associated with a particular field in the given TableRecord.
          \param record The referent TableRecord object.
          \param field The name of this TableCell (the field in the TableRecord).
      */
      TableCell(TableRecord & record, const std::string & field): m_record(record), m_field(field) {}

      virtual ~TableCell() {}

      /** \brief Read the current value of this TableCell from current iterator position.
          \param value The current value.
      */
      void read(double & value) const;

    private:
      TableRecord & m_record;
      std::string m_field;
  };

  /** \class TableRecord

      \brief Encapsulation of a single table record.
  */
  class TableRecord {
    public:
      typedef std::map<std::string, TableCell> CellCont_t;

      /** \brief Construct a TableRecord object, without immediate association with any ITabularData.
          Such an association may be formed later by assignment.
      */
      TableRecord(): m_cells(), m_tab_data(0), m_index(0) {}

      /** \brief Construct a TableRecord object, associated with the given ITabularData and record index.
      */
      TableRecord(ITabularData & tab_data, Index_t index): m_cells(), m_tab_data(&tab_data), m_index(index) {}

      virtual ~TableRecord() {}

      /** \brief Assignment operator. Note that this behaves somewhat unusually!

          This assignment does not change this TableRecord's container of TableCell objects.
          This is necessary in order to preserve the selected content of this TableRecord. The
          idea is that assignment changes which ITabularData and record index this TableRecord points to
          but does not affect which fields were selected in this TableRecord. This way client code
          can be certain of the continued validity of references to TableCells contained in this TableRecord.
      */
      TableRecord & operator =(const TableRecord & rec);

      /** \brief Return a TableCell object for the given field. The TableCell object will be created
          if it does not already exist.

          Note that there is no check at this point whether the underlying ITabularData actually
          has a field with this name.
          \param field The name of the TableCell object (the field in this TableRecord).
      */
      TableCell & operator [](const std::string & field) { return find_or_make(field); }

      /** \brief Return a const TableCell object for the given field. The TableCell object will be created
          if it does not already exist.

          Note that there is no check at this point whether the underlying ITabularData actually
          has a field with this name.
          \param field The name of the TableCell object (the field in this TableRecord).
      */
      const TableCell & operator [](const std::string & field) const {
        TableRecord * This = const_cast<TableRecord *>(this);
        if (!This) throw TableException();
        return This->find_or_make(field);
      }

      // Client code should not normally need to call methods below here.
      // Get the current record index and tab_data.
      Index_t getIndex() const { return m_index; }

      // Get the current ITabularData pointer.
      ITabularData * getTabularData() { assert(m_tab_data); return m_tab_data; }

      // Get the current ITabularData pointer.
      const ITabularData * getTabularData() const { assert(m_tab_data); return m_tab_data; }

      // Go on to the next record. Not operator ++ because this behavior might be unexpected.
      void nextRecord() { ++m_index; }

    private:
      TableCell & find_or_make(const std::string & field);
      CellCont_t m_cells;
      ITabularData * m_tab_data;
      Index_t m_index;
  };

  // TableCell
  inline void TableCell::read(double & value) const
    { m_record.getTabularData()->read(m_field, m_record.getIndex(), value); }

  // TableRecord
  inline TableRecord & TableRecord::operator =(const TableRecord & rec) {
    if (this != &rec) {
      // Note: do *not* assign m_cells! This is important to how TableRecords work!
      m_tab_data = rec.m_tab_data; // This should also confirm that the new table has the right fields.
      m_index = rec.m_index;
    }
    return *this;
  }

  inline TableCell & TableRecord::find_or_make(const std::string & field) {
    CellCont_t::iterator itor = m_cells.find(field);
    if (m_cells.end() == itor) itor = m_cells.insert(itor, std::make_pair(field, TableCell(*this, field)));
    return itor->second;
  }

}

#endif
