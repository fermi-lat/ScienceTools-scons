/** \file TableRecord.h

    \brief Helper classes to allow convenient and idiomatic access to ITabularData objects.

    \author James Peachey, HEASARC
*/
#ifndef tip_TableRecord_h
#define tip_TableRecord_h

#include <cassert>
#include <map>
#include <string>

#include "tip/ITabularData.h"
#include "tip/tip_types.h"

namespace tip {

  class ConstTableRecord;

  /** \class TableCell

      \brief Encapsulation of a single table cell, which may contain (in principle) any type of object.
  */
  class TableCell {
    public:
      /** \brief Construct a TableCell object associated with a particular field in the given ConstTableRecord.
          \param record The referent ConstTableRecord object.
          \param field The name of this TableCell (the field in the ConstTableRecord).
      */
      TableCell(ConstTableRecord & record, const std::string & field): m_record(record), m_field(field),
        m_field_index(s_field_unknown) {}

      virtual ~TableCell() {}

      /** \brief Get a single value from this TableCell at the current iterator position.
          The type of the converted value is given by the template parameter.
          \param value The current value.
      */
      template <typename T>
      void get(T & value) const;

      /** \brief Get a sequence of values from this TableCell at the current iterator position.
          The type of the converted sequence is given by the template parameter.
          \param src_begin Index of the first input element.
          \param src_end Index of the element after the last input element.
          \param dest_begin Pointer to the first element in the destination sequence. The caller
          is responsible for making sure adequate space is allocated.
      */
      template <typename T>
      void get(Index_t src_begin, Index_t src_end, T * dest_begin) const;

      /** \brief Set a single value in this TableCell at the current iterator position.
          The type of the source value is given by the template parameter.
          \param value The current value.
      */
      template <typename T>
      void set(const T & value);

      /** \brief Get the number of elements in the Cell.
      */
      Index_t getNumElements() const;

    private:
      static const FieldIndex_t s_field_unknown = -1;
      ConstTableRecord & m_record;
      std::string m_field;
      FieldIndex_t m_field_index;
  };

  /** \class ConstTableRecord

      \brief Encapsulation of a single table record.
  */
  class ConstTableRecord {
    public:
      typedef std::map<std::string, TableCell> CellCont_t;

      /** \brief Construct a ConstTableRecord object, without immediate association with any ITabularData.
          Such an association may be formed later by assignment.
      */
      ConstTableRecord(): m_cells(), m_tab_data(0), m_index(0) {}

      /** \brief Construct a ConstTableRecord object, without immediate association with any ITabularData.
          Such an association may be formed later by assignment.
      */
      ConstTableRecord(const ConstTableRecord & rec): m_cells(), m_tab_data(rec.m_tab_data), m_index(rec.m_index) {}

      /** \brief Construct a ConstTableRecord object, associated with the given ITabularData and record index.
      */
      ConstTableRecord(ITabularData * tab_data, Index_t index): m_cells(), m_tab_data(tab_data), m_index(index) {}

      /** \brief Assignment operator. Note that this behaves somewhat unusually!

          This assignment does not change this ConstTableRecord's container of TableCell objects.
          This is necessary in order to preserve the selected content of this ConstTableRecord. The
          idea is that assignment changes which ITabularData and record index this ConstTableRecord points to
          but does not affect which fields were selected in this ConstTableRecord. This way client code
          can be certain of the continued validity of references to TableCells contained in this ConstTableRecord.
      */
      ConstTableRecord & operator =(const ConstTableRecord & rec);

      /** \brief Return a const TableCell object for the given field. The TableCell object will be created
          if it does not already exist.

          Note that there is no check at this point whether the underlying ITabularData actually
          has a field with this name.
          \param field The name of the TableCell object (the field in this ConstTableRecord).
      */
      const TableCell & operator [](const std::string & field) const { return find_or_make(field); }

      // Client code should not normally need to call methods below here, but they are
      // public because cell and iterator abstractions need to call them.
      // Get the current record index and tab_data.
      Index_t getIndex() const { return m_index; }

      // Get the current ITabularData pointer.
      ITabularData * getTabularData() { assert(m_tab_data); return m_tab_data; }

      // Get the current ITabularData pointer.
      const ITabularData * getTabularData() const { assert(m_tab_data); return m_tab_data; }

      // The following methods are needed to support access to this class by the
      // iterator class.
      ConstTableRecord & itorNext() { ++m_index; return *this; }
      ConstTableRecord & itorPrev() { --m_index; return *this; }
      bool itorEquals(const ConstTableRecord & record) const
        { return m_tab_data == record.m_tab_data && m_index == record.m_index; }
      bool itorLessThan(const ConstTableRecord & record) const
        { return m_tab_data == record.m_tab_data && m_index < record.m_index; }
      bool itorGreaterThan(const ConstTableRecord & record) const
        { return m_tab_data == record.m_tab_data && m_index > record.m_index; }

    protected:
      TableCell & find_or_make(const std::string & field) const;

      CellCont_t m_cells;
      ITabularData * m_tab_data;
      Index_t m_index;
  };

  /** \class TableRecord

      \brief Encapsulation of a single table record.
  */
  class TableRecord : public ConstTableRecord {
    public:
      typedef ConstTableRecord::CellCont_t CellCont_t;

      /** \brief Construct a TableRecord object, without immediate association with any ITabularData.
          Such an association may be formed later by assignment.
      */
      TableRecord(): ConstTableRecord() {}

      /** \brief Construct a TableRecord object, associated with the given ITabularData and record index.
      */
      TableRecord(ITabularData * tab_data, Index_t index): ConstTableRecord(tab_data, index) {}

      /** \brief Copy construct a TableRecord object.
      */
      TableRecord(const TableRecord & rec): ConstTableRecord(rec) {}

      /** \brief Construct a TableRecord object from a ConstTableRecord object.
      */
      TableRecord(const ConstTableRecord & rec): ConstTableRecord(rec) {}

      /** \brief Assignment operator. Note that this behaves somewhat unusually!

          This assignment does not change this TableRecord's container of TableCell objects.
          This is necessary in order to preserve the selected content of this TableRecord. The
          idea is that assignment changes which ITabularData and record index this TableRecord points to
          but does not affect which fields were selected in this TableRecord. This way client code
          can be certain of the continued validity of references to TableCells contained in this TableRecord.
      */
      TableRecord & operator =(const TableRecord & rec);

      /** \brief Assignment operator. Note that this behaves somewhat unusually!

          This assignment does not change this TableRecord's container of TableCell objects.
          This is necessary in order to preserve the selected content of this TableRecord. The
          idea is that assignment changes which ITabularData and record index this TableRecord points to
          but does not affect which fields were selected in this TableRecord. This way client code
          can be certain of the continued validity of references to TableCells contained in this TableRecord.
      */
      TableRecord & operator =(const ConstTableRecord & rec);

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
          has a field with this name. Note also that this duplicates the method in the base class
          but this is necessary because the above operator [] hides the base class method.
          \param field The name of the TableCell object (the field in this ConstTableRecord).
      */
      const TableCell & operator [](const std::string & field) const { return find_or_make(field); }
  };

  // TableCell
  // get method overloads:
  template <typename T>
  inline void TableCell::get(T & value) const {
    if (m_field_index < 0)
      const_cast<FieldIndex_t &>(m_field_index) = m_record.getTabularData()->getFieldIndex(m_field);
    m_record.getTabularData()->getCell(m_field_index, m_record.getIndex(), 0, 1, &value);
  }

  template <typename T>
  inline void TableCell::get(Index_t src_begin, Index_t src_end, T * dest_begin) const {
    if (m_field_index < 0)
      const_cast<FieldIndex_t &>(m_field_index) = m_record.getTabularData()->getFieldIndex(m_field);
    m_record.getTabularData()->getCell(m_field_index, m_record.getIndex(), src_begin, src_end, dest_begin);
  }

  // set method overloads:
  template <typename T>
  inline void TableCell::set(const T & value) {
    T tmp[1]; // Need an array so that valid begin/end iterators may be passeed to the table.
    tmp[0] = value;
    if (m_field_index < 0) m_field_index = m_record.getTabularData()->getFieldIndex(m_field);
    m_record.getTabularData()->setCell(m_field_index, m_record.getIndex(), 0, tmp, tmp + 1);
  }

  inline Index_t TableCell::getNumElements() const {
    if (m_field_index < 0)
      const_cast<FieldIndex_t &>(m_field_index) = m_record.getTabularData()->getFieldIndex(m_field);
    return m_record.getTabularData()->getFieldNumElements(m_field_index, m_record.getIndex());
  }

  // ConstTableRecord
  inline ConstTableRecord & ConstTableRecord::operator =(const ConstTableRecord & rec) {
    if (this != &rec) {
      // Note: do *not* assign m_cells! This is important to how TableRecords work!
      m_tab_data = rec.m_tab_data; // This should also confirm that the new table has the right fields.
      m_index = rec.m_index;
    }
    return *this;
  }

  inline TableCell & ConstTableRecord::find_or_make(const std::string & field) const {
    ConstTableRecord & rec = const_cast<ConstTableRecord &>(*this);
    CellCont_t::iterator itor = rec.m_cells.find(field);
    if (rec.m_cells.end() == itor) itor = rec.m_cells.insert(itor, std::make_pair(field, TableCell(rec, field)));
    return itor->second;
  }

  // TableRecord
  inline TableRecord & TableRecord::operator =(const TableRecord & rec) {
    ConstTableRecord::operator =(rec);
    return *this;
  }

  inline TableRecord & TableRecord::operator =(const ConstTableRecord & rec) {
    ConstTableRecord::operator =(rec);
    return *this;
  }

}

#endif
