/** \file Table.h

    \brief High level encapsulation of tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_Table_h
#define table_Table_h

#include <cassert>
#include <map>
#include <string>

#include "table/Ref.h"
#include "table/ITabularData.h"
#include "table/table_types.h"

namespace table {

  /** \class TableException

      \brief Exceptions thrown by table objects.
  */
  class TableException : public std::exception {
    public:
      virtual const char * what() const throw() { return "Table component exception"; }
  };

  /** \class Table

      \brief High level encapsulation of tabular data, viewed as a container of Records. Access is mainly
      attained through STL-style iterators.
  */
  class Table {
    public:
      /** \brief Open a file for reading and writing. An appropriate subobject of Table for supported
          file formats will be returned.

          If no file of any supported format can be opened, a TableException is thrown.

          \param file_name Name of any supported file type.
          \param table_name Name of desired table within the file.
      */
      static Table * openReadWrite(const std::string & file_name, const std::string & table_name);

      class Record;

      /** \class Cell

          \brief Encapsulation of a single table cell, which may contain (in principle) any type of object.

          The read() method will need to be overloaded for every type the Cell may in practice contain.
      */
      class Cell {
        public:
          /** \brief Construct a Cell object associated with a particular field in the given Record.
              \param record The referent Record object.
              \param field The name of this Cell (the field in the Record).
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

          /** \brief Construct a Record object, without immediate association with any ITabularData.
              Such an association may be formed later by assignment.
          */
          Record(): m_cells(), m_tab_data(0), m_index(0) {}

          /** \brief Construct a Record object, associated with the given ITabularData and record index.
          */
          Record(ITabularData & tab_data, Index_t index): m_cells(), m_tab_data(&tab_data), m_index(index) {}

          virtual ~Record() {}

          /** \brief Assignment operator. Note that this behaves somewhat unusually!

              This assignment does not change this Record's container of Cell objects.
              This is necessary in order to preserve the selected content of this Record. The
              idea is that assignment changes which ITabularData and record index this Record points to
              but does not affect which fields were selected in this Record. This way client code
              can be certain of the continued validity of references to Cells contained in this Record.
          */
          Record & operator =(const Record & rec);

          /** \brief Return a Cell object for the given field. The Cell object will be created
              if it does not already exist.

              Note that there is no check at this point whether the underlying ITabularData actually
              has a field with this name.
              \param field The name of the Cell object (the field in this Record).
          */
          Cell & operator [](const std::string & field) { return find_or_make(field); }

          /** \brief Return a const Cell object for the given field. The Cell object will be created
              if it does not already exist.

              Note that there is no check at this point whether the underlying ITabularData actually
              has a field with this name.
              \param field The name of the Cell object (the field in this Record).
          */
          const Cell & operator [](const std::string & field) const {
            Record * This = const_cast<Record *>(this);
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
          Cell & find_or_make(const std::string & field);
          CellCont_t m_cells;
          ITabularData * m_tab_data;
          Index_t m_index;
      };

      /** \class Iterator

          \brief STL-style iterator which points to a position (record index) within the ITabularData.
      */
      class Iterator {
        public:
          /** \brief Create an Iterator object which is not immediately associated with a table. Assignment
              to this Iterator can form such an association.
          */
          Iterator(): m_record() {}

          /** \brief Create an Iterator which does refer to the given table and index.
              \param tab_data Pointer to the referent ITabularData object.
              \param record_index The index indicating the position of this iterator within the table.
          */
          Iterator(ITabularData * tab_data, Index_t record_index): m_record(*tab_data, record_index) {}

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
            { return (m_record.getTabularData() != itor.m_record.getTabularData() ||
              m_record.getIndex() != itor.m_record.getIndex()); }

          /** \brief Get the Record object this Iterator contains.
          */
          Record & operator *() { return m_record; }

          /** \brief Get the const Record object this Iterator contains.
          */
          const Record & operator *() const { return m_record; }

          /** \brief Get a pointer to the Record object this Iterator contains.
          */
          Record * operator ->() { return &m_record; }

          /** \brief Get a pointer to the const Record this Iterator contains.
          */
          const Record * operator ->() const { return &m_record; }

        private:
          Record m_record;
          ITabularData * m_tab_data;
      };

      Table(ITabularData * tab_data): m_tab_data(tab_data) {}

      virtual ~Table() { delete m_tab_data; }

      /** \brief Return an iterator pointing to the first record in the table.
      */
      Iterator begin() { return Iterator(m_tab_data, 0); }

      /** \brief Return an iterator pointing past the last record in the table.
      */
      Iterator end() { return Iterator(m_tab_data, m_tab_data->getNumRecords()); }

      Index_t getNumRecords() const { return m_tab_data->getNumRecords(); }

    private:
      ITabularData * m_tab_data;
  };

  inline void Table::Cell::read(double & value) const
    { m_record.getTabularData()->read(m_field, m_record.getIndex(), value); }

  inline Table::Record & Table::Record::operator =(const Table::Record & rec) {
    if (this != &rec) {
      // Note: do *not* assign m_cells! This is important to how Records work!
      m_tab_data = rec.m_tab_data; // This should also confirm that the new table has the right fields.
      m_index = rec.m_index;
    }
    return *this;
  }

  inline Table::Cell & Table::Record::find_or_make(const std::string & field) {
    CellCont_t::iterator itor = m_cells.find(field);
    if (m_cells.end() == itor) itor = m_cells.insert(itor, std::make_pair(field, Cell(*this, field)));
    return itor->second;
  }

  inline Table::Iterator & Table::Iterator::operator =(const Table::Iterator & itor) {
    if (this != &itor) {
      m_record = itor.m_record;
      m_tab_data = itor.m_tab_data;
    }
    return *this;
  }

  typedef Table::Record Record;

  template <typename T>
  class Ref : public ReferenceAdaptor<T, Table::Cell> {
    public:
      Ref(Table::Cell & cell): ReferenceAdaptor<T, Table::Cell>(cell) {}

      Ref & operator =(const T & data) { ReferenceAdaptor<T, Table::Cell>::operator =(data); return *this; }
  };

}

#endif
