/** \file Hist2D.h
    \brief Two dimensional histogram.
*/
#ifndef evtbin_Hist2D_h
#define evtbin_Hist2D_h

#include <vector>

#include "evtbin/Hist.h"

namespace evtbin {

  class Binner;

  /** \class Hist2D
      \brief One dimensional histogram.
  */
  class Hist2D : public Hist {
    public:
      typedef std::vector< std::vector<unsigned long> > Cont_t;
      typedef Cont_t::const_iterator ConstIterator1;
      typedef std::vector<unsigned long>::const_iterator ConstIterator2;

      /** \brief Create a one dimensional histogram which uses the given binner objects
          to determine the indices.
          \param binner1 The binner object for the first dimension.
          \param binner2 The binner object for the second dimension.
      */
      Hist2D(const Binner & binner1, const Binner & binner2);

      virtual ~Hist2D() throw();

      /** \brief Increment the bin appropriate for the given value.
                 This is generic for N-dimensional histograms.
          \param value Vector giving the value being binned. The vector must have at least as
                 many values as the dimensionality of the histogram.
      */
      virtual void fillBin(const std::vector<double> & value);

      /** \brief Increment the bin appropriate for the given value.
          \param value1 The value being binned by the first binner.
          \param value2 The value being binned by the second binner.
      */
      void fillBin(double value1, double value2);

      const std::vector<unsigned long> & operator [](Cont_t::size_type index) const;

      ConstIterator1 begin() const;

      ConstIterator1 end() const;

    private:
      Cont_t m_data;
  };

  inline const std::vector<unsigned long> & Hist2D::operator [](Cont_t::size_type index) const { return m_data[index]; }

  inline Hist2D::ConstIterator1 Hist2D::begin() const { return m_data.begin(); }

  inline Hist2D::ConstIterator1 Hist2D::end() const { return m_data.end(); }

}

#endif
