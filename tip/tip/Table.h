/** \file Table.h

    \brief High level encapsulation of tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_Table_h
#define table_Table_h

#include <cassert>
#include <map>
#include <string>

#include "table/table_types.h"

namespace table {

  /** \class Table

      \brief High level encapsulation of tabular data, viewed as a container of Records. Access is mainly
      attained through STL-style iterators.
  */
  class Table {
    public:
      class Record;

      /** \class Cell

          \brief Encapsulation of a single table cell, which may contain (in principle) any type of object.

          The read() method will need to be overloaded for every type the Cell may in practice contain.
      */
      class Cell {
        public:
          /** \brief Construct a Cell object associated with a particular field in the given Record.
              \param record The referent Record object.
              \param field The name of this cell (the field in the Record).
          */
          Cell(Record & record, const std::string & field): m_record(record), m_field(field) {}

          virtual ~Cell() {}

          /** \brief Read the current value of this Cell from current Iterator position.
              \param value The current value.
          */
          void read(double & value) const;

        private:
          Record & m_record;
          std::string m_field;
      };

      /** \class Record

          \brief Encapsulation of a single table record.
      */
      class Record {
        public:
          typedef std::map<std::string, Cell> CellCont_t;

          /** \brief Construct a Record object, without immediate association with any Table.
              Such an association may be formed later by assignment.
          */
          Record(): m_cells(), m_table(0), m_index(0) {}

          /** \brief Construct a Record object, associated with the given Table and record index.
          */
          Record(Table & table, Index_t index): m_cells(), m_table(&table), m_index(index) {}

          virtual ~Record() {}

          /** \brief Assignment operator. Note that this behaves somewhat unusually!

              This assignment does not change this Record's container of Cell objects.
              This is necessary in order to preserve the selected content of this Record. The
              idea is that assignment changes which Table and record index this Record points to
              but does not affect which fields were selected in this Record. This way client code
              can be certain of the continued validity of references to Cells contained in this Record.
          */
          Record & operator = (const Record & rec);

          /** \brief Return a cell object for the given field. The Cell object will be created
              if it does not already exist.

              Note that there is no check at this point whether the underlying Table actually
              has a field with this name.
              \param field The name of the Cell object (the field in this Record).
          */
          Cell & operator [](const std::string & field);

          // Client code should not normally need to call methods below here.
          // Get the current record index and table.
          Index_t getIndex() const { return m_index; }

          // Get the current Table pointer.
          Table * getTable() { assert(m_table); return m_table; }

          // Go on to the next record. Not operator ++ because this behavior might be unexpected.
          void nextRecord() { ++m_index; }

        private:
          CellCont_t m_cells;
          Table * m_table;
          Index_t m_index;
      };

      /** \class Iterator

          \brief STL-style iterator which points to a position (record index) within the Table.
      */
      class Iterator {
        public:
          /** \brief Create an Iterator object which is not immediately associated with a table. Assignment
              to this Iterator can form such an association.
          */
          Iterator(): m_record(), m_table(0) {}

          /** \brief Create an Iterator which does refer to the given table and index.
              \param table Pointer to the referent Table object.
              \param record_index The index indicating the position of this iterator within the table.
          */
          Iterator(Table * table, Index_t record_index): m_record(*table, record_index), m_table(table) {}

          /** \brief Standard assignment, which makes this object refer to the same record in the same
              table as the source, but does not copy the source iterator's Record.
              \param itor The source iterator object.
          */
          Iterator & operator =(const Iterator & itor);

          /** \brief Go on to the next table position.
          */
          Iterator & operator ++() { m_record.nextRecord(); return *this; }

          /** \brief Compare iterator positions. They will agree iff they point to the same record index
              of the same table.
              \param itor The iterator being compared.
          */
          bool operator !=(const Iterator & itor) const
            { return (m_table != itor.m_table || m_record.getIndex() != itor.m_record.getIndex()); }

          /** \brief Dereference to get the Record object this Iterator contains.
          */
          Record & operator *() { return m_record; }

          /** \brief Dereference to get the Record object this Iterator contains.
          */
          Record * operator ->() { return &m_record; }

        private:
          Record m_record;
          Table * m_table;
      };

      virtual ~Table() {}

      /** \brief Return an iterator pointing to the first record in the table.
      */
      Iterator begin() { return Iterator(this, 0); }

      /** \brief Return an iterator pointing past the last record in the table.
      */
      Iterator end() { return Iterator(this, getNumRecords()); }

      /** \brief Return the number of records in the current tabular data object.
      */
      virtual Index_t getNumRecords() const = 0;

      /** \brief Read a value from the current tabular data object.
          \param field The name of the field (column) to read.
          \param record_index The index whose value to read.
          \param value The output value.
      */
      virtual void read(const std::string & field, Index_t record_index, double & value) const = 0;

    private:
  };

  inline void Table::Cell::read(double & value) const
    { m_record.getTable()->read(m_field, m_record.getIndex(), value); }

  inline Table::Record & Table::Record::operator =(const Table::Record & rec) {
    if (this != &rec) {
      // Note: do *not* assign m_cells! This is important to how Records work!
      m_table = rec.m_table; // This should also confirm that the new table has the right fields.
      m_index = rec.m_index;
    }
    return *this;
  }

  inline Table::Cell & Table::Record::operator [](const std::string & field) {
    CellCont_t::iterator itor = m_cells.find(field);
    if (m_cells.end() == itor) {
      itor = m_cells.insert(itor, std::make_pair(field, Cell(*this, field)));
    }
    return itor->second;
  }

  inline Table::Iterator & Table::Iterator::operator =(const Table::Iterator & itor) {
    if (this != &itor) {
      m_record = itor.m_record;
      m_table = itor.m_table;
    }
    return *this;
  }

  typedef Table::Cell Cell;
  typedef Table::Record Record;
}

#endif
