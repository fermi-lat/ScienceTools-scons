/** \file Table.h

    \brief High level encapsulation of tabular data.

    \author James Peachey, HEASARC
*/
#ifndef tip_Table_h
#define tip_Table_h

#include "tip/Extension.h"
#include "tip/IExtensionManager.h"
#include "tip/Iterator.h"
#include "tip/ScalarAdaptor.h"
#include "tip/TipException.h"
#include "tip/TableRecord.h"
#include "tip/VectorAdaptor.h"
#include "tip/tip_types.h"

namespace tip {

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
      typedef RandomAccessIterator<TableRecord, IndexDiff_t> Iterator;

      /** \brief Helper type: auxilliary Cell access through a type which behaves like a primitive,
          but is connected to the table cell.
      */
      template <typename T>
      class Scalar : public ScalarAdaptor<T, Cell> {
        public:
          Scalar(Cell & cell): ScalarAdaptor<T, Cell>(cell) {}
          Scalar & operator =(const T & data) { ScalarAdaptor<T, Cell>::operator =(data); return *this; }
      };

      /** \brief Helper type: auxilliary Cell access through a type which behaves like a primitive,
          but is connected to the table cell.
      */
      template <typename T>
      class Vector : public VectorAdaptor<T, Cell> {
        public:
          Vector(Cell & cell): VectorAdaptor<T, Cell>(cell) {}
      };

      Table(IExtensionManager * extension_data): Extension(extension_data), m_tab_data(0) {
        if (extension_data) m_tab_data = extension_data->getTabularData();
        else throw TipException("Table::Table(IExtensionManager *): "
          "Cannot create Table with NULL IExtensionManager pointer.");
      }

      virtual ~Table() {}

      /** \brief Return an iterator pointing to the first record in the table.
      */
      Iterator begin() { return Iterator(Record(m_tab_data, 0)); }

      /** \brief Return an iterator pointing past the last record in the table.
      */
      Iterator end() { return Iterator(Record(m_tab_data, m_tab_data->getNumRecords())); }

      /** \brief Return the number of records contained in this table.
      */
      Index_t getNumRecords() const { return m_tab_data->getNumRecords(); }

    private:
      ITabularData * m_tab_data;
  };

}

#endif
