/** \file VectorAdaptor.h

    \brief Utilities to streamline access to vector-valued cells of data inside a table.

    \author James Peachey, HEASARC
*/
#ifndef table_VectorAdaptor_h
#define table_VectorAdaptor_h

#include <cassert>
#include <vector>

#include "table/Ref.h"
#include "table/table_types.h"

namespace table {

  /** \class VectorAdaptor

      \brief Adaptor class which provides a convenient get/set interface to referents of data inside a table.
      Client code should not normally need to use this directly, but only specific subclasses of it.
  */
  template <typename T, typename Referent>
  class VectorAdaptor {
    public:
      class Entry {
        public:
          Entry(VectorAdaptor & adaptor, T * current): m_adaptor(adaptor), m_current(current) {}
          Entry & operator =(const T & data) { *m_current = data; m_adaptor.setModified(); return *this; }
          operator const T &() const { return *m_current; }
          T * getCurrent() { return m_current; }
          const T * getCurrent() const { return m_current; }
          void setCurrent(T * current) { m_current = current; }
          Entry & itorNext() { ++m_current; return *this; }
          Entry & itorPrev() { --m_current; return *this; }
          bool itorEquals(const Entry & entry) const { return m_current == entry; }
          bool itorLessThan(const Entry & entry) const { return m_current < entry; }
          bool itorGreaterThan(const Entry & entry) const { return m_current > entry; }

        private:
          VectorAdaptor & m_adaptor;
          T * m_current;
      };

//      typedef RandomAccessIterator<Entry, IndexDiff_t> Iterator;

      VectorAdaptor(Referent & referent): m_referent(&referent), m_entry(*this, 0), m_begin(0), m_end(0), m_modified(false) {}

      // Need to fix this:
      VectorAdaptor(const VectorAdaptor & adaptor): m_referent(adaptor.m_referent), m_entry(*this, 0), m_begin(0),
        m_end(0), m_modified(false) { assert(0); }

      ~VectorAdaptor() { delete [] m_begin; }

      // Need to fix this:
      VectorAdaptor & operator =(const VectorAdaptor & adaptor) { assert(0); return *this; }

      Entry & operator [](Index_t entry_index) { return getEntry(entry_index); }

      const Entry & operator [](Index_t entry_index) const
        { VectorAdaptor & self = const_cast<VectorAdaptor &>(*this); return self.getEntry(entry_index); }

      void setModified(bool modified = true) { m_modified = modified; }

      void allocate();

      Entry & getEntry(Index_t entry_index);

      Index_t getNumElements() const { return m_referent->getNumElements(); }

    private:
      Referent * m_referent;
      Entry m_entry;
      T * m_begin;
      T * m_end;
      bool m_modified;
  };

  template <typename T, typename Referent>
  void VectorAdaptor<T, Referent>::allocate() {
    Index_t vec_size = m_referent->getNumElements();
    m_begin = new T[vec_size];
    m_end = m_begin + vec_size;
  }

  template <typename T, typename Referent>
  typename VectorAdaptor<T, Referent>::Entry & VectorAdaptor<T, Referent>::getEntry(Index_t entry_index) {
    if (!m_begin) allocate();
    m_referent->get(0, m_end - m_begin, m_begin);
    m_entry.setCurrent(m_begin + entry_index);
    return m_entry;
  }

}

#endif
