/** \file Ref.h

    \brief Utilities to streamline access to cells of data inside a table.

    \author James Peachey, HEASARC
*/
#ifndef table_Ref_h
#define table_Ref_h

namespace table {

  /** \class ReferenceAdaptor

      \brief Adaptor class which provides a convenient get/set interface to referents of data inside a table.
      Client code should not normally need to use this directly, but only specific subclasses of it.
  */
  template <typename T, typename Referent>
  class ReferenceAdaptor {
    public:
      /** \brief Construct a ReferenceAdaptor object which refers to the given Referent object.
          \param referent The Referent object.
      */
      ReferenceAdaptor(Referent & referent): m_data(), m_referent(&referent) {}

      /** \brief Assignment from Referent. This changes which Referent object this ReferenceAdpator refers to.
          \param referent The new referent Referent object.
      */
      ReferenceAdaptor & operator =(Referent & referent) { m_referent = &referent; }

      /** \brief Assignment from templated parameter type. This will write the assigned value into the
          referent to which this object refers. This does not change the Referent object this ReferenceAdaptor
          refers to.
          \param data The source value for the assignment.
      */
      ReferenceAdaptor & operator =(const T & data) { m_data = data; /* m_referent->write(data); */ return *this; }

      /** \brief Retrieve the current templated parameter data value of this object.
      */
      operator const T & () const { m_referent->get(const_cast<T &>(m_data)); return m_data; }

    private:
      T m_data;
      Referent * m_referent;
  };

}

#endif
