/** \file VectorAdaptor.h

    \brief Utilities to streamline access to vector-valued cells of data inside a table.

    \author James Peachey, HEASARC
*/
#ifndef table_VectorAdaptor_h
#define table_VectorAdaptor_h

#include <vector>

#include "table/Ref.h"

namespace table {

  /** \class VectorAdaptor

      \brief Adaptor class which provides a convenient get/set interface to referents of data inside a table.
      Client code should not normally need to use this directly, but only specific subclasses of it.
  */
  template <typename T, typename Referent>
  class VectorAdaptor : public ReferenceAdaptor<std::vector<T>, Referent> {
    public:
      typedef ReferenceAdaptor<std::vector<T>, Referent> ScalarAdaptor;
      /** \brief Construct a VectorAdaptor object which refers to the given Referent object.
          \param referent The Referent object.
      */
      VectorAdaptor(Referent & referent): ScalarAdaptor(referent) {}

      /** \brief Assignment from Referent. This changes which Referent object this ReferenceAdpator refers to.
          \param referent The new referent Referent object.
      */
      VectorAdaptor & operator =(Referent & referent) { ScalarAdaptor::operator =(referent); }

      /** \brief Assignment from templated parameter type. This will write the assigned value into the
          referent to which this object refers. This does not change the Referent object this VectorAdaptor
          refers to.
          \param data The source value for the assignment.
      */
      VectorAdaptor & operator =(const std::vector<T> & data)
        { ScalarAdaptor::operator =(data); /* m_referent->write(data); */ return *this; }

  };

}

#endif
