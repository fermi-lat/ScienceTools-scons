/** \file Table.h

    \brief High level encapsulation of tabular data.

    \author James Peachey, HEASARC
*/
#ifndef table_Table_h
#define table_Table_h

#include <string>

#include "table/Extension.h"
#include "table/Ref.h"
#include "table/TableException.h"
#include "table/TableIterator.h"
#include "table/TableRecord.h"
#include "table/table_types.h"

namespace table {

  class ITabularData;

  /** \class Table

      \brief High level encapsulation of tabular data, viewed as a container of Records. Access is mainly
      attained through STL-style iterators.
  */
  class Table : public Extension {
    public:
      /** \brief Helper type: encapsulation of a cell of data in this table.
      */
      typedef TableCell Cell;

      /** \brief Helper type: encapsulation of a record, containing cells of data in this table.
      */
      typedef TableRecord Record;

      /** \brief Helper type: table iterator.
      */
      typedef TableIterator Iterator;

      /** \brief Helper type: auxilliary Cell access through a type which behaves like a primitive,
          but is connected to the table cell.
      */
      template <typename T>
      class Ref : public ReferenceAdaptor<T, Cell> {
        public:
          Ref(Cell & cell): ReferenceAdaptor<T, Cell>(cell) {}
          Ref & operator =(const T & data) { ReferenceAdaptor<T, Cell>::operator =(data); return *this; }
      };

      /** \brief Open a file for reading and writing. An appropriate subobject of Table for supported
          file formats will be returned.

          If no file of any supported format can be opened, a TableException is thrown.

          \param file_name Name of any supported file type.
          \param table_name Name of desired table within the file.
      */
      static Table * openReadWrite(const std::string & file_name, const std::string & table_name);

      Table(ITabularData * tab_data): Extension(tab_data), m_tab_data(tab_data) {}

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

}

#endif
