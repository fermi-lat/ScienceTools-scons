/** \file Hist1D.h
    \brief One dimensional histogram.
*/
#ifndef evtbin_Hist1D_h
#define evtbin_Hist1D_h

#include <stdexcept>
#include <vector>

#include "evtbin/Binner.h"

namespace evtbin {
  /** \class Hist1D
      \brief One dimensional histogram.
  */
  class Hist1D {
    public:
      typedef std::vector<unsigned long> Cont_t;
      typedef Cont_t::const_iterator ConstIterator;

      /** \brief Create a one dimensional histogram which uses the given binner object:
          \param binner The binner object to use when filling bins.
      */
      Hist1D(const Binner * binner);

      virtual ~Hist1D() throw();

      /** \brief Increment the bin appropriate for the given value.
          \param value The value being binned.
      */
      virtual void fillBin(double value);

      ConstIterator begin() const;

      ConstIterator end() const;

    private:
      Cont_t m_data;
      const Binner * m_binner;
  };

  inline Hist1D::ConstIterator Hist1D::begin() const { return m_data.begin(); }

  inline Hist1D::ConstIterator Hist1D::end() const { return m_data.end(); }

}

#endif
