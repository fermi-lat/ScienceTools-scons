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
#include "table/ITabularData.h"

namespace table {

  /** \class Table

      \brief High level encapsulation of tabular data, viewed as a container of Records. Access is mainly
      attained through STL-style iterators.
  */
  class Table {
    public:

      /** \class Iterator

          \brief STL-style iterator which points to positions (rows) within the Table.
      */
      class Iterator {
        public:

          /** \class Cell

              \brief Encapsulation of a single table cell, which may contain (in principle) any type of information.
          */
          class Cell {
            public:
              /** \brief Construct a Cell object, associated with the given Iterator.
                  \param itor The Iterator object to which this Cell refers.
                  \param field The name of this cell.
              */
              Cell(Iterator & itor, const std::string & field): m_itor(itor), m_field(field) {}

              virtual ~Cell() {}

              /** \brief Read the current value of this Cell from current Iterator position.
                  \param value The read value.
              */
              void read(double & value) const { m_itor.getTable()->read(m_field, m_itor.getRowNum(), value); }

            private:
              // Prevent assignment for now because it's not obvious what its behavior should be;
              // Should it copy values from the source table into destination table? Or just copy
              // the field name? Should it change which iterator the destination points to?
              Cell & operator =(const Cell & cell) { return *this; }
              Iterator & m_itor;
              std::string m_field;
          };

          /** \class Record

              \brief Encapsulation of a single table record.
          */
          class Record {
            public:
              typedef std::map<std::string, Cell> CellCont_t;

              /** \brief Construct a Record object, associated with the given Iterator.
                  \param itor The referent Iterator object.
              */
              Record(Iterator & itor): m_cells(), m_itor(itor) {}

              virtual ~Record() {}

              /** \brief Return a cell object for the given field. The Cell object will be created
                  if it does not already exist.

                  Note that there is no check at this point whether the underlying Table actually 
                  has a field with this name.
                  \param field The name of the Cell object.
              */
              Cell & operator [](const std::string & field) {
                CellCont_t::iterator itor = m_cells.find(field);
                if (m_cells.end() == itor) {
                  itor = m_cells.insert(itor, std::make_pair(field, Cell(m_itor, field)));
                }
                return itor->second;
              }

            private:
              // Prevent assignment for now because it's not obvious what its behavior should be;
              // Should it copy values from the source table into destination table? Or just copy
              // the cell container? Should it change which iterator the destination points to?
              Record & operator = (const Record & rec) { return *this; }

              CellCont_t m_cells;
              Iterator & m_itor;
          };

          /** \brief Create an Iterator object which is not associated directly to a table. Assignment
              to this Iterator will form such an association.
          */
          Iterator(): m_record(*this), m_table(0), m_row_num(0) {}

          /** \brief Standard copy constructor.

              The argument is type const Iterator & to avoid temporary objects whenever possible, even
              though it's not strictly true becase the Table * pointer is copied.
              \param itor The source object.
          */
          Iterator(const Iterator & itor): m_record(*this), m_table(itor.m_table), m_row_num(itor.m_row_num) {}

          /** \brief Create an Iterator which does refer to the given table and index.
              \param table The referent Table object.
              \param row_num The index indicating the position of this iterator within the table.
          */
          Iterator(Table * table, Index_t row_num): m_record(*this), m_table(table), m_row_num(row_num) {}

          /** \brief Standard assignment, which makes this object refer to the same record in the same
              table as the source, but does not copy the source iterator's Record.

              The source object is not type const Iterator & because the destination will hereafter refer
              to (and potentially modify) the table to which the source object points.
              \param itor The source iterator object.
          */
          Iterator & operator =(const Iterator & itor) {
            if (this != &itor) {
              m_table = itor.m_table;
              m_row_num = itor.m_row_num;
            }
            return *this;
          }

          /** \brief Go on to the next table position.
          */
          Iterator & operator ++() { ++m_row_num; return *this; }

          /** \brief Compare iterator positions. They will agree iff they point to the same row of the same table.
              \param it The iterator being compared.
          */
          bool operator !=(const Iterator & itor) const
            { return (m_table != itor.m_table || m_row_num != itor.m_row_num); }

          /** \brief Dereference to get the Record object this Iterator contains.
          */
          Record & operator *() { return m_record; }

          /** \brief Dereference to get the Record object this Iterator contains.
          */
          Record * operator ->() { return &m_record; }

          // Get the current row number and table. Client code should not normally need to call these.
          Index_t getRowNum() const { return m_row_num; }
          Table * getTable() { assert(m_table); return m_table; }

        private:
          Record m_record;
          Table * m_table;
          Index_t m_row_num;
      };

      /** \brief Create a Table object from an ITabularData object.
          \param tabular_data The underlying ITabularData object.
      */
      Table(ITabularData * tabular_data): m_tabular_data(tabular_data) {}

      virtual ~Table() {}

      /** \brief Return an iterator pointing to the first record in the table.
      */
      Iterator begin() { return Iterator(this, 0); }

      /** \brief Return an iterator pointing past the last record in the table.
      */
      Iterator end() { return Iterator(this, m_tabular_data->getNumRows()); }

      /** \brief Get a value from the Table.
          \param field The name of the field (column) to read.
          \param row_num The index whose value to read.
          \pamar value The output value.
      */
      void read(const std::string & field, Index_t row_num, double & value) const
        { m_tabular_data->read(field, row_num, value); }

    private:
      ITabularData * m_tabular_data;
  };

  typedef Table::Iterator::Cell Cell;
  typedef Table::Iterator::Record Record;
}

#endif
